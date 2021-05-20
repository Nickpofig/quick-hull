
// standard
#include <iostream>
#include <sstream>
#include <vector>
#include <cstddef>

// internal
#include "core.hpp"
#include "console.hpp"
#include "algorithm/cuda.hpp"

namespace quick_hull 
{
	// Cuda error handling
	static void cuda_handle_error(cudaError_t code, const char * file, int line) 
	{
		if (code == cudaSuccess) return; 
		
		program::panic_begin 
			<< "Error(" << code << ") in" << file << " at line " << line 
			<< program::panic_end;
	}


	#define macro_cuda_call(call) (cuda_handle_error(call, __FILE__, __LINE__))


	// Cuda version of vector2 functions
	__device__ Vector2 cuda_vector_make(double x, double y)
	{
		Vector2 vector;
		
		vector.x = x;
		vector.y = y;

		return vector;
	}

	__device__ Vector2 cuda_vector_subtract(const Vector2& a, const Vector2 &b) 
	{
		return cuda_vector_make(a.x - b.x, a.y - b.y);
	}

	__device__ Vector2 cuda_vector_add(const Vector2& a, const Vector2& b) 
	{
		return cuda_vector_make(a.x + b.x, a.y + b.y);
	}

	__device__ Vector2 cuda_vector_get_normal(const Vector2& vector) 
	{
		return cuda_vector_make(-vector.y, vector.x);
	}

	__device__ double cuda_vector_get_sqr_magnitude(const Vector2& vector) 
	{
		return (vector.x * vector.x) + (vector.y * vector.y);
	}
	
	__device__ double cuda_vector_dot_product(const Vector2 &vector_a, const Vector2 &vector_b) 
	{
		return (vector_a.x * vector_b.x) + (vector_a.y * vector_b.y); 
	}

	__device__ Vector2 cuda_vector_project(const Vector2 &vector_a, const Vector2 &vector_b)
	{
		double vector_b_sqr_magnitude = cuda_vector_get_sqr_magnitude(vector_b);

		if (vector_b_sqr_magnitude == 0) 
		{
			return cuda_vector_make(0, 0);
		}
		
		double relativeness = cuda_vector_dot_product(vector_a, vector_b);

		return cuda_vector_make(
			vector_b.x * (relativeness / vector_b_sqr_magnitude), 
			vector_b.y * (relativeness / vector_b_sqr_magnitude)
		);
	}

	struct Cuda_Thread_Data
	{
		public:
			int farest_point_index;
			double farest_point_sqr_distance;
	};

	__global__
	void find_farest_point_from_line
	(
		Cuda_Thread_Data* result_point_index, // only the first thread in a block has write access
		const Vector2* points, 
		int point_count, 
		int line_point_a_index, 
		int line_point_b_index
	)
	{
		// Declares dynamic(or not) shared memory
		//extern 
		__shared__ Cuda_Thread_Data block_data[
			1024
		];

		// Defines block settings
		int block_count = gridDim.x;
		int thread_count = blockDim.x;
		int unit_count = thread_count * block_count;

		// Divides point count by number of computation units and ceil it
		int points_per_thread = (point_count + unit_count + 1) / unit_count;
		

		// Defines thread local data
		int block_id = blockIdx.x;
		int thread_id = threadIdx.x;
		int unit_index = thread_count * block_id + thread_id;
		int points_start = (unit_index) * points_per_thread;
		int points_end = (unit_index + 1) * points_per_thread;

		if (point_count < points_end) 
		{
			points_end = point_count;
		}

		double thread_farest_point_sqr_distance = 0;
		int thread_farest_point_index = line_point_a_index;

		Vector2 line_point_a = points[line_point_a_index];
		Vector2 line_point_b = points[line_point_b_index];

		for(int point_index = points_start; point_index < points_end; point_index++) 
		{
			Vector2 point = points[point_index];
			Vector2 line = cuda_vector_subtract(line_point_b, line_point_a);
			Vector2 line_normal = cuda_vector_get_normal(line);

			// projected-point on the line from the current point
			Vector2 projection_base = cuda_vector_add(
				cuda_vector_project(
					cuda_vector_subtract(point, line_point_a), 
					line
				),
				line_point_a 
			); 

			// projection (as vector) from the base to the point
			Vector2 projection = cuda_vector_subtract(point, projection_base);

			double relativity = cuda_vector_dot_product(projection, line_normal);
			double projection_sqr_magnitude = cuda_vector_get_sqr_magnitude(projection);

			if (relativity > 0 && thread_farest_point_sqr_distance < projection_sqr_magnitude)
			{
				thread_farest_point_sqr_distance = projection_sqr_magnitude;
				thread_farest_point_index = point_index;
			}
		}
		
		block_data[thread_id].farest_point_index = thread_farest_point_index;
		block_data[thread_id].farest_point_sqr_distance = thread_farest_point_sqr_distance;

		__syncthreads();

		for (int reduction_step = 2; (thread_id % reduction_step == 0) && (thread_count >= reduction_step); reduction_step <<= 1)
		{
			int supply_thread_id = thread_id + (reduction_step >> 1);

			if (supply_thread_id >= thread_count) break;

			if (block_data[thread_id].farest_point_sqr_distance < block_data[supply_thread_id].farest_point_sqr_distance) 
			{
				block_data[thread_id].farest_point_sqr_distance = block_data[supply_thread_id].farest_point_sqr_distance;
				block_data[thread_id].farest_point_index = block_data[supply_thread_id].farest_point_index;
			};

			__syncthreads();
		}

		if (thread_id == 0)
		{
			auto* copy_to   = &result_point_index[block_id];
			auto* copy_from = &block_data[0];

			copy_to->farest_point_index        = copy_from->farest_point_index;
			copy_to->farest_point_sqr_distance = copy_from->farest_point_sqr_distance;
		}
	}

	// Run over all points
	// Finds farest point from a given line
	// Add the point to convex hull

	template<int T_Block_Count, int T_Thread_Count>
	void grow
	(
		int point_a_index, 
		int point_b_index, 
		const std::vector<Vector2> & points,
		std::vector<Vector2> * convex_hull,
		Vector2* device_points_copy,
		Cuda_Thread_Data* host_far_points,
		Cuda_Thread_Data* device_far_points
	);

	template<int T_Block_Count, int T_Thread_Count>
	void grow
	(
		int point_a_index, 
		int point_b_index, 
		const std::vector<Vector2> & points,
		std::vector<Vector2> * convex_hull,
		Vector2* device_points_copy,
		Cuda_Thread_Data* host_far_points,
		Cuda_Thread_Data* device_far_points
	)
	{
		find_farest_point_from_line<<<T_Block_Count, T_Thread_Count>>>
		(
			device_far_points,
			device_points_copy,
			points.size(),
			point_a_index, 
			point_b_index
		);

		macro_cuda_call(cudaDeviceSynchronize());

		size_t far_points_memsize = T_Block_Count * sizeof(Cuda_Thread_Data);
		macro_cuda_call(cudaMemcpy(host_far_points, device_far_points, far_points_memsize, cudaMemcpyDeviceToHost));

		for (int step = 1; step < T_Block_Count; step <<= 1) 
		{
			for (int index = 0; index < T_Block_Count; index += (step << 1)) 
			{
				auto* recepient = &host_far_points[index];
				auto* donor = &host_far_points[index + step];

				if (recepient->farest_point_sqr_distance < donor->farest_point_sqr_distance)
				{
					recepient->farest_point_index = donor->farest_point_index;
					recepient->farest_point_sqr_distance = donor->farest_point_sqr_distance;
				}
			}
		}

		int point_c_index = host_far_points[0].farest_point_index;

		if (point_c_index != point_a_index && point_c_index != point_b_index)
		{
			// a convex hull from the AC line 
			grow<T_Block_Count, T_Thread_Count>(
				point_a_index, 
				point_c_index, 
				points, 
				convex_hull, 
				device_points_copy,
				host_far_points,
				device_far_points
			); 

			convex_hull->push_back(points[point_c_index]);

			// a convex hull from the CB line
			grow<T_Block_Count, T_Thread_Count>(
				point_c_index, 
				point_b_index, 
				points, 
				convex_hull,
				device_points_copy,
				host_far_points,
				device_far_points
			); 
		}
	}

	template<int T_Block_Count, int T_Thread_Count>
	std::vector<Vector2> * algorithm_internal_run
	(
		const std::vector<Vector2> &points
	)
	{
		int most_left_index  = 0;
		int most_right_index = points.size() - 1;
		
		// Finds the most left and right point
		for (int index = 0; index < points.size(); index++)
		{
			const auto point = points[index];
			const auto most_right = points[most_right_index];
			const auto most_left = points[most_left_index];

			if (point.x > most_right.x || (point.x == most_right.x && point.y > most_right.y))
			{
				most_right_index = index;
			}
			else
			if (point.x < most_left.x || (point.x == most_right.x && point.y < most_right.y)) 
			{
				most_left_index = index;
			}
		}

		auto *convex_hull = new std::vector<Vector2>();

		// Initializes cuda working memory
		Vector2          * device_points_copy;
		Cuda_Thread_Data * device_far_points;
		Cuda_Thread_Data * host_far_points;
		
		size_t far_points_memsize = (T_Block_Count) * sizeof(Cuda_Thread_Data);
		size_t points_memsize = points.size() * sizeof(Vector2);


		macro_cuda_call(cudaHostAlloc((void**) &host_far_points, far_points_memsize, cudaHostAllocDefault));
		macro_cuda_call(cudaMalloc((void**) &device_far_points, far_points_memsize));
		macro_cuda_call(cudaMalloc((void**) &device_points_copy, points_memsize));
		macro_cuda_call(cudaMemcpy(device_points_copy, points.data(), points_memsize, cudaMemcpyHostToDevice));


		// Constructs a convex from right and left side of line going through the most left and right points

		convex_hull->push_back(points[most_left_index]);
		
		grow<T_Block_Count, T_Thread_Count>(
			most_left_index, 
			most_right_index, 
			points, 
			convex_hull, 
			device_points_copy,
			host_far_points,
			device_far_points
		);
		
		convex_hull->push_back(points[most_right_index]);
		
		grow<T_Block_Count, T_Thread_Count>(
			most_right_index, 
			most_left_index,
			points, 
			convex_hull, 
			device_points_copy,
			host_far_points,
			device_far_points
		);


		macro_cuda_call(cudaFreeHost(host_far_points));
		macro_cuda_call(cudaFree(device_far_points));
		macro_cuda_call(cudaFree(device_points_copy));

		return convex_hull;
	}




	Algorithm_Cuda::~Algorithm_Cuda() { }

	std::vector<Vector2> * Algorithm_Cuda::run(const std::vector<Vector2> &points)
	{
		switch(this->block_power) 
		{
			// x1
			case 0 : return algorithm_internal_run<1,1024>(points);
			// x2
			case 1 : return algorithm_internal_run<2,512>(points);
			// x4
			case 2 : return algorithm_internal_run<4,256>(points);
			// x8
			case 3 : return algorithm_internal_run<8,128>(points);
			// x16
			default:
			case 4 : return algorithm_internal_run<16,64>(points);
			// x32
			case 5 : return algorithm_internal_run<32,32>(points); 
			// x64
			case 6 : return algorithm_internal_run<64,16>(points); 
			// x128
			case 7 : return algorithm_internal_run<128,8>(points);
			// x256
			case 8 : return algorithm_internal_run<256,4>(points);
			// x512
			case 9 : return algorithm_internal_run<512,2>(points);
			// x1024
			case 10: return algorithm_internal_run<1024,1>(points);
		}
	}
}


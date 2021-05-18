
// standard
#include <iostream>
#include <sstream>
#include <vector>
#include <cstddef>

// internal
#include "core.hpp"
#include "program.hpp"
#include "algorithm/cuda.hpp"

namespace quick_hull 
{
	// Cuda error handling
	static void cuda_handle_error(cudaError_t error, const char * file, int line) 
	{
		if (error == cudaSuccess) return; 
		
		program::panic_begin 
			<< "Error(" << error << ") in" << file << " at line " << line 
			<< program::panic_end;
	}


	#define macro_cuda_call(call) (cuda_handle_error(call, __FILE__, __LINE__))


	// Cuda version of vector2 functions
	__device__ Vector2 cuda_vector_make(const double x, const double y)
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
		return (vector.x * vector.x) + (vector.y + vector.y);
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
			int farest_point_sqr_distance;
	};

	__global__
	void find_farest_point_from_line
	(
		int* result_point_index, // only the first thread in a block has write access
		const Vector2* points, 
		int point_count, 
		Vector2 line_point_a, 
		Vector2 line_point_b
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

		double thread_farest_point_sqr_distance = ~0; // must be the minimum number in two's complement system
		int thread_farest_point_index = points_start;

		for(int point_index = points_start; point_index < points_end; point_index++) 
		{
			Vector2 point = points[point_index];
			Vector2 line = cuda_vector_subtract(line_point_a, line_point_b);
			Vector2 line_normal = cuda_vector_get_normal(line);

			// projected-point on the line from the current point
			Vector2 projection_base = cuda_vector_project(
				cuda_vector_subtract(point, line_point_a), 
				cuda_vector_add(line, line_point_a)
			); 

			// projection (as vector) from the base to the point
			Vector2 projection = cuda_vector_subtract(point, projection_base);

			double relativity = cuda_vector_dot_product(projection, line_normal);
			double projection_sqr_magnitude = cuda_vector_get_sqr_magnitude(projection);

			if (relativity < 0)
			{
				projection_sqr_magnitude = -projection_sqr_magnitude;
			}
			
			if (thread_farest_point_sqr_distance < projection_sqr_magnitude) 
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
			result_point_index[block_id] = block_data[0].farest_point_index;
		}
	}

	// Run over all points
	// Finds farest point from a given line
	// Add the point to convex hull

	void grow
	(
		Vector2 a, 
		Vector2 b, 
		const std::vector<Vector2> & points,
		std::vector<Vector2> * convex_hull
	)
	{
		int* host_far_points;
		int* device_far_points;
		Vector2* device_points_copy;

		size_t far_points_memsize = 1 * sizeof(int);
		size_t points_memsize = points.size() * sizeof(Vector2);


		program::log_begin << "Allocating device and host memory... points memsize: " << points_memsize << "." << program::log_end;

		macro_cuda_call(cudaHostAlloc((void**) &host_far_points, far_points_memsize, cudaHostAllocDefault));
		macro_cuda_call(cudaMalloc((void**) &device_far_points, far_points_memsize));
		macro_cuda_call(cudaMalloc((void**) &device_points_copy, points_memsize));

		macro_cuda_call(cudaMemcpy(device_points_copy, points.data(), points_memsize, cudaMemcpyHostToDevice));


		program::log_begin << "Executing kernel..." << program::log_end;

		find_farest_point_from_line<<<1, 1024
		//, 1024
		>>>
		(
			device_far_points,
			device_points_copy,
			points.size(),
			a,
			b
		);

		macro_cuda_call(cudaDeviceSynchronize());
		macro_cuda_call(cudaMemcpy(host_far_points, device_far_points, far_points_memsize, cudaMemcpyDeviceToHost));

		program::log_begin << "Reading result point..." << program::log_end;

		Vector2 c = points.at(host_far_points[0]);

		macro_cuda_call(cudaFreeHost(host_far_points));
		macro_cuda_call(cudaFree(device_far_points));
		macro_cuda_call(cudaFree(device_points_copy));

		if (c != a && c != b)
		{
			program::log_begin << "Growing convex hull left..." << program::log_end;
			grow(a, c, points, convex_hull); // a convex hull from the AC line 

			program::log_begin << "Adding convex hull point..." << program::log_end;
			convex_hull->push_back(c);

			program::log_begin << "Growing convex hull right..." << program::log_end;
			grow(c, b, points, convex_hull); // a convex hull from the CB line
		}
	}




	Quick_Hull_Cuda::~Quick_Hull_Cuda() { }

	std::vector<Vector2> * Quick_Hull_Cuda::run(const std::vector<Vector2> &points)
	{
		Vector2 most_left  = points.front();
		Vector2 most_right = points.back();
		
		// Finds the most left and right point
		for (const auto &point: points)
		{
			if (point.x > most_right.x || (point.x == most_right.x && point.y > most_right.y))
			{
				most_right = point;
			}
			else
			if (point.x < most_left.x || (point.x == most_right.x && point.y < most_right.y)) 
			{
				most_left  = point;
			}
		}

		// Convex hull 
		auto *convex_hull = new std::vector<Vector2>();

		// Constructs a convex from right and left side of line going through the most left and right points
		grow(most_left, most_right, points, convex_hull);
		grow(most_right, most_left, points, convex_hull);

		return convex_hull;
	}
}



// standard
#include <iostream>
#include <sstream>
#include <vector>
#include <cstddef>
#include <chrono>

// internal
#include "core.hpp"
#include "console.hpp"
#include "algorithm/cuda.hpp"

namespace quick_hull 
{
	// CUDA error handling
	static void cuda_handle_error(cudaError_t code, const char * file, int line) 
	{
		if (code == cudaSuccess) return; 
		
		program::panic_begin 
			<< "Error(" << code << ") in" << file << " at line " << line 
			<< program::panic_end;
	}

	#define macro_cuda_call(call) (cuda_handle_error(call, __FILE__, __LINE__))


	// CUDA version of vector2 functions
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
		// Declares the block memory
		__shared__ Cuda_Thread_Data block_data
		[
			1024 // ..the maximum amount of threads
		];

		// Defines dimensions
		int block_count = gridDim.x;
		int thread_count = blockDim.x;
		int unit_count = thread_count * block_count;

		// Divides point count by number of computation units and ceil it to get a per thread standard amount of points
		int points_per_thread = (point_count + unit_count + 1) / unit_count;

		// Defines thread local data
		int block_id = blockIdx.x;
		int thread_id = threadIdx.x;
		int unit_index = thread_count * block_id + thread_id;
		int points_start = (unit_index) * points_per_thread;
		int points_end = (unit_index + 1) * points_per_thread;

		// Amount of points may not be a power of two. 
		// That is why, it is necessary to trim points_end of the last thread
		if (points_end > point_count) // ..no need to check a thread id, because only the last one could satisfy this condition
		{
			points_end = point_count;
		}

		// By default, for each thread, sets point A as the farest one
		block_data[thread_id].farest_point_index = line_point_a_index;
		block_data[thread_id].farest_point_sqr_distance = 0;

		// Defines AB line and its normal towards the considered side
		Vector2 line_point_a = points[line_point_a_index];
		Vector2 line_point_b = points[line_point_b_index];
		Vector2 line = cuda_vector_subtract(line_point_b, line_point_a);
		Vector2 line_normal = cuda_vector_get_normal(line);

		// 1. Finds the farest point from AB line
		for(int point_index = points_start; point_index < points_end; point_index++) 
		{
			Vector2 point = points[point_index];

			// A point, which is a base of a projection on the AB line from the current point
			Vector2 projection_base = cuda_vector_add(
				cuda_vector_project(
					cuda_vector_subtract(point, line_point_a), 
					line
				),
				line_point_a 
			); 

			// The projection (as vector) from the base to the point
			Vector2 projection = cuda_vector_subtract(point, projection_base);

			// Relativity checks that point is on the considered side of the AB line
			double relativity = cuda_vector_dot_product(projection, line_normal);
			double projection_sqr_magnitude = cuda_vector_get_sqr_magnitude(projection);

			// Saves the farest point data
			if (relativity > 0 && block_data[thread_id].farest_point_sqr_distance < projection_sqr_magnitude)
			{
				block_data[thread_id].farest_point_index = point_index;
				block_data[thread_id].farest_point_sqr_distance = projection_sqr_magnitude;
			}
		}
		
		__syncthreads();


		// 2. Reduces a result of all threads using the (binary / a power of two) reduction method. 
		//    The farest point will be stored in the first thread memory.
		for (int reduction_step = 2; (thread_id % reduction_step == 0) && (thread_count >= reduction_step); reduction_step <<= 1)
		{
			// Gets a position of a new subject thread.
			int subject_thread_id = thread_id + (reduction_step >> 1);

			if (subject_thread_id >= thread_count) break; // .. there is no subject thread left

			// Checks for a subject thread to have a farther point and overrides this thread data when it does.
			if (block_data[thread_id].farest_point_sqr_distance < block_data[subject_thread_id].farest_point_sqr_distance) 
			{
				block_data[thread_id].farest_point_sqr_distance = block_data[subject_thread_id].farest_point_sqr_distance;
				block_data[thread_id].farest_point_index = block_data[subject_thread_id].farest_point_index;
			};

			__syncthreads();
		}

		// 3. Copies the result of the first thread into thread's block result.
		if (thread_id == 0)
		{
			auto* copy_to   = &result_point_index[block_id];
			auto* copy_from = &block_data[0];

			copy_to->farest_point_index        = copy_from->farest_point_index;
			copy_to->farest_point_sqr_distance = copy_from->farest_point_sqr_distance;
		}
	}




	// Implements [ grow ] function
	template<int T_Block_Count, int T_Thread_Count>
	void Algorithm_Cuda::grow
	(
		int point_a_index, 
		int point_b_index, 
		const std::vector<Vector2> & points
	)
	{
		// Captures kernel start time
		auto stopwatch_start = std::chrono::steady_clock::now();

		// 1. Runs the kernel function, which finds far points along the AB line
		find_farest_point_from_line<<<T_Block_Count, T_Thread_Count>>>
		(
			device_far_points,
			device_points_copy,
			points.size(),
			point_a_index, 
			point_b_index
		);

		// Waits the kernel function
		macro_cuda_call(cudaDeviceSynchronize());

		// Captures kernel end time
		auto stopwatch_end = std::chrono::steady_clock::now();

		// Gathers analytic data
		this->total_recursion_call_count++; // ..recurison counting
		this->kernel_total_time += std::chrono::duration<double, std::milli> // ..kernel ellapsed milliseconds
		(
			stopwatch_end - stopwatch_start
		)
		.count(); 


		// 2. Copies the result data from the device to the host
		size_t far_points_memsize = T_Block_Count * sizeof(Cuda_Thread_Data);
		stopwatch_start = std::chrono::steady_clock::now();

		macro_cuda_call(cudaMemcpy(host_far_points, device_far_points, far_points_memsize, cudaMemcpyDeviceToHost));

		stopwatch_end = std::chrono::steady_clock::now();
		this->cuda_memcpy_total_time += std::chrono::duration<double, std::milli>
		(
			stopwatch_end - stopwatch_start
		)
		.count(); 


		// 3. Reduces result of blocks into the first block memory.
		//    The farest point among others will be stored.

		stopwatch_start = std::chrono::steady_clock::now();
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

		stopwatch_end = std::chrono::steady_clock::now();
		this->reduction_total_time += std::chrono::duration<double, std::milli>
		(
			stopwatch_end - stopwatch_start
		)
		.count();

		// Gets the farest point from AB line
		int point_c_index = host_far_points[0].farest_point_index;

		// Checks it to be different from A and B point
		if (point_c_index != point_a_index && point_c_index != point_b_index)
		{
			// Tries to grow the convex hull from the AC line 
			grow<T_Block_Count, T_Thread_Count>(
				point_a_index, 
				point_c_index, 
				points
			); 

			// Adds the founded farest point to the convex hull
			convex_hull->push_back(points[point_c_index]);

			// Tries to grow the convex hull from the CB line
			grow<T_Block_Count, T_Thread_Count>(
				point_c_index, 
				point_b_index, 
				points
			); 
		}
	}

	template<int T_Block_Count, int T_Thread_Count>
	std::vector<Vector2> * Algorithm_Cuda::internal_run
	(
		const std::vector<Vector2> &points
	)
	{
		int most_left_index  = 0;
		int most_right_index = points.size() - 1;
		
		// 1. Finds the most left and right point
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
		
		size_t far_points_memsize = (T_Block_Count) * sizeof(Cuda_Thread_Data);
		size_t points_memsize = points.size() * sizeof(Vector2);

		// 2. Allocates special CUDA memory

		auto stopwatch_start = std::chrono::steady_clock::now();
		
		// ============================================================
		// !!! This CALL is 60~80 % of the program execution time !!!!!
		// !!! Makes CUDA implementation massively inefficient !!!!!!!!
		macro_cuda_call(cudaHostAlloc((void**) &host_far_points, far_points_memsize, cudaHostAllocDefault));
		macro_cuda_call(cudaMalloc((void**) &device_far_points, far_points_memsize));
		macro_cuda_call(cudaMalloc((void**) &device_points_copy, points_memsize));
		macro_cuda_call(cudaMemcpy(device_points_copy, points.data(), points_memsize, cudaMemcpyHostToDevice));
		// ============================================================

		auto stopwatch_end = std::chrono::steady_clock::now();
		this->cuda_meminit_total_time = std::chrono::duration<double, std::milli>
		(
			stopwatch_end - stopwatch_start
		)
		.count();


		// 3. Constructs a convex from right and left side of line going through the most left and right points.

		convex_hull = new std::vector<Vector2>();

		convex_hull->push_back(points[most_left_index]);
		
		// 3.1 Grows the convex hull from ML->MR line 
		grow<T_Block_Count, T_Thread_Count>(
			most_left_index, 
			most_right_index, 
			points
		);
		
		convex_hull->push_back(points[most_right_index]);
		
		// 3.2 Grows the convex hull from MR->ML line 
		grow<T_Block_Count, T_Thread_Count>(
			most_right_index, 
			most_left_index,
			points
		);

		// 4. Releases special CUDA memory
		macro_cuda_call(cudaFreeHost(host_far_points));
		macro_cuda_call(cudaFree(device_far_points));
		macro_cuda_call(cudaFree(device_points_copy));

		return convex_hull;
	}




	Algorithm_Cuda::~Algorithm_Cuda() { }

	Algorithm_Cuda::Algorithm_Cuda(int block_power) 
	{
		this->block_power = block_power;
	}

	std::vector<Vector2> * Algorithm_Cuda::run(const std::vector<Vector2> &points)
	{
		// Resets analytic data
		this->kernel_total_time = 0;
		this->total_recursion_call_count = 0;
		this->cuda_memcpy_total_time = 0;
		this->reduction_total_time = 0;

		// Depending on the power of the number of CUDA blocks,
		// matches with a call, with a correct number of threads per block
		switch(this->block_power) 
		{
			// x1
			case 0 : return internal_run<1,1024>(points);
			// x2
			case 1 : return internal_run<2,512>(points);
			// x4
			case 2 : return internal_run<4,256>(points);
			// x8
			case 3 : return internal_run<8,128>(points);
			// x16
			default: // by default, if block power has not been set then 16 blocks and 64 threads will be used
			case 4 : return internal_run<16,64>(points);
			// x32
			case 5 : return internal_run<32,32>(points); 
			// x64
			case 6 : return internal_run<64,16>(points); 
			// x128
			case 7 : return internal_run<128,8>(points);
			// x256
			case 8 : return internal_run<256,4>(points);
			// x512
			case 9 : return internal_run<512,2>(points);
			// x1024
			case 10: return internal_run<1024,1>(points);
		}
	}
}


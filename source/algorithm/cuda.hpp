#pragma once

// standard
#include <vector>

// internal
#include "core.hpp"
#include "./base.hpp"

namespace quick_hull 
{
	struct Cuda_Thread_Data
	{
		public:
			int farest_point_index;
			double farest_point_sqr_distance;
	};


	struct Algorithm_Cuda : public Algorithm
	{
		private: // ..settings
			int block_power;
		
		private: // ..buffers
			std::vector<Vector2> * convex_hull;
			Vector2              * device_points_copy;
			Cuda_Thread_Data     * device_far_points;
			Cuda_Thread_Data     * host_far_points;

		private: //.. analytic data
			int total_recursion_call_count;
			double cuda_meminit_total_time;
			double cuda_memcpy_total_time;
			double reduction_total_time;
			double kernel_total_time;
			
		public: // ..getters (analytic)
			double get_kernel_total_time() const { return kernel_total_time; }
			double get_cuda_memcpy_total_time() const { return cuda_memcpy_total_time; }
			double get_cuda_meminit_total_time() const { return cuda_meminit_total_time; }
			double get_reduction_total_time() const { return reduction_total_time; }
			int    get_total_recursion_call_count() const { return total_recursion_call_count; }
			
		public: // ..constructors & destructors
			~Algorithm_Cuda();
			Algorithm_Cuda(int block_power);
			
		public: //.. methods
			std::vector<Vector2> * run(const std::vector<Vector2> &points) override;

		private: //..methods

			template<int T_Block_Count, int T_Thread_Count>
			void grow
			(
				int point_a_index, 
				int point_b_index, 
				const std::vector<Vector2> & points
			);

			template<int T_Block_Count, int T_Thread_Count>
			std::vector<Vector2> * internal_run
			(
				const std::vector<Vector2> &points
			);
	};
}
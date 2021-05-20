#pragma once

// standard
#include <vector>

// internal
#include "core.hpp"
#include "base.hpp"

namespace quick_hull 
{
	struct Quick_Hull_Cuda : public Algorithm_Producing_Convex_Hull
	{
		public:
			int block_power;
			int thread_power;
		public:
			~Quick_Hull_Cuda();
			std::vector<Vector2> * run(const std::vector<Vector2> &points) override;

		private:
	};


	void grow
	(
		int point_a_index, 
		int point_b_index, 
		const std::vector<Vector2> & points,
		std::vector<Vector2> * convex_hull,
		Vector2* device_points_copy,
		int* host_far_points,
		int* device_far_points
	);
}
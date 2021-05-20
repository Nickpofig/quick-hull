#pragma once

// standard
#include <vector>

// internal
#include "core.hpp"
#include "./base.hpp"

namespace quick_hull 
{
	struct Algorithm_Cuda : public Algorithm
	{
		public:
			int block_power;
			int thread_power;
		public:
			~Algorithm_Cuda();
			std::vector<Vector2> * run(const std::vector<Vector2> &points) override;
	};
}
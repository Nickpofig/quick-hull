#pragma once

// standard
#include <vector>

// internal
#include "core.hpp"
#include "algorithm/base.hpp"

namespace quick_hull 
{
	struct Quick_Hull_OpenMP : public Algorithm_Producing_Convex_Hull
	{
		public: // methods
			~Quick_Hull_OpenMP();

			std::vector<Vector2> * run(const std::vector<Vector2> &points) override;
		private: // methods
			std::vector<Vector2> * grow
			(
				Vector2 a,
				Vector2 b,
				const std::vector<Vector2> &points
			);
	};
}
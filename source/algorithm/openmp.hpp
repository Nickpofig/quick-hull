#pragma once

// standard
#include <vector>

// internal
#include "core.hpp"
#include "./base.hpp"

namespace quick_hull 
{
	struct Algorithm_OpenMP : public Algorithm
	{
		public: // methods
			~Algorithm_OpenMP();

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
#pragma once

// standard
#include <functional>
#include <vector>
#include <map>

// internal
#include "core.hpp"
#include "./base.hpp"

namespace quick_hull
{
	// Sequential implementation of the quick hull algorithm
	struct Algorithm_Sequential : public Algorithm
	{
		public: // methods
			~Algorithm_Sequential();

			std::vector<Vector2> * run(const std::vector<Vector2> &points) override;

		private: // methods
			// Returns convex hull (pointer to allocated data) of the right side for the given AB line for the given points
			std::vector<Vector2> * grow
			(
				Vector2 a, 
				Vector2 b, 
				const std::vector<Vector2> &points
			);
	};

}
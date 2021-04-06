#pragma once

// standard
#include <vector>

// internal
#include "core.hpp"

namespace quick_hull 
{
	struct Algorithm_Producing_Convex_Hull 
	{
		public: // methods
			virtual ~Algorithm_Producing_Convex_Hull() = 0;
			// Returns a convex hull for the given set of points
			virtual std::vector<Vector2> * run(const std::vector<Vector2> &points) = 0;
	};
}
#pragma once

#include <vector>
#include "core.hpp"

namespace program 
{
	struct Quick_Hull
	{
		public:
			std::vector<Vector2> run(std::vector<Vector2> points);
		private:
			/// TODO: implement points flags 
			// 0        -> uncertain
			// positive -> outside   (convex hull)
			// negative -> inside    (processed point)
			std::vector<char> point_flags;
		
			std::vector<Vector2> * grow
			(
				Vector2 a, 
				Vector2 b, 
				std::vector<Vector2> &points
			);
	};
}
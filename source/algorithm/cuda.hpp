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
			~Quick_Hull_Cuda();
			
			std::vector<Vector2> * run(const std::vector<Vector2> &points) override;

		private:
	};


	void grow
	(
		Vector2 a, 
		Vector2 b, 
		const std::vector<Vector2> & points,
		std::vector<Vector2> * convex_hull
	);
}
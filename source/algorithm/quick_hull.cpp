// std
#include <iostream>

// program
#include "algorithm/quick_hull.hpp"

namespace program 
{
	std::vector<Vector2> Quick_Hull::run(std::vector<Vector2> points)
	{
		// Convex hull
		std::vector<Vector2> convex_hull;

		// Finds the most left and right point
		Vector2 left  = points.front();
		Vector2 right = points.back();
		
		for (const auto &point: points)
		{
			if (point.x > right.x) right = point;
			if (point.x < left.x)  left  = point;
		}

		convex_hull.push_back(right);
		convex_hull.push_back(left);

		grow(left, right, points, convex_hull);
		grow(right, left, points, convex_hull);

		return convex_hull;
	}

	void Quick_Hull::grow
	(
		Vector2 a, 
		Vector2 b, 
		std::vector<Vector2> &points, 
		std::vector<Vector2> &convex_hull
	)
	{
		Vector2 c = a;
		double  sqr_distance_to_c = 0;
		
		Vector2 ab = b - a;
		Vector2 ab_normal = ab.get_conter_clockwise_normal();

		// std::cout << "Trying line: A:{x:" << a.x << ", y:" << a.y 
		// 	<< "}, B:{x:" << b.x << ", y:" << b.y << "}." << std::endl;

		for (auto e : points)
		{
			auto d = project(e - a, ab) + a;
			auto de = e - d;

			double relativity = dot_product(de, ab_normal);

			if (relativity > 0)
			{
				double de_sqr_magnitude = de.get_sqr_magnitude();

				// std::cout << "D: [x:" << d.x << ", y:" << d.y << "]." << std::endl;
				// std::cout << "E: [x:"<< e.x << ", y:" << e.y << "] distance: " << de_sqr_magnitude << std::endl;

				if (de_sqr_magnitude > sqr_distance_to_c) 
				{
					c = e;
					sqr_distance_to_c = de_sqr_magnitude;
				}
			}
		}

		if (c != a && c != b)
		{
			// std::cout << "Grew to C:{x:" << c.x << ", y:" << c.y << "} !" << std::endl;

			convex_hull.push_back(c);
			grow(a, c, points, convex_hull);
			grow(c, b, points, convex_hull);
		}
	}
}
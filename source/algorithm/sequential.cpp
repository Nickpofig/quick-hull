// standard
#include <iostream>
#include <sstream>

// external
#include "omp.h"

// internal
#include "algorithm/sequential.hpp"

namespace quick_hull 
{
	Quick_Hull_Sequential::~Quick_Hull_Sequential() { }

	std::vector<Vector2> * Quick_Hull_Sequential::run(const std::vector<Vector2> &points)
	{
		// Convex hull 
		auto *convex_hull = new std::vector<Vector2>();

		Vector2 most_left  = points.front();
		Vector2 most_right = points.back();
		
		// Finds the most left and right point
		for (const auto &point: points)
		{
			if (point.x > most_right.x) most_right = point;
			if (point.x < most_left.x)  most_left  = point;
		}

		// Constructs a convex from right and left side of line going through the most left and right points
		auto *convex_hull_left  = grow(most_left, most_right, points);
		auto *convex_hull_right = grow(most_right, most_left, points);

		convex_hull->push_back(most_left);
		for (auto const point: *convex_hull_left) convex_hull->push_back(point);

		convex_hull->push_back(most_right);
		for (auto const point: *convex_hull_right) convex_hull->push_back(point);

		delete convex_hull_right;
		delete convex_hull_left;

		return convex_hull;
	}


	std::vector<Vector2> * Quick_Hull_Sequential::grow
	(
		Vector2 a, 
		Vector2 b, 
		const std::vector<Vector2> &points
	)
	{
		Vector2 ab = b - a;
		Vector2 ab_normal = ab.get_conter_clockwise_normal();

		Vector2 c = a;                 // the farest point from AB line.
		double  sqr_distance_to_c = 0; // the srotest square distance from the AB line to the C point.

		// Vector to store the resulted convex hell
		auto *convex_hull = new std::vector<Vector2>();

		// Subsets of the given points, which lays on the conter clockwise normal side of the AB line.
		auto *relative_points = new std::vector<Vector2>();

		for (auto e : points)
		{
			Vector2 d = project(e - a, ab) + a; // projection of the point E
			Vector2 de = e - d;                 // projection (as vector) from the point D to E

			double relativity = dot_product(de, ab_normal);
			
			if (relativity > 0)
			{
				double projection_sqr_magnitude = de.get_sqr_magnitude();

				if (projection_sqr_magnitude > sqr_distance_to_c) 
				{
					c = e;
					sqr_distance_to_c = projection_sqr_magnitude;
				}

				relative_points->push_back(e);
			}
		}

		if (c != a && c != b)
		{
			std::vector<Vector2> *convex_hull_left;
			std::vector<Vector2> *convex_hull_right;

			convex_hull_left  = grow(a, c, *relative_points); // a convex hull from the AC line 
			convex_hull_right = grow(c, b, *relative_points); // a convex hull from the CB line

			for (auto const point: *convex_hull_left) convex_hull->push_back(point);

			convex_hull->push_back(c);
			
			for (auto const point: *convex_hull_right) convex_hull->push_back(point);

			delete convex_hull_left;
			delete convex_hull_right;
		}

		delete relative_points;
		
		return convex_hull;
	}
}
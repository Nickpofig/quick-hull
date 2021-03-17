// standard
#include <iostream>

// external
#include "omp.h"

// internal
#include "algorithm/quick_hull.hpp"

namespace program 
{
	std::vector<Vector2> Quick_Hull::run(std::vector<Vector2> points)
	{
		// Convex hull
		std::vector<Vector2> convex_hull;

		Vector2 left  = points.front();
		Vector2 right = points.back();
		
		// Finds the most left and right point
		for (const auto &point: points)
		{
			if (point.x > right.x) right = point;
			if (point.x < left.x)  left  = point;
		}

		#pragma omp parallel
		#pragma omp single
		{
			std::vector<Vector2> *convex_hull_left; 
			std::vector<Vector2> *convex_hull_right; 

			#pragma omp task
			{
				convex_hull_left = grow(left, right, points);
			}
			
			#pragma omp task
			{
				convex_hull_right = grow(right, left, points);
			}

			#pragma omp taskwait

			convex_hull.push_back(left);
			for (auto const point: *convex_hull_left) convex_hull.push_back(point);

			convex_hull.push_back(right);
			for (auto const point: *convex_hull_right) convex_hull.push_back(point);

			delete convex_hull_left;
			delete convex_hull_right;
		}

		return convex_hull;
	}

	// Convert to return found convex points as an array
	std::vector<Vector2> * Quick_Hull::grow
	(
		Vector2 a, 
		Vector2 b, 
		std::vector<Vector2> &points
	)
	{
		Vector2 c = a;
		double  sqr_distance_to_c = 0;
		
		Vector2 ab = b - a;
		Vector2 ab_normal = ab.get_conter_clockwise_normal();

		// std::cout << "Thread: " << omp_get_thread_num() << "is working to grow the convex hull." << std::endl;
		// std::cout.flush();

		// std::cout << "Trying line: A:{x:" << a.x << ", y:" << a.y 
		// 	<< "}, B:{x:" << b.x << ", y:" << b.y << "}." << std::endl;

		// TODO: devide this loop into "dot-product" comupation and checking (vectorization)
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

		auto convex_hull = new std::vector<Vector2>();

		if (c != a && c != b)
		{
			// std::cout << "Grew to C:{x:" << c.x << ", y:" << c.y << "} !" << std::endl;

			std::vector<Vector2> *convex_hull_left;
			std::vector<Vector2> *convex_hull_right;

			#pragma omp task
			{
				convex_hull_left = grow(a, c, points);
			}
			
			#pragma omp task
			{
				convex_hull_right = grow(c, b, points);
			}

			#pragma omp taskwait

			// thread_logs << "Added to convex hull points:\n";
			for (auto const point: *convex_hull_left) convex_hull->push_back(point);
			// for (auto const point: *convex_hull_left) thread_logs << "x" << point.x << ", y:" << point.y << "\n";
			convex_hull->push_back(c);
			// thread_logs << "x" << c.x << ", y:" << c.y << "\n";
			for (auto const point: *convex_hull_right) convex_hull->push_back(point);
			// for (auto const point: *convex_hull_right) thread_logs << " p.x" << point.x << ", p.y:" << point.y << "\n";
			// thread_logs.flush();

			delete convex_hull_left;
			delete convex_hull_right;
		}

		return convex_hull;
	}
}
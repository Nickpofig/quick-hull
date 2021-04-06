// standard
#include <iostream>
#include <sstream>
#include <vector>

// external
#include "omp.h"

// internal
#include "algorithm/openmp.hpp"

namespace quick_hull 
{
	Quick_Hull_OpenMP::~Quick_Hull_OpenMP() { }

	std::vector<Vector2> * Quick_Hull_OpenMP::run(const std::vector<Vector2> &points)
	{
		// Convex hull 
		auto *convex_hull = new std::vector<Vector2>();

		Vector2 most_left  = points.front();
		Vector2 most_right = points.back();
		
		int point_count = points.size();

		// Finds the most left and right point
		/// TODO: parallelize the above for loop
		// #pragma omp parallel for shared(points)
		#pragma omp simd
		for (int index = 0; index < point_count; index++)
		{
			const auto &point = points.at(index);
			if (point.x > most_right.x) most_right = point;
			if (point.x < most_left.x)  most_left  = point;
		}

		// Constructs a convex from right and left side of line going through the most left and right points
		std::vector<Vector2> *convex_hull_left; 
		std::vector<Vector2> *convex_hull_right;
		
		#pragma omp parallel
		#pragma omp master
		{

			#pragma omp task shared(points, convex_hull_left)
			{
				convex_hull_left = grow(most_left, most_right, points);
			}

			#pragma omp task shared(points, convex_hull_right)
			{
				convex_hull_right = grow(most_right, most_left, points);
			}
		}

		convex_hull->push_back(most_left);
		for (auto const point: *convex_hull_left) convex_hull->push_back(point);

		convex_hull->push_back(most_right);
		for (auto const point: *convex_hull_right) convex_hull->push_back(point);

		delete convex_hull_right;
		delete convex_hull_left;

		return convex_hull;
	}


	std::vector<Vector2> * Quick_Hull_OpenMP::grow
	(
		Vector2 a, 
		Vector2 b, 
		const std::vector<Vector2> &points
	)
	{
		int thread_id = omp_get_thread_num();

		// {
		// 	std::stringstream log;
		// 	log << "thread: " << thread_id << std::endl;
		// 	std::cout << log.str();
		// }

		Vector2 ab = b - a;
		Vector2 ab_normal = ab.get_conter_clockwise_normal();

		Vector2 c = a;                 // the farest point from AB line.
		double  sqr_distance_to_c = 0; // the srotest square distance from the AB line to the C point.

		int point_count = points.size();

		// Vector to store the resulted convex hell
		auto *convex_hull = new std::vector<Vector2>();

		// Subsets of the given points, which lays on the conter clockwise normal side of the AB line.
		auto *relative_points = new std::vector<Vector2>();

		auto *dot_products = new std::vector<double>(); 
			// thread_datas.at(thread_id).dot_products;
		auto *projection_distances = new std::vector<double>();
			// thread_datas.at(thread_id).projection_distances;


		#pragma omp simd
		for (int index = 0; index < point_count; index++)
		{
			auto e = points.at(index);
			Vector2 d = project(e - a, ab) + a; // projection of the point E
			Vector2 de = e - d;                 // projection (as vector) from the point D to E

			double relativity = dot_product(de, ab_normal);
			double projection_sqr_magnitude = de.get_sqr_magnitude();
			
			dot_products->push_back(relativity);
			projection_distances->push_back(projection_sqr_magnitude);
		}


		#pragma omp simd
		for (int index = 0; index < point_count; index++)
		{
			double relativity = dot_products->at(index);
			double projection_sqr_magnitude = projection_distances->at(index);

			if (relativity > 0)
			{
				auto e = points.at(index);

				if (projection_sqr_magnitude > sqr_distance_to_c) 
				{
					c = e;
					sqr_distance_to_c = projection_sqr_magnitude;
				}

				relative_points->push_back(e);
			}
		}

		delete dot_products;
		delete projection_distances;

		if (c != a && c != b)
		{
			std::vector<Vector2> *convex_hull_left;
			std::vector<Vector2> *convex_hull_right;

			#pragma omp task shared(convex_hull_left, relative_points)
			{
				convex_hull_left  = grow(a, c, *relative_points); // a convex hull from the AC line 
			}

			#pragma omp task shared(convex_hull_right, relative_points)
			{
				convex_hull_right = grow(c, b, *relative_points); // a convex hull from the CB line
			}

			#pragma omp taskwait

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
#include <iostream>
#include <vector>

#include "core.hpp"
#include "algorithm/quick_hull.hpp"

using namespace program;

int main(int argument_count, char **arguments) 
{
	std::string input_filepath;
	
	// Gets program parameters (if any)
	for (int index = 1; index < argument_count; index++)
	{
		if (strcmp(arguments[index], "--help") == 0)
		{
			std::cout << "There is no help. Bye!" << std::endl;
			exit(EXIT_SUCCESS);
		}
		else input_filepath = std::string(arguments[index]);
	}

	// Asserts assignment of compulsory parameters
	if (input_filepath.empty()) 
	{
		panic_begin << "An input file is missing!" << panic_end;
	}
	
	/// TEST: prints read points
	std::cout << "Testing file read..." << std::endl;

	// Reads points
	std::vector<Vector2> points;
	read_points_from_file(input_filepath, points);

	/// TEST: prints read points
	for (auto &point : points)
	{
		std::cout << "Point { x: " << point.x << ", y: " << point.y << " }" << std::endl;
	}

	std::cout << "Begin construction of convex hull..." << std::endl;
	
	// Runs algorithm
	Quick_Hull algorithm;
	auto convex_hull = algorithm.run(points);

	std::cout << "Convex Hull:" << std::endl;
	for (auto &point: convex_hull)
	{
		std::cout << "Point { x: " << point.x << ", y: " << point.y << " }" << std::endl;
	}

	return 0;
}
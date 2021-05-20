// standard
#include <fstream>

// internal
#include "io.hpp"

namespace program 
{
	void read_points(std::string filepath, std::vector<Vector2> & points)
	{
		std::ifstream input(filepath, std::ios::in | std::ios::binary);

		int point_count;

		input >> point_count;

		for (int index = 0; index < point_count; index++)
		{
			double x, y;

			input >> x >> y;

			points.push_back(Vector2(x, y));
		}
	}

	void write_points(std::string filepath, const std::vector<Vector2> & points) 
	{
		std::ofstream output(filepath, std::ios::out | std::ios::trunc | std::ios::binary);

		// if (log_level == Program_Log_Level::Verbose) 
		// {
		// 	std::cout << "generated point count: " << generated_points->size() << std::endl;
		// }

		int point_count = points.size();

		output << point_count << std::endl;

		for (const auto &point : points)
		{
			output << point.x << " " << point.y << std::endl;
			output.flush();
		}

		output.close();

		// if (log_level == Program_Log_Level::Verbose) 
		// {
		// 	std::cout << "writted point count: " << writted_point_count << std::endl;
		// }
	}
}
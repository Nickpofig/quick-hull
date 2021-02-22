#include <iostream>
#include <fstream>
#include <cerrno>

#include "core.hpp"

namespace program 
{
	// Program's general utility code
	Panic_Message     panic_begin;
	Panic_Message_End panic_end;
	
	template <typename T> 
	Panic_Message& Panic_Message::operator<< (T argument)
	{
		std::cerr << argument;
		return *this;
	}

	Panic_Message& Panic_Message::operator<< (Panic_Message_End& end)
	{
		std::cerr << std::endl;
		exit(EXIT_FAILURE);
		return *this;
	}


	// Program's execution specific code
	Point::Point(double x, double y) : x(x), y(y) {   }


	void read_points_from_file
	(
		std::string const &filepath, 
		std::vector<Point> &points
	)
	{
		std::ifstream file_stream(filepath);
		std::string line;

		for(int line_number = 1; std::getline(file_stream, line); line_number++)
		{
			if (file_stream.bad())
			{
				panic_begin << "Failed to read file: (" << filepath << ")." << panic_end;
			}

			char *next_start;

			double x = std::strtod(line.c_str(), &next_start);
			if (errno) 
			{
				panic_begin 
					<< "Failed to read (x) position of the point at the line " 
					<< line_number 
					<< " in the file: (" << filepath << ")"
					<< panic_end;
			}
			
			double y = std::strtod(next_start, &next_start);
			if (errno) 
			{
				panic_begin 
					<< "Failed to read (y) position of the point at the line " 
					<< line_number 
					<< " in the file (" << filepath << ")"
					<< panic_end;
			}

			points.push_back(Point(x, y));
		}

		file_stream.close();
	}
}
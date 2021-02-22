#pragma once

#include <vector>
#include <string>

namespace program 
{
	// Program's general utility code
	struct Panic_Message_End {   };
 	struct Panic_Message
	{
		public:
			template <typename T> 
			Panic_Message& operator<< (T argument);
			Panic_Message& operator<< (Panic_Message_End& end);
	};

	extern Panic_Message     panic_begin;
	extern Panic_Message_End panic_end;


	// Program's execution specific code
	struct Point
	{
		public:
			double x;
			double y;

		Point(double x, double y);
	};

	extern void read_points_from_file
	(
		std::string const &filepath, 
		std::vector<Point> &points
	);
}
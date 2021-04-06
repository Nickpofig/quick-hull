// standard
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cerrno>


// external
#include "omp.h"

// internal
#include "core.hpp"
#include "algorithm/sequential.hpp"
#include "algorithm/openmp.hpp"
#include "program.hpp"

using namespace quick_hull;


int main(const int argument_count, const char **arguments) 
{
	// Gets program parameters (if any)
	Program_Configuration program_config(arguments, argument_count);

	bool log_is_verbose = program_config.get_log_level() == Program_Log_Level::Verbose;

	if (program_config.get_program_mode() == Program_Mode::Algorithm_Execution)
	{
		if (!program_config.is_using_sequential_algorithm())
		{
			if (log_is_verbose) 
			{
				program::log_begin << "Threads:\t" << omp_get_max_threads() << program::log_end;
			}
		}

		auto &algorithm = program_config.get_algorithm();
		auto &points = program_config.get_points();

		// Starts program runtime counting
		auto stopwatch_start = std::chrono::steady_clock::now();

		// Runs algorithm
		auto *convex_hull = algorithm.run(points);

		// Ends program runtime counting
		auto stopwatch_end = std::chrono::steady_clock::now();

		// Prints result
		if(log_is_verbose)
		{
			program::log_begin << "Convex Hull:" << program::log_end;
		}
		
		for (int index = 0, ch_size = convex_hull->size(); index < ch_size; index++)
		{
			auto &point = convex_hull->at(index);
			
			if (log_is_verbose)
			{
				program::log_begin << "\t{ x: " << point.x << ", y: " << point.y << " }" << program::log_end;
			}
			else // quit
			{
				program::log_begin << point.x << " " << point.y << program::log_end;
			}
		}

		// Prints runtime
		if (log_is_verbose) 
		{
			program::log_begin << "Runtime: " << std::chrono::duration<double, std::milli>(stopwatch_end - stopwatch_start).count() << " ms." << program::log_end;
		}
	}
	else 
	{
		auto *generated_points = program_config.get_points_generation_method().execute();

		for (const auto &point : *generated_points)
		{
			if (log_is_verbose)
			{
				program::log_begin << point.x << " " << point.y << program::log_end;
			}
		}

		delete generated_points;
	}

	return 0;
}
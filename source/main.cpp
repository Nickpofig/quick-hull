// standard
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cerrno>


// external
#if _OPENMP
#include "omp.h"
#endif

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

	bool log_is_quiet   = program_config.get_log_level() == Program_Log_Level::Quiet;
	bool log_is_silent  = program_config.get_log_level() == Program_Log_Level::Silent;
	bool log_is_verbose = program_config.get_log_level() == Program_Log_Level::Verbose;
	bool must_execute_algorithm = program_config.get_program_mode() == Program_Mode::Algorithm_Execution;

	if (must_execute_algorithm)
	{
		switch (program_config.get_algorithm_implementation_enum())
		{
			case Program_Algorithm_Implementation::Sequential: break;
			case Program_Algorithm_Implementation::Cuda: break;
			case Program_Algorithm_Implementation::OpenMP:
			{
#if _OPENMP
				if (log_is_verbose || log_is_quiet) 
				{
					program::log_begin << "Threads:\t" << omp_get_max_threads() << program::log_end;
				}
#endif
				break;
			}
		}

		auto &algorithm = program_config.get_algorithm();
		auto &points = program_config.get_points();

		if (log_is_quiet || log_is_verbose) 
		{
			program::log_begin << "Points:\t" << points.size() << program::log_end;
		}

		// Starts program runtime counting
		auto stopwatch_start = 
			// omp_get_wtime();
			std::chrono::steady_clock::now();

		// Runs algorithm
		auto *convex_hull = algorithm.run(points);

		// Ends program runtime counting
		auto stopwatch_end =
			//  omp_get_wtime();
			std::chrono::steady_clock::now();

		// Prints result
		if(log_is_verbose)
		{
			program::log_begin << "Convex Hull:" << program::log_end;
		}
		
		size_t convex_hull_hash;

		if (log_is_silent) // prints number of points
		{
			program::log_begin << convex_hull->size() << program::log_end;
		}

		for (int index = 0, ch_size = convex_hull->size(); index < ch_size; index++)
		{
			auto &point = convex_hull->at(index);
			
			if (log_is_verbose)
			{
				program::log_begin << "\t{ x: " << point.x << ", y: " << point.y << " }" << program::log_end;
			}
			else if (log_is_silent) // prints point coordinates
			{
				program::log_begin << point.x << " " << point.y << program::log_end;
			}
			else if(log_is_quiet) 
			{
				convex_hull_hash ^= point.get_hash() << 1;
			}
		}

		if (log_is_quiet)
		{
			program::log_begin << "Result hash: " << convex_hull_hash << "." << program::log_end;
		}

		// Prints runtime
		if (log_is_verbose || log_is_quiet) 
		{
			program::log_begin << "Convex hull: " << convex_hull->size() << "." << program::log_end;
			program::log_begin << "Runtime: " 
				<< std::chrono::duration<double, std::milli>(stopwatch_end - stopwatch_start).count() 
				// << stopwatch_end - stopwatch_start
			<< " ms." << program::log_end;
		}
	}
	else 
	{
		if (log_is_verbose || log_is_quiet) 
		{
			program_config.get_points_generation_method().log();
		}

		program_config.generate_and_write_points();
	}

	return 0;
}
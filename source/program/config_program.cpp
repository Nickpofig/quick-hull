// standard
#include <chrono>

// internal
#include "io.hpp"
#include "./config_program.hpp"
#include "./config_computing.hpp"
#include "./config_generation.hpp"
#include "./algorithm/config_base.hpp"
#include "./algorithm/config_cuda.hpp"
#include "./algorithm/config_openmp.hpp"
#include "./algorithm/config_sequential.hpp"
#include "./constants.hpp"

namespace program 
{
	bool Configuration_Program::try_initialize
	(
		Input_Configuration input, 
		bool allow_panic
	) 
	{
		for (auto iterator = Input_Configuration_Iterator(input); iterator.move_next();)
		{
			if (iterator.is_argument(program_arguments_tag::log_quiet)) 
			{
				this->log_mode = Log_Mode::Quiet;
			}
			else if (iterator.is_argument(program_arguments_tag::log_verbose)) 
			{
				this->log_mode = Log_Mode::Verbose;
			}
			if (iterator.is_argument(program_arguments_tag::help)) 
			{
				program::print_help();
				exit(EXIT_SUCCESS);
			}
		}


		auto application_computing = new Application_Configuration_Computing();
		auto application_generation = new Application_Configuration_Points_Generation();
		
		// * Defines application
		if (application_generation->try_initialize(input, allow_panic))
		{
			this->application = application_generation;
		}
		else if (application_computing->try_initialize(input, allow_panic)) 
		{
			this->application = application_computing;
		}
		else if (allow_panic) 
		{
			program::panic_begin << "Panic: insufficient program configuration!" << program::panic_end;
		}
		else return false;
		
		if (this->application != application_computing) delete application_computing;
		if (this->application != application_generation) delete application_generation;


		return true;
	}


	void Configuration_Program::execute_application() const
	{
		bool log_is_quiet = this->log_mode == Log_Mode::Quiet;
		bool log_is_verbose = this->log_mode == Log_Mode::Verbose;


		// Captures program start time
		auto stopwatch_start = std::chrono::steady_clock::now();

		if (auto application_points_generation = dynamic_cast<Application_Configuration_Points_Generation*>(application)) 
		{
			if (log_is_verbose || log_is_quiet) 
			{
				program::log_begin
					<< "Output: " << application_points_generation->get_output_filepath() << "." 
					<< "\nPoints: " << application_points_generation->get_points_count() << "."
					<< program::log_end;
			}

			std::vector<Vector2> points;
			application_points_generation->execute(points);

			if (log_is_verbose || log_is_quiet)
			{
				for (int index = 0; index < points.size(); index++) 
				{
					auto &point = points.at(index);
					
					program::log_begin 
						<< "\t{ x: " << point.x << ", y: " << point.y << " }" 
						<< program::log_end;
				}
			}

			program::write_points(application_points_generation->get_output_filepath(), points);
		}
		else
		if (auto application_computing = dynamic_cast<Application_Configuration_Computing*>(application)) 
		{
			std::vector<Vector2> convex_hull;
			double ellapsed_milliseconds;
			std::ostringstream detail_info;

			// Prints algorithm parallelization method
			if(log_is_verbose || log_is_quiet) 
			{
				program::log_begin
					<< "Method: " << application_computing->get_algorithm_config()->get_info_text() << "."
					<< "\nPoints: " << application_computing->get_point_count() << "."
					<< program::log_end;
			}

			application_computing->compute_convex_hull(convex_hull, ellapsed_milliseconds, detail_info);

			// Prints number of points
			if(log_is_verbose || log_is_quiet)
			{
				program::log_begin
					<< "Convex Hull: " << convex_hull.size() << "."
					<< program::log_end;
			}
			else // prints raw number of convex hull points
			{
				program::log_begin << convex_hull.size() << program::log_end;
			}


			size_t convex_hull_hash;

			for (int index = 0; index < convex_hull.size(); index++)
			{
				auto &point = convex_hull.at(index);
				
				if (log_is_verbose)
				{
					program::log_begin 
						<< "\t{ x: " << point.x << ", y: " << point.y << " }" 
						<< program::log_end;
				}
				else if(log_is_quiet) 
				{
					convex_hull_hash ^= point.get_hash() << 1;
				}
				else // prints a raw point coordinates
				{
					program::log_begin 
						<< point.x << " " << point.y 
						<< program::log_end;
				}
			}

			// Prints runtime
			if (log_is_verbose || log_is_quiet) 
			{
				auto details = detail_info.str();

				if (details.empty()) 
				{
					details = "{ }";
				}

				program::log_begin
					<< "Result hash: " << convex_hull_hash << "." 
					<< "\nAlgorithm time: " << ellapsed_milliseconds << " milliseconds."
					<< "\nDetails: " << details << "."
					<< program::log_end;
			}
		}


		// Captures program end time
		if (log_is_verbose || log_is_quiet) 
		{
			auto stopwatch_end = std::chrono::steady_clock::now();
			double program_ellapsed_milliseconds = std::chrono::duration<double, std::milli>
			(
				stopwatch_end - stopwatch_start
			)
			.count();

			program::log_begin
				<< "Total time: " << program_ellapsed_milliseconds << " milliseconds."
				<< program::log_end;
		}
	}
}
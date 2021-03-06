// standard
#include <chrono>

// internal
#include "io.hpp"
#include "input_configuration.hpp"
#include "./constants.hpp"
#include "./config_computing.hpp"
#include "./algorithm/config_base.hpp"
#include "./algorithm/config_sequential.hpp"

#if _OPENMP
	#include "./algorithm/config_openmp.hpp"
#elif __NVCC__
	#include "./algorithm/config_cuda.hpp"
#endif


namespace program 
{
	bool Application_Configuration_Computing::try_initialize
	(
		Input_Configuration input, 
		bool allow_panic
	)
	{
		// 1. Looks at input

		std::string points_filepath;

		for(auto iterator = Input_Configuration_Iterator(input); iterator.move_next();) 
		{
			if (iterator.is_argument(program_arguments_tag::points_filepath)) 
			{
				if (iterator.move_next())
				{
					points_filepath = iterator.get_argument_as_string();
				}
				else if (allow_panic)
				{
					program::panic_begin << "Panic: points filepath is undefined!" << program::panic_end;
				}
			}
		};


		// 2. Reads points

		if (points_filepath.empty()) 
		{
			return false;
		}

		read_points(points_filepath, this->points);


		// 3.1 Selects algorithm

#if _OPENMP
		auto algorithm_config_openmp = new Algorithm_Configuration_OpenMP();
		if (algorithm_config_openmp->try_initialize(input, false)) 
		{
			this->algorithm_config = algorithm_config_openmp;
		}
#elif __NVCC__
		auto algorithm_config_cuda = new Algorithm_Configuration_Cuda();
		if(algorithm_config_cuda->try_initialize(input, false))
		{
			this->algorithm_config = algorithm_config_cuda;
		}
#else
		if (false) { }
#endif
		else // ..by default the sequential implementtion is used
		{
			this->algorithm_config = new Algorithm_Configuration_Sequential();
			this->algorithm_config->try_initialize(input, true); // ..if even sequential algoirhtm fails to initialize the program should not conntinue its execution
		}


		// 3.2 Deletes unused algorithm configurations (if any)
#if _OPENMP
		if (this->algorithm_config != algorithm_config_openmp) delete algorithm_config_openmp;
#elif __NVCC__
		if (this->algorithm_config != algorithm_config_cuda) delete algorithm_config_cuda;
#endif

		return true;
	}

	void Application_Configuration_Computing::compute_convex_hull
	(
		std::vector<Vector2> & result_convex_hull,
		double               & result_ellapsed_milliseconds,
		std::ostringstream   & result_runtime_info_buffer
	)
	const
	{
		auto & executor = this->algorithm_config->get_executor_instance();

		// Captures computing start time
		auto stopwatch_start = std::chrono::steady_clock::now();

		auto * convex_hull = executor.run(points);

		// Captures computing end time
		auto stopwatch_end = std::chrono::steady_clock::now();

		// Computes computing ellapsed milliseconds
		result_ellapsed_milliseconds = std::chrono::duration<double, std::milli>
		(
			stopwatch_end - stopwatch_start
		)
		.count(); 

		// Copies computing result into the output result
		for (int index = 0; index < convex_hull->size(); index++) 
		{
			Vector2 point = convex_hull->at(index);
			result_convex_hull.push_back(point);
		}

		result_runtime_info_buffer << this->algorithm_config->get_runtime_info_text();

		delete convex_hull;
	}
}
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
		// * Looks at input

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


		// * Reads points

		if (points_filepath.empty()) 
		{
			return false;
		}

		read_points(points_filepath, this->points);


		// * Selects algorithm

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
		else
		{
			this->algorithm_config = new Algorithm_Configuration_Sequential();
		}


		// * Deletes unused algorithm configurations (if any)
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
		double               & result_ellapsed_milliseconds
	) 
	const
	{
		auto executor = this->algorithm_config->create_executor_instance();


		// Starts program runtime counting
		auto stopwatch_start = std::chrono::steady_clock::now();

		// program::log_begin << "cch 1." << program::log_end;

		auto * convex_hull = executor->run(points);

		// program::log_begin << "cch 2." << program::log_end;

		// Ends program runtime counting
		auto stopwatch_end = std::chrono::steady_clock::now();

		result_ellapsed_milliseconds = std::chrono::duration<double, std::milli>(stopwatch_end - stopwatch_start).count(); 

		for (int index = 0; index < convex_hull->size(); index++) 
		{
			Vector2 point = convex_hull->at(index);
			result_convex_hull.push_back(point);
		}

		delete convex_hull;
		delete executor;
	}
}
#pragma once

// internal
#include <string>

namespace program_arguments_tag 
{
	static const std::string help              = "--help";

	// application & points generation
	static const std::string points_filepath   = "--file";

	// logging
	static const std::string log_verbose       = "--log-verbose";
	static const std::string log_quiet         = "--log-quiet";

	// open mp
	static const std::string openmp            = "--openmp";
	static const std::string openmp_threads    = "--openmp-threads";

	// points generation
	static const std::string generate_circle   = "--generate-circle";
	static const std::string generate_size     = "--generate-count";

	// cuda
	static const std::string cuda              = "--cuda";
	static const std::string cuda_block_power  = "--cuda-block-power"; 
	static const std::string cuda_thread_power = "--cuda-thread-power";
};

namespace program_arguments_definition 
{
	static const std::string help              = "Prints this information. [] - compulsory, () - optional parameter value.";
	static const std::string points_filepath   = "Sets points filepath.";

	static const std::string log_verbose       = "Sets log level: all.";
	static const std::string log_quiet         = "Sets log level: minimum";

	static const std::string openmp            = "Marks that program will use Open MP version of the Quickhull algorithm.";
	static const std::string opemp_threads     = "Sets the maximum number of available threads. Should be equal to the amount of physical cores.";
	
	static const std::string generation_size   = "Sets amount of points to be generated.";
	static const std::string generate_circle   = "Sets the points generation method: Circle, with outer and inner radius specified.";

	static const std::string cuda              = "Marks that program will use CUDA version of the Quickhull algorithm.";
	static const std::string cuda_block_power  = "Sets order of power of block number.";
	static const std::string cuda_thread_power = "Sets order of power of thread number.";
}

namespace program 
{
	void print_help();
}
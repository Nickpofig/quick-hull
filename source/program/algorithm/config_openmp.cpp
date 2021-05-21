// external
#include "omp.h"

// internal
#include "console.hpp"
#include "input_configuration.hpp"
#include "algorithm/openmp.hpp"
#include "program/constants.hpp"
#include "./config_openmp.hpp"

namespace program 
{
	bool Algorithm_Configuration_OpenMP::try_initialize
	(
		Input_Configuration input, 
		bool allow_panic
	)
	{
		bool is_there_openmp_flag = false;
		this->thread_count = omp_get_max_threads();

		for (auto iterator = Input_Configuration_Iterator(input); iterator.move_next();) 
		{
			if (iterator.is_argument(program_arguments_tag::openmp)) 
			{
				is_there_openmp_flag = true;
			}
			else
			if (iterator.is_argument(program_arguments_tag::openmp_threads)) 
			{
				if (iterator.move_next()) 
				{
					this->thread_count = iterator.get_argument_as_int();
				}
				else if (allow_panic) program::panic_begin << "Panic: open mp thread count value is missing!";
			}
		}

		this->algorithm = new quick_hull::Algorithm_OpenMP();
		omp_set_num_threads(this->thread_count);

		return is_there_openmp_flag;
	}


	std::string Algorithm_Configuration_OpenMP::get_info_text() const 
	{
		std::ostringstream builder;

		builder 
			<< "Open MP { threads: " 
			<< omp_get_max_threads() 
			<< "}";

		return builder.str();
	}
}
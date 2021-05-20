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

		for (auto iterator = Input_Configuration_Iterator(input); iterator.move_next();) 
		{
			if (iterator.is_argument(program_arguments_tag::openmp)) 
			{
				is_there_openmp_flag = true;

				if (iterator.move_next()) 
				{
					this->thread_count = iterator.get_argument_as_double();
				}
				else if (allow_panic) program::panic_begin << "Panic: open mp thread count value is missing!";
			}
		}

		return is_there_openmp_flag;
	}

	quick_hull::Algorithm * Algorithm_Configuration_OpenMP::create_executor_instance() const 
	{
		return new quick_hull::Algorithm_OpenMP();
	}

	std::string Algorithm_Configuration_OpenMP::get_info_text() const 
	{
		return "Open MP";
	}
}
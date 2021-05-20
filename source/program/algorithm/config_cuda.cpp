// standard
#include <cmath>

// internal
#include "console.hpp"
#include "input_configuration.hpp"
#include "algorithm/cuda.hpp"
#include "program/constants.hpp"
#include "./config_cuda.hpp"

namespace program 
{
	bool Algorithm_Configuration_Cuda::try_initialize
	(
		Input_Configuration input, 
		bool allow_panic
	)
	{
		bool is_there_cuda_flag = false;

		this->block_power = 4; // x16

		for (auto iterator = Input_Configuration_Iterator(input); iterator.move_next();) 
		{
			if (iterator.is_argument(program_arguments_tag::cuda)) 
			{
				is_there_cuda_flag = true;
			}
			else
			if (iterator.is_argument(program_arguments_tag::cuda_block_power)) 
			{
				if (iterator.move_next()) 
				{
					this->block_power = iterator.get_argument_as_double();
				}
				else if (allow_panic) program::panic_begin << "Panic: cuda block power value is missing!";
			}
			else 
			if (iterator.is_argument(program_arguments_tag::cuda_thread_power)) 
			{
				if (iterator.move_next()) 
				{
					this->thread_power = iterator.get_argument_as_double();
				}
				else if (allow_panic) program::panic_begin << "Panic: cuda thread power value is missing!";
			}
		}

		this->block_power = std::min(10, std::max(0, this->block_power));

		return is_there_cuda_flag;
	}

	quick_hull::Algorithm * Algorithm_Configuration_Cuda::create_executor_instance() const 
	{
		auto result = new quick_hull::Algorithm_Cuda();
		
		result->block_power = this->block_power;
		result->thread_power = this->thread_power;

		return result;
	}

	std::string Algorithm_Configuration_Cuda::get_info_text() const 
	{
		std::ostringstream builder;

		builder 
			<< "CUDA { blocks: " 
			<< (1 << this->block_power) 
			<< ", threads: " 
			<< (1 << std::max(0, 10 - this->block_power)) 
			<< "}";

		return builder.str();
	}
}
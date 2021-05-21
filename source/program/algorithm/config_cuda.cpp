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

		if (this->block_power != -1) 
		{
			this->block_power = std::min(10, std::max(0, this->block_power));
		}

		this->algorithm = new quick_hull::Algorithm_Cuda(this->block_power);

		return is_there_cuda_flag;
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


	std::string Algorithm_Configuration_Cuda::get_runtime_info_text() const 
	{
		std::ostringstream builder;

		if (auto * algorithm_cuda = dynamic_cast<quick_hull::Algorithm_Cuda * >(this->algorithm)) 
		{
			builder 
				<< "{ " 
				<< "\n\t kernel: " << algorithm_cuda->get_kernel_total_time() << " ms"
				<< "\n\t cuda memcpy: " << algorithm_cuda->get_cuda_memcpy_total_time() << " ms"
				<< "\n\t reduction: " << algorithm_cuda->get_reduction_total_time() << " ms"
				<< "\n\t cuda meminit: " << algorithm_cuda->get_cuda_meminit_total_time() << " ms"
				<< "\n\t recursion calls: " << algorithm_cuda->get_total_recursion_call_count()
				<< "\n }";
		}

		return builder.str();
	}
}
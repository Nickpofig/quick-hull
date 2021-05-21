// standard
#include <iostream>

// internal
#include "./constants.hpp"

namespace program 
{
	void print_help()
	{
		std::cout
		<< std::endl
		
		<< '\t' << program_arguments_tag::points_filepath 
			<< " [path] \t -> " << program_arguments_definition::points_filepath 
			<< std::endl
			
		<< '\t' << program_arguments_tag::log_verbose 
			<< "\t -> " << program_arguments_definition::log_verbose 
			<< std::endl
		<< '\t' << program_arguments_tag::log_quiet 
			<< "\t -> " << program_arguments_definition::log_quiet 
			<< std::endl
			
		<< '\t' << program_arguments_tag::openmp 
			<< "\t -> " << program_arguments_definition::openmp 
			<< std::endl
		<< '\t' << program_arguments_tag::openmp_threads 
			<< " [positive interger] \t -> " << program_arguments_definition::opemp_threads 
			<< std::endl
			
		<< '\t' << program_arguments_tag::cuda 
			<< "\t -> " << program_arguments_definition::cuda 
			<< std::endl
		<< '\t' << program_arguments_tag::cuda_block_power 
			<< "\t -> " << program_arguments_definition::cuda_block_power 
			<< std::endl
		// << '\t' << program_arguments_tag::cuda_thread_power 
		// 	<< "\t -> " << program_arguments_definition::cuda_thread_power 
		// 	<< std::endl
			
		<< '\t' << program_arguments_tag::generate_circle 
			<< " [outer-radius] [inner-radius] \t -> " << program_arguments_definition::generate_circle 
			<< std::endl
		<< '\t' << program_arguments_tag::generate_size 
			<< " [positive integer] \t -> " << program_arguments_definition::generation_size 
			<< std::endl

		<< '\t' << program_arguments_tag::help 
			<< "\t -> " << program_arguments_definition::help 
			<< std::endl

		<< "\n\tExamples:"
		   "\n\t(Seq)    : ./quickhull --file [path] --log-quiet"
		   "\n\t(Open MP): ./quickhull --file [path] --log-quiet --openmp --openmp-threads 4"
		   "\n\t(CUDA)   : ./quickhull_cuda --file [path] --log-quiet --cuda --cuda-block-power 3"
			<< std::endl

		<< std::endl;
	}
}
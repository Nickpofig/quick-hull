// standard
#include <sstream>
#include <string>
#include <cstring>
#include <stdlib.h>

// internal
#include "program.hpp"
#include "algorithm/sequential.hpp"
#include "algorithm/openmp.hpp"

#if __NVCC__
	#include "algorithm/cuda.hpp"
#endif

// Points_Generation_Method

Points_Generation_Method::Points_Generation_Method
(
	Program_Input input,
	const int start_index
) 
: input{input}, index{start_index}
{
	for (int _index = 0; _index < input.size; _index++) 
	{
		if (strcmp(input.array[_index], program_arguments_tag::points_count.c_str()) == 0) 
		{
			if (++_index >= input.size) 
			{
				program::panic_begin << "Error: value for <" << program_arguments_tag::points_count << "> parameter is missing!" << program::panic_end;
			}

			try 
			{
				count = std::stoi(input.array[_index]);
			}
			catch(std::exception exception)
			{
				program::panic_begin << "Error: (" << exception.what() << ") when tried to parse value of <" << program_arguments_tag::points_count << "> parameter!" << program::panic_end;
			}
		}
	}
}

// Circle_Points_Generation_Method 

Circle_Points_Generation_Method::Circle_Points_Generation_Method
(
	Program_Input input,
	const int start_index
) 
: Points_Generation_Method { input, start_index }
{
	is_found = false;

	for (;index < input.size; index++) 
	{
		if (strcmp(input.array[index], program_arguments_tag::circle.c_str()) == 0) 
		{
			is_found = true;
			break;
		}
	}
}

void Circle_Points_Generation_Method::log()
{
	program::log_begin
		<< "Generation Method: Circle." 
		<< "\nCount: " << this->count << "."
		<< "\nCenter: (x:" << this->center_point.x << ", y:" << this->center_point.y << ")."
		<< "\nInner Radius: " << this->inner_radius << "."
		<< "\nOuter Radius: " << this->outer_radius << "."
		<< program::log_end;
}

void Circle_Points_Generation_Method::read_double(double *store)
{
	try 
	{
		*store = std::stod(input.array[index]);
	}
	catch(std::exception exception)
	{
		program::panic_begin << "Error: " << exception.what() << "!" << program::panic_end;
	}
}

bool Circle_Points_Generation_Method::move_next()
{
	index++;
	return index <= input.size;
}

bool Circle_Points_Generation_Method::find()
{
	if (!is_found) 
	{
		return false;
	}

	// compolsury
	if (!move_next()) 
	{
		program::panic_begin << "Error: <outer-radius> value of <" << program_arguments_tag::circle << "> paramter is missing!" << program::panic_end;
	}
	read_double(&outer_radius);

	if (!move_next()) 
	{
		program::panic_begin << "Error: <inner-radius> value of <" << program_arguments_tag::circle << "> parameter is missing!" << program::panic_end;
	}
	read_double(&inner_radius);

	// optional
	for (;index < input.size; index++) 
	{
		if (strcmp(input.array[index], program_arguments_tag::circle_center.c_str()) == 0) 
		{
			read_double(&center_point.x);
			if (!move_next()) program::panic_begin << "Error: <y> value of " << program_arguments_tag::circle_center << " parameter is missing!" << program::panic_end;
			read_double(&center_point.y);
			break;
		}
	}

	return true;
}

std::vector<Vector2> * Circle_Points_Generation_Method::execute() const
{
	return generate_points_in_circle(count, center_point, outer_radius, inner_radius);
}


// Program_Configuration 
void Program_Configuration::read_points_filepath()
{
	std::string filepath; 

	for (int index = 0; index < input.size; index++)
	{
		if (strcmp(input.array[index], program_arguments_tag::points_filepath.c_str()) == 0)
		{
			if (++index >= input.size)
			{
				program::panic_begin << "Error: <" << program_arguments_tag::points_filepath << "> paramter value is missing!" << program::panic_end;
			}

			filepath = std::string(input.array[index]);
			break;
		}
	}

	if (filepath.empty()) 
	{
		program::panic_begin << "Error: <" << program_arguments_tag::points_filepath << "> parameter is missing!" << program::panic_end;
	}

	points_filepath = filepath;
}

void Program_Configuration::generate_and_write_points()
{
	auto *generated_points = points_generation_method->execute();
	std::ofstream output(points_filepath, std::ios::out | std::ios::trunc | std::ios::binary);

	int writted_point_count = 0;

	if (log_level == Program_Log_Level::Verbose) 
	{
		std::cout << "generated point count: " << generated_points->size() << std::endl;
	}

	int generated_point_count = generated_points->size();
	output << generated_point_count << std::endl;

	for (const auto &point : *generated_points)
	{
		output << point.x << " " << point.y << std::endl;
		output.flush();
		writted_point_count++;
	}

	output.close();

	if (log_level == Program_Log_Level::Verbose) 
	{
		std::cout << "writted point count: " << writted_point_count << std::endl;
	}

	delete generated_points;
}

void Program_Configuration::read_points()
{
	// std::string content = encoding::heuristically_read_file(filepath); 
	// std::stringstream content_stream(content);

	std::ifstream input(points_filepath, std::ios::in | std::ios::binary);

	int point_count;

	input >> point_count;

	if (log_level == Program_Log_Level::Verbose) 
	{
		std::cout << "Point count to read: " << readed_points.size() << std::endl;
	}

	// while(!ifstream.eof()) 
	for (int index = 0; index < point_count; index++)
	{
		double x, y;

		// content_stream >> std::skipws >> x >> std::skipws >> y;

		input >> x >> y;

		readed_points.push_back(Vector2(x, y));
	}

	if (log_level == Program_Log_Level::Verbose) 
	{
		std::cout << "Readed point count: " << readed_points.size() << std::endl;
	}
}



void Program_Configuration::read_points_generation_method(int start_index)
{
	mode = Program_Mode::Points_Generation;

	auto *circle_method = new Circle_Points_Generation_Method (input, start_index);

	if (!circle_method->find()) 
	{
		program::panic_begin << "Error: the <" << program_arguments_tag::circle << "> parameter is missing or not properly declared!" << program::panic_end;
	}

	points_generation_method = circle_method;
}

void Program_Configuration::read()
{
	// Sets default parameters.
	mode = Program_Mode::Algorithm_Execution;
	log_level = Program_Log_Level::Silent;


	// Check for help asking
	for (int index = 0; index < input.size; index++)
	{
		auto *argument = input.array[index];

		if (strcmp(argument, program_arguments_tag::help.c_str()) == 0) 
		{
			program::print_help();
			exit(EXIT_SUCCESS);
		}
	}

	// Compulsory action
	read_points_filepath();
	
	// Decisive
	for (int index = 0; index < input.size; index++)
	{
		auto *argument = input.array[index];

		if (strcmp(argument, program_arguments_tag::sequential_mode.c_str()) == 0)
		{
			algorithm = new Quick_Hull_Sequential();
			mode = Program_Mode::Algorithm_Execution;
			read_points();
			break;
		}
		else if (strcmp(argument, program_arguments_tag::points_generation.c_str()) == 0) 
		{
			read_points_generation_method(index + 1);
			break;
		}
	}

	// Defines other parameters...
	for (int index = 0; index < input.size; index++) 
	{
		auto *argument = input.array[index];

		if (strcmp(argument, program_arguments_tag::thread_count.c_str()) == 0) 
		{
			if (++index >= input.size) 
			{
				program::panic_begin << "Error: value of <" << program_arguments_tag::thread_count << "> parameter is missing!" << program::panic_end;
			}

			try 
			{
				int number_of_threads = std::stoi(input.array[index]);
#if _OPENMP
				omp_set_num_threads(number_of_threads);
#else
				program::panic_begin << "Error: could not set number of threads. Reason: application has been compiled without Open MP" << program::panic_end; 
#endif
			}
			catch(std::exception exception)
			{
				program::panic_begin << "Error: (" << exception.what() << ") when tried to parse value of <" 
							<< program_arguments_tag::thread_count << "> parameter!" << program::panic_end;
			}
		}
		else if (strcmp(argument, program_arguments_tag::log_verbose.c_str()) == 0) 
		{
			log_level = Program_Log_Level::Verbose;
		}
		else if (strcmp(argument, program_arguments_tag::log_quiet.c_str()) == 0) 
		{
			log_level = Program_Log_Level::Quiet;
		}
	}

	// by default uses parallel implementation of the algorithm
	if (!algorithm && mode == Program_Mode::Algorithm_Execution)
	{
#if _OPENMP
		algorithm = new Quick_Hull_OpenMP();
#elif __NVCC__
		algorithm = new Quick_Hull_Cuda();
		auto cuda_algorithm = (Quick_Hull_Cuda & )(&algorithm);
		
		for (int index = 0; index < input.size; index++)
		{
			auto *argument = input.array[index];

			if (strcmp(argument, program_arguments_tag::cuda_block_power) == 0)
			{
				cuda_algorithm.block_power = program::read_double();
				break;
			}
			else if (strcmp(argument, program_arguments_tag::points_generation.c_str()) == 0) 
			{
				read_points_generation_method(index + 1);
				break;
			}
		}
#else
#warning no parallel implementation has been compiled 
		algorithm = new Quick_Hull_Sequential();
#endif
		read_points();
	}
}


Program_Configuration::Program_Configuration(const char** argv, const int argc) : input{argv, argc}
{
	algorithm = nullptr;
	points_generation_method = nullptr;

	read();
}

Program_Configuration::~Program_Configuration() 
{
	if (algorithm) delete algorithm;
	if (points_generation_method) delete points_generation_method;
}

const Program_Mode & Program_Configuration::get_program_mode() const
{
	return mode;
}

const Program_Log_Level & Program_Configuration::get_log_level() const
{
	return log_level;
}

const std::vector<Vector2> & Program_Configuration::get_points() const
{
	return readed_points;
}

const std::string & Program_Configuration::get_points_filepath() const 
{
	return points_filepath;
}


Algorithm_Producing_Convex_Hull & Program_Configuration::get_algorithm() const
{
	return *algorithm;
}

Points_Generation_Method & Program_Configuration::get_points_generation_method() const
{
	return *points_generation_method;
}

bool Program_Configuration::is_using_sequential_algorithm() const 
{
	return dynamic_cast<Quick_Hull_Sequential *>(algorithm) != nullptr;
}

Program_Algorithm_Implementation Program_Configuration::get_algorithm_implementation_enum() const
{
#if _OPENMP
	if (dynamic_cast<Quick_Hull_OpenMP *>(algorithm) != nullptr) return Program_Algorithm_Implementation::OpenMP;
#elif __NVCC__
	if (dynamic_cast<Quick_Hull_Cuda *>(algorithm) != nullptr) return Program_Algorithm_Implementation::Cuda;
#endif
	return Program_Algorithm_Implementation::Sequential;
}


// Program's general utility code
namespace program 
{
	Panic_Message     panic_begin;
	Panic_Message_End panic_end;

	Panic_Message& Panic_Message::operator<<(Panic_Message_End& end)
	{
		std::cerr << std::endl;
		exit(EXIT_FAILURE);
		return *this;
	}

	Log_Stream     log_begin;
	Log_Stream_End log_end;

	Log_Stream& Log_Stream::operator<<(Log_Stream_End& end)
	{
		std::cout << stream.str() << std::endl;
		std::cout.flush();
		stream.str(""); // I hate c++ for this
		return *this;
	}

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
		<< '\t' << program_arguments_tag::sequential_mode 
			<< "\t -> " << program_arguments_definition::sequential_mode 
			<< std::endl
		<< '\t' << program_arguments_tag::thread_count 
			<< " [positive interger] \t -> " << program_arguments_definition::thread_count 
			<< std::endl
		<< '\t' << program_arguments_tag::points_generation 
			<< " [method] [count]\t -> " << program_arguments_definition::points_generation 
			<< std::endl
		<< '\t' << program_arguments_tag::circle 
			<< " [outer-radius] [inner-radius] (center-x-offset) (center-y-offset) \t -> " << program_arguments_definition::circle 
			<< std::endl
		<< '\t' << program_arguments_tag::points_count 
			<< " [positive integer] \t -> " << program_arguments_definition::points_count 
			<< std::endl
		<< '\t' << program_arguments_tag::help 
			<< "\t -> " << program_arguments_definition::help 
			<< std::endl
		<< std::endl
		;
	}
}
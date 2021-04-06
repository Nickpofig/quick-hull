// standard
#include <sstream>
#include <string>
#include <stdlib.h>

// internal
#include "program.hpp"
#include "utils/unicode.hpp"

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
		program::panic_begin << "Error: <outer radius> value of <" << program_arguments_tag::circle << "> paramter is missing!" << program::panic_end;
	}
	read_double(&outer_radius);

	if (!move_next()) 
	{
		program::panic_begin << "Error: <inner radius> value of <" << program_arguments_tag::circle << "> parameter is missing!" << program::panic_end;
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

	std::string content = encoding::heuristically_read_file(filepath); 

	std::stringstream content_stream(content);

	while(!content_stream.eof()) 
	{
		double x, y;

		content_stream >> std::skipws >> x >> std::skipws >> y;

		readed_points.push_back(Vector2(x, y));
	}
	
	// std::wifstream file_stream(filepath);
	// std::string line;

	// for(int line_number = 1; std::getline(file_stream, line); line_number++)
	// while(!file_stream.eof())
	// {
	// 	if (file_stream.bad())
	// 	{
	// 		panic_begin << "Failed to read file: (" << filepath << ")." << panic_end;
	// 	}

	// 	// BUG: 
	// 	// strtod does not read number if it reads non-number characters first. 
	// 	// It also returns 0 when it didn't read any number.

	// 	// {
	// 	// 	std::stringstream log;
	// 	// 	log << "Readed line: " << line << std::endl;
	// 	// 	std::cout << log.str();
	// 	// 	std::cout.flush();
	// 	// }

	// 	char *next_start;
	// 	double x, y;

	// 	// x = std::strtod(line.c_str(), &next_start);
	// 	file_stream >>  x >>  y;
		
	// 	{
	// 		std::stringstream log;
	// 		log << "Readed x: " << x << ", y: " << y << std::endl;
	// 		std::cout << log.str();
	// 		std::cout.flush();
	// 	}

	// 	// if (next_start == 0)
	// 	// {
	// 	// 	panic_begin 
	// 	// 		<< "Failed to read (x) position of the point at the line " << line_number 
	// 	// 		<< " in the file: (" << filepath << ")" << panic_end;
	// 	// }
		
	// 	// y = std::strtod(next_start, &next_start);

	// 	// if (next_start == 0)
	// 	// {
	// 	// 	panic_begin 
	// 	// 		<< "Failed to read (y) position of the point at the line " << line_number 
	// 	// 		<< " in the file (" << filepath << ")" << panic_end;
	// 	// }

	// 	readed_points.push_back(Vector2(x, y));
	// }

	// file_stream.close();
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

	// Defines mode
	for (int index = 0; index < input.size; index++)
	{
		auto *argument = input.array[index];

		if (strcmp(argument, program_arguments_tag::sequential_mode.c_str()) == 0)
		{
			algorithm = new Quick_Hull_Sequential();
			mode = Program_Mode::Algorithm_Execution;
			read_points_filepath();
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
				omp_set_num_threads(number_of_threads);
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
		algorithm = new Quick_Hull_OpenMP();
		read_points_filepath();
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
}
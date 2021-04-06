#pragma once

// standard
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

// internal
#include "core.hpp"
#include "algorithm/base.hpp"
#include "algorithm/sequential.hpp"
#include "algorithm/openmp.hpp"

using namespace quick_hull;

namespace program_arguments_tag 
{
	static const std::string help              = "--help";
	static const std::string points_filepath   = "--file";
	static const std::string log_verbose       = "--verbose";
	static const std::string log_quiet         = "--quiet";
	static const std::string file1             = "--file1"; // optional file

	static const std::string sequential_mode   = "--sequential";
	static const std::string thread_count      = "--threads";
	
	static const std::string points_generation = "--generate";
	static const std::string points_count      = "--count";
	static const std::string circle            = "--circle";
	static const std::string circle_center     = "--circle-center";
};

enum Program_Mode 
{
	Algorithm_Execution,
	Points_Generation
};

enum Program_Log_Level 
{
	Silent,
	Quiet,
	Verbose
};

struct Program_Input 
{
	public: // fields
		const char** array;
		const int size;
	public: // methods
		Program_Input(const char** array, const int size) : array{array}, size{size} { };
};

struct Points_Generation_Method 
{
	protected: // fields
		Program_Input input;
		int index;
		int count;

	public: // methods
		Points_Generation_Method
		(
			Program_Input input,
			const int start_index
		);

		virtual ~Points_Generation_Method() { }

		virtual bool find() = 0;
		virtual std::vector<Vector2> * execute() const = 0;
};

struct Circle_Points_Generation_Method : Points_Generation_Method
{
	private: // fields
		Vector2 center_point;
		double outer_radius;
		double inner_radius;
		bool is_found;

	public: // methods
		Circle_Points_Generation_Method(Program_Input input, const int start_index);

		std::vector<Vector2> * execute() const override;
		bool find() override;
	
	private:
		void read_double(double *store);
		bool move_next();
};

struct Program_Configuration 
{
	private: // fields
		Program_Input input;
		Program_Mode mode;
		Program_Log_Level log_level;
		Algorithm_Producing_Convex_Hull *algorithm;
		Points_Generation_Method *points_generation_method;

		// Could be use read to read/write points.
		std::string points_filepath;
		std::vector<Vector2> readed_points;

	private: // methods
		void read_points_filepath();
		void read_points_generation_method(int start_index);
		void read();

	public: // methods
		Program_Configuration(const char** argv, const int argc);
		~Program_Configuration();

		const Program_Mode & get_program_mode() const;
		const Program_Log_Level & get_log_level() const;
		const std::vector<Vector2> & get_points() const;

		Algorithm_Producing_Convex_Hull & get_algorithm() const;
		Points_Generation_Method & get_points_generation_method() const;

		bool is_using_sequential_algorithm() const;
};


namespace program 
{
	// Program's general utility code
	struct Panic_Message_End {   };
 	struct Panic_Message
	{
		public:
			template <typename T> 
			Panic_Message& operator<<(T argument)
			{
				std::cerr << argument;
				return *this;
			}

			Panic_Message& operator<< (Panic_Message_End& end);
	};

	extern Panic_Message     panic_begin;
	extern Panic_Message_End panic_end;


	struct Log_Stream_End { };
	struct Log_Stream 
	{
		private:
			std::stringstream stream;
		public:
			template<typename T>
			Log_Stream& operator<<(T argument)
			{
				stream << argument;
				return *this;
			}

			Log_Stream& operator<<(Log_Stream_End& end);
	};

	extern Log_Stream     log_begin;
	extern Log_Stream_End log_end;
}
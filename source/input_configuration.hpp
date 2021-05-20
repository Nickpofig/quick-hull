#pragma once

// standard
#include <string>

// internal
#include "console.hpp"

namespace program
{
	struct Input_Configuration 
	{
		private: // fields
			int     array_size;
			const char ** array;
		
		public: // constructor
			~Input_Configuration();
			Input_Configuration(int array_size, const char ** array);

		public: // getters
			int get_size() const;
			bool get_string(int index, std::string & result) const;
	};

	struct Input_Configuration_Iterator 
	{
		private: // fields
			const Input_Configuration & input;
			int index;

		public: // constructors & destructors
			~Input_Configuration_Iterator();
			Input_Configuration_Iterator(const Input_Configuration & input) :
				input(input), 
				index(-1) 
			{
			}
			
		public: // methods
			bool move_next();
			bool is_argument(std::string value) const;
			std::string get_argument_as_string() const;
			int get_argument_as_int() const;
			int get_argument_as_double() const;
	};
}
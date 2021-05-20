// standard
#include <cstring>

// internal
#include "console.hpp"
#include "input_configuration.hpp"

namespace program
{
	Input_Configuration::~Input_Configuration() 
	{
	}

	Input_Configuration::Input_Configuration(int array_size, const char ** array)
	{
		this->array = array;
		this->array_size = array_size;
	}

	int Input_Configuration::get_size() const
	{ 
		return array_size; 
	}

	bool Input_Configuration::get_string(int index, std::string & result) const 
	{
		// program::log_begin << "?." << program::log_end;

		if (index >= 0 && index < array_size) 
		{
			result = array[index];
			return true;
		}

		result = nullptr;
		return false;
	}



	Input_Configuration_Iterator::~Input_Configuration_Iterator() 
	{
	}

	bool Input_Configuration_Iterator::move_next() 
	{
		this->index += 1;
		return this->index >= 0 && this->index < input.get_size();
	}

	bool Input_Configuration_Iterator::is_argument(std::string value) const 
	{
		return std::strcmp(value.c_str(), get_argument_as_string().c_str()) == 0;
	}

	std::string Input_Configuration_Iterator::get_argument_as_string() const 
	{
		std::string result;

		/// TODO: add warning for fail case
		input.get_string(index, result); 

		return result;
	}

	int Input_Configuration_Iterator::get_argument_as_int() const 
	{
		std::string result;

		/// TODO: add warning for fail case
		input.get_string(index, result); 

		return std::stoi(result);
	}

	int Input_Configuration_Iterator::get_argument_as_double() const 
	{
		std::string result;

		/// TODO: add warning for fail case
		input.get_string(index, result); 

		return std::stod(result);
	}
}
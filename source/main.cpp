// standard
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cerrno>


// external

// internal
#include "program/config_program.hpp"


int main(const int argument_count, const char **arguments) 
{
	// Gets program parameters (if any)
	program::Configuration_Program configuration;

	// program::log_begin << "main 1." << program::log_end;

	configuration.try_initialize(program::Input_Configuration(argument_count, arguments), true);
	configuration.execute_application();

	return 0;
}
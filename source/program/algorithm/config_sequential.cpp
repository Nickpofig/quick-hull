// internal
#include "console.hpp"
#include "input_configuration.hpp"
#include "algorithm/sequential.hpp"
#include "program/constants.hpp"
#include "./config_sequential.hpp"

namespace program 
{
	bool Algorithm_Configuration_Sequential::try_initialize
	(
		Input_Configuration input, 
		bool allow_panic
	)
	{
		return true;
	}

	quick_hull::Algorithm * Algorithm_Configuration_Sequential::create_executor_instance() const 
	{
		return new quick_hull::Algorithm_Sequential();
	}

	std::string Algorithm_Configuration_Sequential::get_info_text() const 
	{
		return "Sequentual";
	}
}
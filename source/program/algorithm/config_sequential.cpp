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
		this->algorithm = new quick_hull::Algorithm_Sequential();
		return true;
	}

	std::string Algorithm_Configuration_Sequential::get_info_text() const 
	{
		return "Sequentual";
	}
}
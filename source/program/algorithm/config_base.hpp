#pragma once

// internal
#include "../config_base.hpp"

namespace program 
{
	struct Algorithm_Configuration_Base : public Configuration_Base
	{
		public:
			virtual std::string get_info_text() const = 0;
			virtual quick_hull::Algorithm * create_executor_instance() const = 0;
	};
}
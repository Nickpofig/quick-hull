#pragma once

// internal
#include "console.hpp"
#include "input_configuration.hpp"

namespace program 
{
	struct Configuration_Base
	{
		public:
			virtual bool try_initialize(Input_Configuration input, bool allow_panic) = 0;
	};

	struct Application_Configuration_Base : public Configuration_Base
	{
	};
}
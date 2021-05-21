#pragma once

// standard
#include "input_configuration.hpp"
#include "./config_base.hpp"

namespace program
{
	struct Algorithm_Configuration_Sequential : public Algorithm_Configuration_Base
	{
		public:// ..methods
			std::string get_info_text() const override;
			bool try_initialize(Input_Configuration input, bool allow_panic) override;
	};
}
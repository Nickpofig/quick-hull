#pragma once

// internal
#include "input_configuration.hpp"
#include "./config_base.hpp"
#include "./algorithm/config_base.hpp"

namespace program 
{
	enum Log_Mode 
	{
		None,
		Quiet,
		Verbose
	};


	struct Configuration_Program : public Configuration_Base
	{
		private:
			Application_Configuration_Base * application;
			Log_Mode                         log_mode;
		public:
			bool try_initialize(Input_Configuration input, bool allow_panic) override;
			void execute_application() const;
	};
}
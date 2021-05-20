#pragma once

// internal
#include "input_configuration.hpp"
#include "./config_base.hpp"

namespace program 
{
	struct Algorithm_Configuration_OpenMP : public Algorithm_Configuration_Base 
	{
		private: // ..fields
			int thread_count;

		public: // ..methods
			std::string get_info_text() const override;
			bool try_initialize(Input_Configuration input, bool allow_panic) override;
			quick_hull::Algorithm * create_executor_instance() const override;
	};
}
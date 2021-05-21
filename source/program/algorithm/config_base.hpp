#pragma once

// internal
#include "../config_base.hpp"

namespace program
{
	struct Algorithm_Configuration_Base : public Configuration_Base
	{
		protected: // ..fields
			quick_hull::Algorithm * algorithm;

		public: // ..getters
			quick_hull::Algorithm & get_executor_instance() const { return *algorithm; }

		public: // ..deconstructors
			~Algorithm_Configuration_Base() { delete algorithm; }

		public: // ..abstract methods
			virtual std::string get_info_text() const = 0;
			virtual std::string get_runtime_info_text() const { return std::string(""); }
	};
}
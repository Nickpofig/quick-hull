#pragma once

// standard
#include <string>

// internal
#include "input_configuration.hpp"
#include "./config_base.hpp"
#include "./config_generation_method.hpp"

namespace program 
{
	struct Application_Configuration_Points_Generation : public Application_Configuration_Base
	{
		private: // fields
			std::string                                        output_filepath;
			int                                                points_count;
			points_generation::Configuration_Generation_Base * points_generator;
			
		public: // getters
			int get_points_count() { return points_count; }
			std::string get_output_filepath() { return output_filepath; }
			
		public: // methods
			bool try_initialize(Input_Configuration input, bool allow_panic) override;
			void execute(std::vector<Vector2> & result) const;
	};
}
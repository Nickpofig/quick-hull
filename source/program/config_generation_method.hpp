#pragma once

// standard
#include <vector>

// internal
#include "core.hpp"
#include "./config_base.hpp"

namespace points_generation 
{
	struct Configuration_Generation_Base : public program::Configuration_Base
	{
		public:
			virtual void execute(std::vector<Vector2> & result, int count) const = 0;
	};


	struct Configuration_Generation_Circle : public Configuration_Generation_Base 
	{
		private:
			Vector2 center;
			double inner_radius;
			double outer_radius;
		
		public:
			bool try_initialize(program::Input_Configuration input, bool allow_panic) override;
			void execute(std::vector<Vector2> & result, int count) const override;
	};
}
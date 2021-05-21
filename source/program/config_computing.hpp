#pragma once

// internal
#include "input_configuration.hpp"

#include "./config_base.hpp"
#include "./algorithm/config_base.hpp"

namespace program 
{
	struct Application_Configuration_Computing : public Application_Configuration_Base 
	{
		private: // ..fields
			std::vector<Vector2>           points;
			Algorithm_Configuration_Base * algorithm_config;

		public: // ..getters
			int get_point_count() { return points.size(); }
			const Algorithm_Configuration_Base * get_algorithm_config() 
			{ 
				return algorithm_config;
			}

		public: // ..methods
			bool try_initialize(Input_Configuration input, bool allow_panic) override;
			
			void compute_convex_hull
			(
				std::vector<Vector2> & result_convex_hull, 
				double               & result_ellapsed_milliseconds,
				std::ostringstream   & result_runtime_info_buffer
			) const;
	};
}
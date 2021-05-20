// standard
#include <random>
#include <cerrno>


// internal
#include "console.hpp"
#include "./constants.hpp"
#include "./config_generation_method.hpp"


namespace points_generation 
{
	void Configuration_Generation_Circle::execute(std::vector<Vector2> & result, int count) const
	{
		std::random_device random_device;
		std::mt19937 generator(random_device());
		std::uniform_real_distribution<double> position_distribution(-1.0, 1.0);
		std::uniform_real_distribution<double> distance_distribution( 0.0, 1.0);


		result.clear();

		for (int index = 0; index < count; index++) 
		{
			// Creates random vector
			auto random_x = position_distribution(generator);
			auto random_y = position_distribution(generator);
			auto random_vector = Vector2(random_x, random_y).get_normalized();

			auto distance = ((outer_radius - inner_radius) * distance_distribution(generator)) + inner_radius;

			// Scales vector based on outer and inner radius, and shifts by center coordinates
			random_vector.x = (random_vector.x * distance) + center.x;
			random_vector.y = (random_vector.y * distance) + center.y;

			result.push_back(random_vector);
		}
	}

	bool Configuration_Generation_Circle::try_initialize
	(
		program::Input_Configuration input, 
		bool allow_panic
	) 
	{
		bool is_there_circle_generation_method = false;

		for (auto iterator = program::Input_Configuration_Iterator(input); iterator.move_next();)
		{
			if (iterator.is_argument(program_arguments_tag::generate_circle)) 
			{
				is_there_circle_generation_method = true;

				if (iterator.move_next()) 
				{
					this->outer_radius = iterator.get_argument_as_double();
				}
				else if (allow_panic) program::panic_begin << "Panic: circle inner radius is undefined!";

				if (iterator.move_next())
				{
					this->inner_radius = iterator.get_argument_as_double();
				}
				else if (allow_panic) program::panic_begin << "Panic: circle outer radius is undefined!";
			}
		}

		this->center = Vector2(0, 0);

		return is_there_circle_generation_method;
	}
}
// internal
#include "./config_generation_method.hpp"
#include "./config_generation.hpp"
#include "./constants.hpp"

namespace program 
{
	bool Application_Configuration_Points_Generation::try_initialize
	(
		Input_Configuration input, 
		bool allow_panic
	) 
	{
		// * Looks at input

		for(auto iterator = Input_Configuration_Iterator(input); iterator.move_next();) 
		{
			if (iterator.is_argument(program_arguments_tag::points_filepath)) 
			{
				if (iterator.move_next())
				{
					this->output_filepath = iterator.get_argument_as_string();
				}
				else if (allow_panic)
				{
					program::panic_begin << "Panic: points filepath is undefined!" << program::panic_end;
				}
			}
			else if (iterator.is_argument(program_arguments_tag::generate_size))
			{
				if (iterator.move_next())
				{
					this->points_count = iterator.get_argument_as_int();
				}
				else if (allow_panic)
				{
					program::panic_begin << "Panic: number of point is undefined!" << program::panic_end;
				}
			}
		};

		if (this->output_filepath.empty() || this->points_count == 0) 
		{
			return false;
		}

		this->points_generator = new points_generation::Configuration_Generation_Circle();

		if (this->points_generator->try_initialize(input, allow_panic) == false) 
		{
			return false;
		}

		return true;
	}

	void Application_Configuration_Points_Generation::execute
	(
		std::vector<Vector2> & result
	) 
	const
	{
		this->points_generator->execute(result, this->points_count);
	}
}
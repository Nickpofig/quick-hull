#include <iostream>
#include <fstream>
#include <cerrno>

#include "core.hpp"

namespace program 
{
	// Program's general utility code
	Panic_Message     panic_begin;
	Panic_Message_End panic_end;
	
	template <typename T> 
	Panic_Message& Panic_Message::operator<< (T argument)
	{
		std::cerr << argument;
		return *this;
	}

	Panic_Message& Panic_Message::operator<< (Panic_Message_End& end)
	{
		std::cerr << std::endl;
		exit(EXIT_FAILURE);
		return *this;
	}


	// Program's execution specific code
	Vector2::Vector2(double x, double y) : x(x), y(y) {   }

	Vector2 Vector2::operator-(const Vector2 &other)
	{
		return Vector2(this->x - other.x, this->y - other.y);
	}

	Vector2 Vector2::operator+(const Vector2 &other)
	{
		return Vector2(this->x + other.x, this->y + other.y);
	}

	bool Vector2::operator==(const Vector2 &other)
	{
		return this->x == other.x && this->y == other.y;
	}

	bool Vector2::operator!=(const Vector2 &other)
	{
		return !(*this == other);
	}

	double Vector2::get_sqr_magnitude() const
	{
		return dot_product(*this, *this);
	}

	Vector2 Vector2::get_conter_clockwise_normal() const
	{
		return Vector2(-(this->y), this->x);
	}



	double sqr_distance(const Vector2 &point_a , const Vector2 &point_b)
	{
		double delta_x = point_a.x - point_b.x;
		double delta_y = point_a.y - point_b.y;
		
		return (delta_x * delta_x) + (delta_y * delta_y);
	}

	double dot_product(const Vector2 &vector_a, const Vector2 &vector_b) 
	{
		return (vector_a.x * vector_b.x) + (vector_a.y * vector_b.y); 
	}

	Vector2 project(const Vector2 &vector_a, const Vector2 &vector_b)
	{
		double vector_b_sqr_magnitude = vector_b.get_sqr_magnitude();

		if (vector_b_sqr_magnitude == 0) 
		{
			return Vector2(0, 0);
		}
		
		double relativeness = dot_product(vector_a, vector_b);

		return Vector2(
			vector_b.x * (relativeness / vector_b_sqr_magnitude), 
			vector_b.y * (relativeness / vector_b_sqr_magnitude)
		);
	}


	void read_points_from_file
	(
		std::string const &filepath, 
		std::vector<Vector2> &points
	)
	{
		std::ifstream file_stream(filepath);
		std::string line;

		for(int line_number = 1; std::getline(file_stream, line); line_number++)
		{
			if (file_stream.bad())
			{
				panic_begin << "Failed to read file: (" << filepath << ")." << panic_end;
			}

			char *next_start;

			// BUG: 
			// strtod does not read number if it reads non-number characters first. 
			// It also returns 0 when it didn't read any number.

			double x = std::strtod(line.c_str(), &next_start);
			if (errno) 
			{
				panic_begin 
					<< "Failed to read (x) position of the point at the line " 
					<< line_number 
					<< " in the file: (" << filepath << ")"
					<< panic_end;
			}
			
			double y = std::strtod(next_start, &next_start);
			if (errno) 
			{
				panic_begin 
					<< "Failed to read (y) position of the point at the line " 
					<< line_number 
					<< " in the file (" << filepath << ")"
					<< panic_end;
			}

			points.push_back(Vector2(x, y));
		}

		file_stream.close();
	}
}
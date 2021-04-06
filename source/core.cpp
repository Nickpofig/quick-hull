#include <iostream>
#include <fstream>
#include <random>
#include <cerrno>

#include "core.hpp"

namespace quick_hull 
{
	// Program's general utility code
	Panic_Message     panic_begin;
	Panic_Message_End panic_end;

	Panic_Message& Panic_Message::operator<<(Panic_Message_End& end)
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

	Vector2 Vector2::operator/(const double number) 
	{
		auto x = this->x / number;
		auto y = this->y / number;

		return Vector2(x, y);
	}

	Vector2 Vector2::operator/=(const double number) 
	{
		return (*this) / number;
	}

	double Vector2::get_sqr_magnitude() const
	{
		return dot_product(*this, *this);
	}

	double Vector2::get_magnitude() const 
	{
		return std::sqrt(get_sqr_magnitude());
	}

	Vector2 Vector2::get_conter_clockwise_normal() const
	{
		return Vector2(-(this->y), this->x);
	}

	Vector2 Vector2::get_normalized() const 
	{
		auto copy = Vector2(*this);

		copy.normalize();

		return copy;
	}

	void Vector2::normalize()
	{
		double magnitude = this->get_magnitude();

		/// WARNING: could break when magnitude is zero
		this->x /= magnitude;
		this->y /= magnitude;
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

	std::vector<Vector2> * generate_points_in_circle(int count, Vector2 center, double outer_radius, double inner_radius)
	{
		std::random_device random_device;
		std::mt19937 generator(random_device());
		std::uniform_real_distribution<double> distribution(-1.0, 1.0);

		auto points = new std::vector<Vector2>();

		for (int index = 0; index < count; index++) 
		{
			// Creates random vector
			auto random_x = distribution(generator);
			auto random_y = distribution(generator);
			auto random_vector = Vector2(random_x, random_y).get_normalized();

			auto radius_range = outer_radius - inner_radius;

			// Scales vector based on outer and inner radius, and shifts by center coordinates
			random_vector.x = (random_vector.x * radius_range) + inner_radius + center.x;
			random_vector.y = (random_vector.y * radius_range) + inner_radius + center.y;

			points->push_back(random_vector);
		}

		return points;
	}
}
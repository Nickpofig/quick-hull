
// standard
#include <functional>
#include <iostream>
#include <fstream>
#include <random>
#include <cerrno>

// internal
#include "core.hpp"
#include "console.hpp"

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
	return Vector2::dot_product(*this, *this);
}

double Vector2::get_magnitude() const 
{
	return std::sqrt(get_sqr_magnitude());
}

size_t Vector2::get_hash() const
{
	size_t x_hash = std::hash<double>{}(this->x);
	size_t y_hash = std::hash<double>{}(this->y);

	return x_hash ^ (y_hash << 1);
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



double Vector2::sqr_distance(const Vector2 &point_a , const Vector2 &point_b)
{
	double delta_x = point_a.x - point_b.x;
	double delta_y = point_a.y - point_b.y;
	
	return (delta_x * delta_x) + (delta_y * delta_y);
}

double Vector2::dot_product(const Vector2 &vector_a, const Vector2 &vector_b) 
{
	return (vector_a.x * vector_b.x) + (vector_a.y * vector_b.y); 
}

Vector2 Vector2::project(const Vector2 &vector_a, const Vector2 &vector_b)
{
	double vector_b_sqr_magnitude = vector_b.get_sqr_magnitude();

	if (vector_b_sqr_magnitude == 0) 
	{
		return Vector2(0, 0);
	}
	
	double relativeness = Vector2::dot_product(vector_a, vector_b);

	return Vector2(
		vector_b.x * (relativeness / vector_b_sqr_magnitude), 
		vector_b.y * (relativeness / vector_b_sqr_magnitude)
	);
}
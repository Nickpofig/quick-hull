#pragma once

// standard
#include <vector>
#include <string>

// Program's execution specific code
struct Vector2
{
	public: // fields
		double x;
		double y;

	public: // methods
		Vector2(double x, double y);
		Vector2() = default;

		// Subtracts other vector from this vector
		Vector2 operator-(const Vector2 &other);
		Vector2 operator+(const Vector2 &other);
		Vector2 operator/(const double number);
		Vector2 operator/=(const double number);
		bool operator==(const Vector2 &other);
		bool operator!=(const Vector2 &other);

		// Returns this vector's squared magnitude
		double get_sqr_magnitude() const;
		double get_magnitude() const;
		size_t get_hash() const;
		Vector2 get_conter_clockwise_normal() const;
		Vector2 get_normalized() const;

		void normalize();

	public: // static methods
		static double  sqr_distance(const Vector2 &point_a , const Vector2 &point_b);
		static double  dot_product (const Vector2 &vector_a, const Vector2 &vector_b);
		static Vector2 project     (const Vector2 &vector_a, const Vector2 &vector_b); 
};
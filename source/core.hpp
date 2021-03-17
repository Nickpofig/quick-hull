#pragma once

// standard
#include <vector>
#include <string>

// external
#include "omp.h"

namespace program 
{
	// Program's general utility code
	struct Panic_Message_End {   };
 	struct Panic_Message
	{
		public:
			template <typename T> 
			Panic_Message& operator<< (T argument);
			Panic_Message& operator<< (Panic_Message_End& end);
	};

	extern Panic_Message     panic_begin;
	extern Panic_Message_End panic_end;


	// Program's execution specific code
	struct Vector2
	{
		public:
			double x;
			double y;

			Vector2(double x, double y);

			// Subtracts other vector from this vector
			Vector2 operator-(const Vector2 &other);
			Vector2 operator+(const Vector2 &other);
			bool operator==(const Vector2 &other);
			bool operator!=(const Vector2 &other);

			// Returns this vector's squared magnitude
			double get_sqr_magnitude() const;

			Vector2 get_conter_clockwise_normal() const;
	};


	extern double  sqr_distance(const Vector2 &point_a , const Vector2 &point_b);
	extern double  dot_product (const Vector2 &vector_a, const Vector2 &vector_b);
	extern Vector2 project     (const Vector2 &vector_a, const Vector2 &vector_b); 

	extern void read_points_from_file
	(
		std::string const &filepath, 
		std::vector<Vector2> &points
	);



	
	struct Thread_Logs 
	{
		private:
			std::string *buffers;

		public:
			void init(int number_of_threads);
			void flush();

			template <typename T>
			Thread_Logs& operator << (T any);
	};

	extern Thread_Logs thread_logs;
}
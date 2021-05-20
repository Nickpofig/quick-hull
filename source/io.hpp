#pragma once

// standard
#include <string>
#include <vector>

// internal
#include "core.hpp"


namespace program
{
	extern void read_points(std::string filepath, std::vector<Vector2> & points);
	extern void write_points(std::string filepath, const std::vector<Vector2> & points);
}
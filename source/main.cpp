#include <iostream>
#include <vector>
#include "algorithm/quick_hull.hpp"

int main(int argument_count, char **arguments) 
{
	Quick_Hull algorithm; // TODO: find a way to construct a class without making a copy

	algorithm.run();

	std::cout << "Test" << std::endl;

	return 0;
}
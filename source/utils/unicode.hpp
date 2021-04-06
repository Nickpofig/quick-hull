#pragma once

// standard
#include <string>

#define ENCODING_ASCII      0
#define ENCODING_UTF8       1
#define ENCODING_UTF16LE    2
#define ENCODING_UTF16BE    3

namespace encoding 
{
	std::string heuristically_read_file(std::string path);
}
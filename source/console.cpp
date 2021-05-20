// standard
#include <sstream>
#include <string>
#include <cstring>
#include <stdlib.h>

// internal
#include "console.hpp"


// Program's general utility code
namespace program 
{
	Panic_Message     panic_begin;
	Panic_Message_End panic_end;

	Panic_Message& Panic_Message::operator<<(Panic_Message_End& end)
	{
		std::cerr << std::endl;
		exit(EXIT_FAILURE);
		return *this;
	}

	Log_Stream     log_begin;
	Log_Stream_End log_end;

	Log_Stream& Log_Stream::operator<<(Log_Stream_End& end)
	{
		std::cout << stream.str() << std::endl;
		std::cout.flush();
		stream.str(""); // I hate c++ for this
		return *this;
	}
}
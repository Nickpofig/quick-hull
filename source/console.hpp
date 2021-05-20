#pragma once

// standard
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

// internal
#include "core.hpp"
#include "algorithm/base.hpp"


namespace program 
{
	// Program's general utility code
	struct Panic_Message_End {   };
 	struct Panic_Message
	{
		public:
			template <typename T> 
			Panic_Message& operator<<(T argument)
			{
				std::cerr << argument;
				return *this;
			}

			Panic_Message& operator<< (Panic_Message_End& end);
	};

	extern Panic_Message     panic_begin;
	extern Panic_Message_End panic_end;




	struct Log_Stream_End { };
	struct Log_Stream 
	{
		private:
			std::stringstream stream;
		public:
			template<typename T>
			Log_Stream& operator<<(T argument)
			{
				stream << argument;
				return *this;
			}

			Log_Stream& operator<<(Log_Stream_End& end);
	};

	extern Log_Stream     log_begin;
	extern Log_Stream_End log_end;
}
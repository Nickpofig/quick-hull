
// standard
#include <iostream>

// external
#include "omp.h"

// internal
#include "utils/buffers.hpp"

namespace rasul 
{
	// Buffer_Info

	Buffer_Info::~Buffer_Info() 
	{
		delete_func(pointer);
	}


	// Buffer_Storage

	// template<typename T>
	// std::vector<T> * Buffer_Storage::get(const char *buffer_name)
	


	// Multi_Thread_Buffer_Storage

	Multi_Thread_Buffer_Storage::Multi_Thread_Buffer_Storage()
	{
		int thread_count = omp_get_num_threads();
		buffer_storages = std::vector<Buffer_Storage>(thread_count);

		std::cout << "Multi thread buffer storage is constructed, with buffer storages: " 
		          << buffer_storages.size() << "." 
		          << std::endl;
	}

	// template<typename T>
	// std::vector<T> * Multi_Thread_Buffer_Storage::get_buffer(const char *name) 
	
}
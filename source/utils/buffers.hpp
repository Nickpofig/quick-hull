#pragma once

// standard
#include <vector>
#include <functional>


namespace rasul 
{
	struct Buffer_Info 
	{
		public:
			std::string name;
			std::function<void(void*)> delete_func; 
			void * pointer;

		public:
			Buffer_Info
			(
				std::string name, 
				std::function<void(void*)> delete_func, 
				void * pointer
			) : 
				name{name}, 
				delete_func{delete_func}, 
				pointer{pointer}
			{   };

			~Buffer_Info();
	};

	struct Buffer_Storage
	{
		private: // fields
			std::vector<Buffer_Info> list;

		public: // methods
			template<typename T> 
			std::vector<T> * get(std::string const& buffer_name)
			{
				for (const auto &buffer : list)
				{
					if (buffer.name.compare(buffer_name) == 0) 
					{
						return static_cast<std::vector<T> *>(buffer.pointer);
					}
				}

				auto *new_buffer = new std::vector<T>();

				list.push_back(Buffer_Info
				{
					buffer_name, 
					[](void * pointer)
					{
						auto* casted_pointer = static_cast<std::vector<T> *>(pointer);
						delete casted_pointer;
					},
					new_buffer
				});

				return new_buffer;
			}
	};

	struct Multi_Thread_Buffer_Storage 
	{
		private: // fields
			std::vector<Buffer_Storage> buffer_storages;

		public: // constructors
			Multi_Thread_Buffer_Storage();

		public: // methods
			template<typename T>
			std::vector<T> * get_buffer(std::string const& buffer_name)
			{
				int thread_number = omp_get_thread_num();
				auto &storage = buffer_storages[thread_number];

				return storage.get<T>(buffer_name);
			}
	};
}
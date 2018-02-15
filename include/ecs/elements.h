#pragma once
#include <cpprelude/defines.h>
#include <cpprelude/string.h>

namespace ecs
{
	constexpr cpprelude::u64 INVALID_ID = -1UL;

	struct World;

	struct Entity
	{
		cpprelude::u64 id = INVALID_ID;
		World* world;
	};

	struct Internal_Component
	{
		using free_func = void(*)(void*&, cpprelude::memory_context*);

		cpprelude::string type;
		void* data = nullptr;
		free_func destroy = nullptr;
	};
}

template<typename T>
void 
internal_component_dispose(void*& d, cpprelude::memory_context* _context)
{
	if (d == nullptr) return;
	T* data = (T*)d;
	auto data_slice = cpprelude::make_slice(data);
	_context->free(data_slice);
	d = nullptr;
}
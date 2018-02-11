#pragma once

#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/string.h>
 

namespace ecs
{
	constexpr cpprelude::u64 INVALID_ID = -1UL;

	struct World;

	struct Entity
	{
		cpprelude::u64 id = INVALID_ID;
		World* world = nullptr;
	};

	struct Internal_Component
	{
		using free_func = void(*) (void*&, cpprelude::memory_context*);

		cpprelude::u64 id = INVALID_ID;
		cpprelude::u64 entity_id = INVALID_ID;
		cpprelude::string name;
		cpprelude::string type;
		void* _data = nullptr;
		World* world = nullptr;
		free_func _destroy = nullptr;

		void destroy(cpprelude::memory_context* context)
		{
			_destroy(_data, context);
			world = nullptr;
		}

		~Internal_Component()
		{
			//destroy();
			//for now, we wil leave it like that
		}
	};

	template<typename T>
	void internal_component_dispose(void*& d, cpprelude::memory_context* _context)
	{
		if (d == nullptr) return;
		T* data = (T*)d;
		auto data_slice = make_slice(data);
		_context->free(data_slice);
		d = nullptr;
	}

	template<typename T>
	struct Component
	{
		Internal_Component* component;

		Component(Internal_Component* c)
			:component(c)
		{}

		T get_data()
		{
			return *(static_cast<T*>(component->_data));
		}
	};
}
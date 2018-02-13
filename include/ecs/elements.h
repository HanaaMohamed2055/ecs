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

		Internal_Component()
		{}

		Internal_Component(cpprelude::u64 cid, cpprelude::u64 eid, const cpprelude::string& ctype, const cpprelude::string& cname = "")
			:id(cid), entity_id(eid), type(ctype), name(cname)
		{}
		
		void
		destroy(cpprelude::memory_context* context)
		{
			_destroy(_data, context);
			world = nullptr;
		}
	};

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

	template<typename T>
	struct Component
	{
		Internal_Component* component;

		Component(Internal_Component* c)
			:component(c)
		{}

		T& 
		get_data()
		{
			return *(static_cast<T*>(component->_data));
		}

		const T&
		get_data() const
		{
			return *(static_cast<T*>(component->_data));
		}
	};
}
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
		World* world = nullptr;
	};

	struct Internal_Component
	{
		using free_func = void(*)(void*&, cpprelude::memory_context*);
		using copy_func = void(*)(void*, Internal_Component&, cpprelude::memory_context*);

		cpprelude::string type;
		void* data = nullptr;
		free_func destroy = nullptr;
		copy_func copy = nullptr;
	};


	template<typename T>
	struct component_view
	{
		cpprelude::sequential_iterator<Internal_Component> _begin, _current, _end;

		component_view(const cpprelude::sequential_iterator<Internal_Component>& begin, const cpprelude::sequential_iterator<Internal_Component>& end)
			:_begin(begin), _current(begin), _end(end)
		{}

		cpprelude::sequential_iterator<Internal_Component>
		next()
		{
			if (_current != _end)
				++_current;
			else
			{
				_current = _begin;
			}
			return _current;
		}

		bool
		end()
		{
			return _current == _end;
		}

		void
		reset()
		{
			_current = _begin;
		}

		T&
		data()
		{
			return *(static_cast<T*>(_current->data));
		}
	};
}

namespace utility
{
	template<typename T>
	void
	internal_component_dispose(void*& d, cpprelude::memory_context* _context)
	{
		if (d == nullptr) return;
		T* data = (T*)d;
		data->~T();
		auto data_slice = cpprelude::make_slice(data);
		_context->free(data_slice);
		d = nullptr;
	}

	template<typename T>
	void
	copy(void* d, ecs::Internal_Component& component, cpprelude::memory_context* _context)
	{
		cpprelude::string type(typeid(T).name(), _context);
		component.type = type;
		component.destroy = internal_component_dispose<T>;
		if (d != nullptr)
		{
			T data = *(static_cast<T*>(d));
			component.data = _context->alloc<T>();
			new (component.data) T(data);
		}
	}
}

# pragma once

#include <cpprelude/defines.h>

#include <ecs/elements.h>

namespace ecs
{
	template<typename T>
	struct type_view
	{
		cpprelude::sequential_iterator<Internal_Component> _begin, _current, _end;

		type_view(const cpprelude::sequential_iterator<Internal_Component>& begin, const cpprelude::sequential_iterator<Internal_Component>& end)
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
			assert(_current != _end);
			return *(static_cast<T*>(_current->data));
		}
	};
}

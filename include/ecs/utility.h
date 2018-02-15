# pragma once

#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>

#include <ecs/elements.h>

namespace ecs
{
	template<typename T>
	struct type_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = cpprelude::isize;
		using pointer = T*;
		using reference = T&;
		using data_type = T;
		
		cpprelude::sequential_iterator<Internal_Component> _element_it;
		cpprelude::usize _size = 0;


		type_iterator(Internal_Component* _element, cpprelude::usize size)
			:_element_it(_element), _size(size)
		{}

		type_iterator(const type_iterator& itr)
			:_element_it(itr._element_it._element), _size(itr._size)
		{}

		type_iterator&
		operator++()
		{
			 ++_element_it;
			 return *this;
		}

		type_iterator&
		operator++(int)
		{
			
			_element_it++:
		}

		bool operator==(const type_iterator& other) const
		{
			return _element_it == other._element_it;
		}

		bool operator!=(const type_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		data()
		{
			return *(static_cast<T*>(_element_it->data));
		}

		const value_type&
		data() const 
		{
			return *(static_cast<T*>(_element_it->data));
		}

		Internal_Component&
		value()
		{
			return *_element_it;
		}

		Internal_Component*
		operator->()
		{
			return _element_it;
		}

		Internal_Component&
		operator*()
		{
			return *_element_it;
		}
	};

	template<typename T>
	struct type_view
	{
		type_iterator<T> _begin, _current, _end;
		
		type_view(const type_iterator<T>& begin, const type_iterator<T>& end)
			:_begin(begin), _current(begin), _end(end)
		{}

		type_iterator<T> next()
		{
			if (_current != _end)
				++_current;
			else
				{
					_current = _begin;
				}
			return _current;
		}

		bool end()
		{
			return _current == _end;
		}

		type_iterator<T> reset()
		{
			_current = _begin;
		}

		T& data()
		{
			assert(_current != _end);
			return _current.data();
		}
	};
}
 
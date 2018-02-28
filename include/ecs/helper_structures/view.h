# pragma once
#include <cpprelude/iterator.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/elements.h>

namespace ecs
{
	struct generic_component_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = Component;
		using difference_type = cpprelude::isize;
		using pointer = Component*;
		using reference = Component&;
		using data_type = Component;

		Component* _values;
		cpprelude::sequential_iterator<cpprelude::usize> _index_it;

		generic_component_iterator(const sparse_set_iterator<Component>& set_it, cpprelude::sequential_iterator<cpprelude::usize> index_it)
			:_values(set_it._values), _index_it(index_it)
		{}

		generic_component_iterator(const generic_component_iterator& other)
			:_values(other._values), _index_it(other._index_it)
		{}

		generic_component_iterator&
		operator++()
		{
			++_index_it;
			return *this; 
		}

		generic_component_iterator&
		operator++(int)
		{
			auto result = *this;
			++_index_it;

			return result;
		}

		bool
		operator==(const generic_component_iterator& other) const
		{
			return _index_it == other._index_it
				&& _values == other._values;
		}

		bool
		operator!=(const generic_component_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		value()
		{
			return _values[*_index_it];
		}

		const value_type&
		value() const
		{
			return _values[*_index_it];
		}
		
		value_type&
		operator*()
		{
			return _values[*_index_it];
		}

		const value_type&
		operator*() const
		{
			return _values[*_index_it];
		}
	};
	
	template<typename T>
	struct component_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = cpprelude::isize;
		using pointer = T*;
		using reference = T&;
		using data_type = T;

		Component* _values;
		cpprelude::sequential_iterator<cpprelude::usize> _index_it;
		const char* _type;
		cpprelude::usize _size = 0;

		component_iterator(const sparse_set_iterator<Component>& set_it, cpprelude::sequential_iterator<cpprelude::usize> index_it, const char* type, cpprelude::usize size)
			:_values(set_it._values), _index_it(index_it), _type(type), _size(size)
		{
			while (_size > 0 && _values[*_index_it].utils->type != _type)
			{
				++_index_it;
				--_size;
			}
		}

		component_iterator(const component_iterator& other)
			:_values(other._values), _index_it(other._index_it), _type(other._type), _size(other._size)
		{
			while (_size > 0 && _values[*_index_it].utils->type != _type)
			{
				++_index_it;
				--_size;
			}
		}

		component_iterator&
		operator++()
		{
			++_index_it;
			--_size;
			
			while (_size > 0 && _values[*_index_it].utils->type != _type)
			{
				++_index_it;
				--_size;
			}

			return *this;
		}

		component_iterator&
		operator++(int)
		{
			auto result = *this;
			++_index_it;
			--_size;

			while (_size > 0 && _values[*_index_it].utils->type != _type)
			{
				++_index_it;
				--_size;
			}

			return result;
		}

		bool operator==(const component_iterator& other) const
		{
			return _index_it == other._index_it
				&& _values == other._values;
		}

		bool operator!=(const component_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		data()
		{
			auto component = _values[*_index_it];
			return *(static_cast<value_type*>(component.data));
		}

		const value_type&
		data() const
		{
			auto component = _values[*_index_it];
			return *(static_cast<value_type*>(component.data));
		}

		Component&
		value()
		{
			return _values[*_index_it];
		}

		value_type&
		operator*()
		{
			auto component = _values[*_index_it];
			return *(static_cast<value_type*>(component.data));
		}

		const value_type&
		operator*() const
		{
			auto component = _values[*_index_it];
			return *(static_cast<value_type*>(component.data));
		}
	};

	template<typename iterator>
	struct components_view
	{
		iterator _begin_it;
		iterator _end_it;

		components_view(iterator begin_it, iterator end_it)
			:_begin_it(begin_it), _end_it(end_it)
		{}

		iterator
		begin()
		{
			return _begin_it;
		}

		iterator
		end()
		{
			return _end_it;
		}
	};
}

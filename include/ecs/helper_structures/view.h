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
		using value_type = Internal_Component;
		using difference_type = cpprelude::isize;
		using pointer = Internal_Component*;
		using reference = Internal_Component&;
		using data_type = Internal_Component;

		sparse_unordered_set<Internal_Component>* component_set;
		cpprelude::sequential_iterator<cpprelude::usize> _index_it;

		generic_component_iterator()
			:component_set(nullptr), _index_it(nullptr)
		{}

		generic_component_iterator(sparse_unordered_set<Internal_Component>* set, const cpprelude::sequential_iterator<cpprelude::usize>& index_it)
			:component_set(set), _index_it(index_it)
		{}

		generic_component_iterator(const generic_component_iterator& other)
			:component_set(other.component_set), _index_it(other._index_it)
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
				&& component_set == other.component_set;
		}

		bool
		operator!=(const generic_component_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		value()
		{
			return component_set->at(*_index_it);
		}

		const value_type&
		value() const
		{
			return component_set->at(*_index_it);
		}
		
		value_type&
		operator*()
		{
			return component_set->at(*_index_it);
		}

		const value_type&
		operator*() const
		{
			return component_set->at(*_index_it);
		}

		value_type*
		operator->()
		{
			return component_set->_dense.data() + component_set->_sparse[*_index_it];
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

		sparse_unordered_set<Internal_Component>* component_set;
		cpprelude::sequential_iterator<cpprelude::usize> _index_it;
		const char* _type;
		cpprelude::usize _size = 0;

		component_iterator()
			:component_set(nullptr), _index_it(nullptr), _size(0)
		{}

		component_iterator(sparse_unordered_set<Internal_Component>* set, cpprelude::sequential_iterator<cpprelude::usize> index_it, const char* type, cpprelude::usize size)
			:component_set(set), _index_it(index_it), _type(type), _size(size)
		{
			while (_size > 0 && component_set->at(*_index_it).utils->type != _type)
			{
				++_index_it;
				--_size;
			}
		}

		component_iterator(const component_iterator& other)
			:component_set(other.component_set), _index_it(other._index_it), _type(other._type), _size(other._size)
		{
			while (_size > 0 && component_set->at(*_index_it).utils->type != _type)
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
			
			while (_size > 0 && component_set->at(*_index_it).utils->type != _type)
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

			while (_size > 0 && component_set->at(*_index_it).utils->type != _type)
			{
				++_index_it;
				--_size;
			}

			return result;
		}

		bool operator==(const component_iterator& other) const
		{
			return _index_it == other._index_it
				&& component_set == other.component_set;
		}

		bool operator!=(const component_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		data()
		{
			auto component = component_set->at(*_index_it);
			return *(static_cast<value_type*>(component.data));
		}

		const value_type&
		data() const
		{
			auto component = component_set->at(*_index_it);
			return *(static_cast<value_type*>(component.data));
		}

		Component&
		value()
		{
			return component_set->at(*_index_it);
		}

		value_type&
		operator*()
		{
			auto component = component_set->at(*_index_it);
			return *(static_cast<value_type*>(component.data));
		}

		const value_type&
		operator*() const
		{
			auto component = component_set->at(*_index_it);
			return *(static_cast<value_type*>(component.data));
		}

		Component*
		operator->()
		{
			return component_set->_dense.data() + component_set->_sparse[*_index_it];
		}
	};

	template<typename iterator>
	struct view
	{
		iterator _begin_it;
		iterator _end_it;

		view()
		{}

		view(iterator begin_it, iterator end_it)
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

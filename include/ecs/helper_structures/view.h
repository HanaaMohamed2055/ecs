# pragma once
#include <cpprelude/iterator.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/elements.h>

namespace ecs
{
	template<typename T>
	struct components_view
	{
		using iterator = cpprelude::sequential_iterator<cpprelude::usize>;

		iterator current;
		cpprelude::dynamic_array<cpprelude::usize>* _indices;
		sparse_unordered_set<Component>* _components;
		const char* _type;

		components_view(cpprelude::dynamic_array<cpprelude::usize>* indices, sparse_unordered_set<Component>* components, const char* type)
			:_indices(indices), _components(components), _type(type)
		{
			current = indices->begin();
			while (current != _indices->end() &&  _components->at(*current).utils->type != _type)
				++current;
		}

		T&
		get()
		{
			auto component = _components->at(*current);
			return *(static_cast<T*>(component.data));
		}

		T&
		operator*()
		{
			auto component = _components->at(*current);
			return *(static_cast<T*>(component.data));
		}

		void
		increment()
		{
			++current;
			if (current != _indices->end() && _components->at(*current).utils->type != _type)
				++current;
		}

		void
		operator++()
		{
			++current;
			if (current != _indices->end() && _components->at(*current).utils->type != _type)
				++current;

		}

		void
		reset()
		{
			current = _indices->begin();
		}

		iterator
		begin()
		{
			return _indices->begin();
		}

		iterator
		end()
		{
			return _indices->end();
		}

		bool
		reached_end()
		{
			return current == _indices->end();
		}
	};
	
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
	
	struct generic_components_view
	{
		using iterator = generic_component_iterator;

		iterator _begin_it;
		iterator _end_it;

		generic_components_view(const iterator& begin_it, const iterator& end_it)
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


	};
}

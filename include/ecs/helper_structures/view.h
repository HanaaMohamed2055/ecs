# pragma once
#include <cpprelude/iterator.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/elements.h>

namespace ecs
{
	struct generic_components_view
	{
		using iterator = cpprelude::sequential_iterator<cpprelude::usize>;

		iterator current;
		cpprelude::dynamic_array<cpprelude::usize>* _indices;
		sparse_unordered_set<Component>* _components;

		generic_components_view(cpprelude::dynamic_array<cpprelude::usize>* indices, sparse_unordered_set<Component>* components)
			:_indices(indices), _components(components)
		{
			current = indices->begin();
		}

		Component&
		get_component()
		{
			return _components->at(*current);
		}
		
		Component&
		operator*()
		{
			return _components->at(*current);
		}
				
		void
		increment()
		{
			if (current != _indices->end())
				++current;
		}

		void
		operator++()
		{
			if (current != _indices->end())
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

		cpprelude::sequential_iterator<cpprelude::usize> _index_it;
		
	};

	template<typename T>
	struct component_iterator
	{

	};
}

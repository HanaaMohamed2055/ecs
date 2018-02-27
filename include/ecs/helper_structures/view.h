# pragma once
#include <cpprelude/iterator.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/utility.h>
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
			return _components->get(*current);
		}
		
		Component&
		operator*()
		{
			return _components->get(*current);
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
		const char* type = utility::get_type_name<T>();

		components_view(cpprelude::dynamic_array<cpprelude::usize>* indices, sparse_unordered_set<Component>* components)
			:_indices(indices), _components(components)
		{
			current = indices->begin();
			while (current != _indices->end() &&  _components->at(*current).utils->type != type)
				++current;
		}

		T&
		get()
		{
			auto component = _components->get(*current);
			return *(static_cast<T*>(component.data));
		}

		T&
		operator*()
		{
			auto component = _components->get(*current);
			return *(static_cast<T*>(component.data));
		}

		void
		increment()
		{
			++current;
			if (current != _indices->end() && _components->at(*current).utils->type != type)
				++current;
		}

		void
		operator++()
		{
			++current;
			if (current != _indices->end() && _components->at(*current).utils->type != type)
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
}
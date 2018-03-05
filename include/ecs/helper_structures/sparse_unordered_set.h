# pragma once

#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>

#include <ecs/helper_structures/bag.h>

namespace ecs
{
	
	// This structure exists mainly to facilitate the following:
	// 1- iterating densely through the _dense container using range_based loops and iterators
	// 2- Maintaining access by ID/actual_index  and asking for membership

	template<typename T>
	struct sparse_unordered_set
	{
		using iterator = cpprelude::sequential_iterator<T>;
		using const_iterator = cpprelude::sequential_iterator<const T>;

		cpprelude::dynamic_array<T> _dense;
		bag<cpprelude::usize> _sparse;

		sparse_unordered_set(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_dense(context),
			_sparse(context)
		{}

						
		template<typename ... TArgs>
		cpprelude::usize
		emplace(TArgs&& ... args)
		{
			_dense.emplace_back(std::forward<TArgs>(args)...);
			cpprelude::usize actual_index = _sparse.insert(_dense.count() - 1);
						
			return actual_index;
		}

		cpprelude::usize
		insert(T&& value)
		{
			_dense.insert_back(std::move(value));
			cpprelude::usize actual_index = _sparse.insert(_dense.count() - 1);

			return actual_index;
		}

		cpprelude::usize
		insert(const T& value)
		{
			_dense.insert_back(value);
			cpprelude::usize actual_index = _sparse.insert(_dense.count() - 1);
					
			return actual_index;
		}

		bool
		has(cpprelude::usize actual_index)
		{
			return _sparse[actual_index] < _dense.count() && _sparse.is_slot_occupied(actual_index);
		}

		void
		remove(cpprelude::usize index)
		{
			cpprelude::usize actual_index = _sparse[index];
			_sparse.remove(actual_index);
			std::swap(_dense[actual_index], _dense[_dense.count() - 1]);
		}
		
		// [] and at are used to retrieve the object using its index in the sparse container
		T&
		operator[](cpprelude::usize actual_index)
		{
			return _dense[_sparse[actual_index]];
		}

		const T&
		operator[](cpprelude::usize actual_index) const
		{
			return _dense[_sparse[actual_index]];
		}

		T&
		at(cpprelude::usize actual_index)
		{
			return _dense[_sparse[actual_index]];
		}

		const T&
		at(cpprelude::usize actual_index) const
		{
			return _dense[_sparse[actual_index]];
		}
		
		cpprelude::usize
		count() const
		{
			return _dense.count();
		}

		bool
		empty() const
		{
			return _dense.count() == 0;
		}

		cpprelude::usize
		capacity() const
		{
			return _dense.capacity();
		}

		void
		reserve(cpprelude::usize expected_size)
		{
			_dense.reserve(expected_size);
			_sparse.reserve(expected_size);
		}

		const_iterator
		begin() const
		{
			return _dense.begin();
		}

		iterator
		begin()
		{
			return _dense.begin();
		}

		const_iterator
		cbegin() const
		{
			return _dense.cbegin();
		}

		const_iterator
		end() const
		{
			return _dense.end();
		}

		iterator
		end()
		{
			return _dense.end();
		}

		const_iterator
		cend()
		{
			return _dense.end();
		}

		~sparse_unordered_set()
		{

		}
	};
}

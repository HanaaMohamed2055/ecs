# pragma once

#include <cpprelude/defines.h>
#include <cpprelude/dynamic_array.h>
#include <cpprelude/stack_array.h>
#include <cpprelude/platform.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/iterator.h> 

namespace ecs
{
	template<typename T>
	struct bag
	{
		//there should be an iterator here
		using iterator = cpprelude::sequential_iterator<T>;
		using const_iterator = cpprelude::sequential_iterator<const T>;

		//dynamic array of the T with some form of other data that could occupy a byte or so
		cpprelude::dynamic_array<T> _data;
		cpprelude::stack_array<cpprelude::usize> _free_indices;

		//more constructors could be added
		bag(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_data(context),
			_free_indices(context)
		{}

		~bag()
		{
			clear();
		}

		template<typename ... TArgs>
		cpprelude::usize
		emplace(TArgs&& ... args)
		{
			cpprelude::usize place = _data.count();

			if (!_free_indices.empty())
			{
				place = _free_indices.top();
				_free_indices.pop();
				new (_data.data() + place) T(std::forward<TArgs>(args)...);
				return place;
			}

			_data.emplace_back(std::forward<TArgs>(args)...);
			return place;
		}

		cpprelude::usize
		insert(const T& value)
		{
			cpprelude::usize place = _data.count();

			if (!_free_indices.empty())
			{
				place = _free_indices.top();
				_free_indices.pop();
				new (_data.data() + place) T(value);
				return place;
			}

			_data.emplace_back(value);
			return place;
		}

		cpprelude::usize
		insert(T&& value)
		{
			cpprelude::usize place = _data.count();

			if (!_free_indices.empty())
			{
				place = _free_indices.top();
				_free_indices.pop();
				new (_data.data() + place) T(std::move(value));
				return place;
			}

			_data.emplace_back(std::move(value));
			return place;
		}

		void
		remove(cpprelude::usize index)
		{
			_data[index].~T();
			_free_indices.push(index);
		}

		T&
		operator[](cpprelude::usize index)
		{
			return _data[index];
		}

		const T&
		operator[](cpprelude::usize index) const
		{
			return _data[index];
		}

		cpprelude::usize
		count() const
		{
			return _data.count() - _free_indices.count();
		}

		bool
		empty() const
		{
			return count() == 0;
		}

		cpprelude::usize
		capacity() const
		{
			return _data.capacity()
		}

		void
		reserve(cpprelude::usize expected_size)
		{
			cpprelude::usize available_size = _data.capacity() - _data.count() + _free_indices.count();
			if (available_size >= expected_size)
				return;
			_data.reserve(expected_size - available_size);
		}
		
		void
		clear()
		{
			for (T element: _data)
				element.~T();
			_data.clear();
			_free_indices.decay();
		}

		//iterator interface here
		const_iterator
		front() const
		{
			return _data.front();
		}
		
		iterator
		front()
		{
			return _data.front();
		}
		
		const_iterator
		back() const
		{
			return _data.back();
		}

		iterator
		back()
		{
			return _data.back();
		}

		const_iterator
		begin() const 
		{
			return _data.begin();
		}

		const_iterator
		cbegin() const
		{
			return _data.cbegin();
		}

		iterator
		begin()
		{
			return _data.begin();
		}

		const_iterator
		end() const
		{
			return _data.end();
		}

		const_iterator
		cend() const
		{
			return _data.cend();
		}

		iterator
		end()
		{
			return _data.end();
		}
	};

}
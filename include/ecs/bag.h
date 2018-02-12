# pragma once

#include <cpprelude/defines.h>
#include <cpprelude/dynamic_array.h>
#include <cpprelude/stack_array.h>
#include <cpprelude/platform.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/iterator.h> 

namespace ecs
{
	namespace details
	{
		template<typename T>
		struct cell
		{
			T _content;
			bool is_valid;
			
			cell(T&& data)
			{
				_content = std::move(data);
				is_valid = true;
			}

			cell(T& data)
			{
				_content = data;
				is_valid = true;
			}
		};
	}

	template<typename T>
	struct bag_iterator;

	template<typename T>
	struct bag
	{
		//there should be an iterator here
		using iterator = bag_iterator<T>;
		using const_iterator = bag_iterator<const T>;

		//dynamic array of the T with some form of other data that could occupy a byte or so
		cpprelude::dynamic_array<T> _data;
		cpprelude::dynamic_array<bool> _valid;
		cpprelude::stack_array<cpprelude::usize> _free_indices;

		//more constructors could be added
		bag(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_data(context),
			_free_indices(context)
		{
			reserve(8);
		}

		~bag()
		{
			clear();
		}

		template<typename ... TArgs>
		cpprelude::usize
		emplace(TArgs&& ... args)
		{
			cpprelude::usize place = _data.count();
			_valid.insert_back(true);

			if (!_free_indices.empty())
			{
				place = _free_indices.top();
				_free_indices.pop();
				new (_data.data() + place) T(std::forward<TArgs>(args)...);
				_valid[place] = true;
				return place;
			}

			_valid.insert_back(true);
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
				_valid[place] = true;
				return place;
			}

			_valid.insert_back(true);
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
				_valid[place] = true;
				return place;
			}

			_valid.insert_back(true);
			_data.emplace_back(std::move(value));
			return place;
		}

		void
		remove(cpprelude::usize index)
		{
			_valid[index] = false;
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

		bool
		valid(cpprelude::usize index)
		{
			
			return (index < _valid.count() && _valid[index]);
		}

		void
		reserve(cpprelude::usize expected_size)
		{
			cpprelude::usize available_size = _data.capacity() - _data.count() + _free_indices.count();
			if (available_size >= expected_size)
				return;
			_data.reserve(expected_size - available_size);
			_valid.reserve(expected_size - available_size);
		}
		
		void
		clear()
		{
			for (cpprelude::usize i = 0; i < _data.count(); ++i)
			{ 
				if (_valid[i])
					_data[i].~T();
			}
			_data._count = 0;
		}

		//iterator interface here
		const_iterator
		back() const
		{
			return const_iterator(_data.back(), _valid.back(), 0);
		}

		iterator
		back()
		{
			return iterator(_data.back(), _valid.back(), 0);
		}

		const_iterator
		begin() const 
		{
			const_iterator result(_data.begin(), _valid.begin(), _data.count());

			if (*result._flag_it == false)
				++result;

			return result;
		}

		const_iterator
		cbegin() const
		{
			const_iterator result(_data.begin(), _valid.begin(), _data.count());

			if (*result._flag_it == false)
				++result;

			return result;
		}

		iterator
		begin()
		{
			iterator result(_data.begin(), _valid.begin(), _data.count());

			if (*result._flag_it == false)
				++result;

			return result;
		}

		const_iterator
		end() const
		{
			return const_iterator(_data.end(), _valid.end(), 0);
		}

		const_iterator
		cend() const
		{
			return const_iterator(_data.end(), _valid.end(), 0);
		}

		iterator
		end()
		{
			return iterator(_data.end(), _valid.end(), 0);
		}
	};

	template<typename T>
	struct bag_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = cpprelude::isize;
		using pointer = T*;
		using reference = T&;
		using data_type = T;
		cpprelude::sequential_iterator<value_type> _element_it;
		cpprelude::sequential_iterator<bool> _flag_it;
		cpprelude::usize _size = 0;


		bag_iterator(value_type* _element, bool* flag, cpprelude::usize size)
			:_element_it(_element), _flag_it(flag), _size(size)
		{}
		
		bag_iterator&
		operator++()
		{
			++_flag_it;
			++_element_it;
			--_size;

			while(*_flag_it == false && _size > 0)
			{
				++_flag_it;
				++_element_it;
				--_size;
			}

			return *this;	
		}

		bag_iterator&
		operator++(int)
		{
			auto result = *this;

			++_flag_it;
			++_element_it;
			--_size;

			while(*_flag_it == false && _size > 0)
			{
				++_flag_it;
				++_element_it;
				--_size;
			}
		}

		bool operator==(const bag_iterator& other) const
		{
			return _flag_it == other._flag_it &&
				   _element_it == other._element_it;	
		}

		bool operator!=(const bag_iterator& other) const
		{
			return !operator==(other);
		}

		// here we should add const_bag_iterator

		value_type&
		value()
		{
			return *_element_it;
		}

		value_type*
		operator->()
		{
			return _element_it;
		}
		
		value_type&
		operator*()
		{
			return *_element_it;
		}
	
	};
}
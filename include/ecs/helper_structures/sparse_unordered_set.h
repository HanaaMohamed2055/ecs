# pragma once

#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>

#include <ecs/helper_structures/bag.h>

namespace ecs
{
	template<typename T>
	struct sparse_set_iterator;

	template<typename T>
	struct sparse_unordered_set
	{
		using iterator = sparse_set_iterator<T>;
		using const_iterator = sparse_set_iterator<const T>;

		cpprelude::dynamic_array<cpprelude::usize> _dense;
		bag<T> _sparse;

		sparse_unordered_set(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_dense(context),
			_sparse(context)
		{}

						
		template<typename ... TArgs>
		void
		emplace(TArgs&& ... args)
		{
			cpprelude::usize index = _sparse.emplace(std::forward<TArgs>(args)...);
			_dense.insert_back(index);
		}

		void
		insert(T&& value)
		{
			cpprelude::usize index = _sparse.insert(std::move(value));
			_dense.insert_back(index);
		}

		void
		insert(const T& value)
		{
			cpprelude::usize index = _sparse.insert(value);
			_dense.insert_back(index);
		}

		bool
		remove(cpprelude::usize index)
		{
			auto itr = std::find(_dense.begin(), _dense.end(), index);
			if (itr != _dense.end())
			{
				std::swap(_dense[_dense.count() - 1], *itr);
				_dense.remove_back();
				_sparse.remove(index);
				return true;
			}
			return false;
		}
		
		T&
		operator[](cpprelude::usize index)
		{
			return _sparse[_dense[index]];
		}

		const T&
		operator[](cpprelude::usize index) const
		{
			return _sparse[_dense[index]];
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
			return const_iterator(_sparse._data._data_block.ptr, _dense._data_block.ptr);
		}

		iterator
		begin()
		{
			return iterator(_sparse._data._data_block.ptr, _dense._data_block.ptr);
		}

		const_iterator
		cbegin() const
		{
			return const_iterator(_sparse._data._data_block.ptr, _dense._data_block.ptr);
		}

		const_iterator
		end() const
		{
			return const_iterator(_sparse._data._data_block.ptr,
				_dense._data_block.ptr + _dense.count());
		}

		iterator
		end()
		{
			return iterator(_sparse._data._data_block.ptr,
				_dense._data_block.ptr + _dense.count());
		}

		const_iterator
		cend()
		{
			return const_iterator(_sparse._data._data_block.ptr,
				_dense._data_block.ptr + _dense.count());
		}

		~sparse_unordered_set()
		{

		}
	};

	template<typename T>
	struct sparse_set_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = cpprelude::isize;
		using pointer = T*;
		using reference = T&;
		using data_type = T;

		T* _value_it;
		cpprelude::usize* _index_it;
			
		sparse_set_iterator(value_type* value_it, cpprelude::usize* index_it)
			:_value_it(value_it), _index_it(index_it)
		{}

		sparse_set_iterator&
		operator++()
		{
			++_index_it;
			return *this;
		}

		sparse_set_iterator&
		operator++(int)
		{
			auto result = *this;
			++_index_it;
			return result;
		}

		bool
		operator==(const sparse_set_iterator& other) const
		{
			return _index_it == other._index_it 
				&& _value_it == other._value_it;
		}

		bool
		operator!=(const sparse_set_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		value()
		{
			return _value_it[*_index_it];
		}

		const value_type&
		value() const
		{
			return _value_it[*_index_it];
		}
				
		value_type&
		operator*()
		{
			return _value_it[*_index_it];
		}
	};
}
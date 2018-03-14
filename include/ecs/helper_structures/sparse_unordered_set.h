# pragma once

#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>
#include <cpprelude/dynamic_array.h>
#include <cpprelude/stack_array.h>

#include <ecs/elements.h>

namespace ecs
{
	
	// This structure exists mainly to facilitate the following:
	// 1- iterating densely through the _dense container using range_based loops and iterators
	// 2- Maintaining access by ID/actual_index  and asking for membership

	template<typename T>
	struct sparse_unordered_set
	{
		using iterator = cpprelude::sequential_iterator<std::pair<T, cpprelude::usize>>;
		using const_iterator = cpprelude::sequential_iterator<std::pair<const T, cpprelude::usize>>;

		cpprelude::dynamic_array <std::pair <T, cpprelude::usize>> _dense;
		cpprelude::dynamic_array<cpprelude::isize> _sparse;
		cpprelude::isize next = -1;
		cpprelude::usize recycle_count = 0;

		sparse_unordered_set(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_dense(context),
			_sparse(context)
		{}
								
		template<typename ... TArgs>
		cpprelude::usize
		emplace(TArgs&& ... args)
		{
			cpprelude::usize place = _sparse.count();
			
			if (recycle_count > 0)
			{
			}

			_dense.emplace_back(std::make_pair(std::forward<TArgs>(args)..., place));
			_sparse.insert_back(_dense.count() - 1);

			return place;
		}

		cpprelude::usize
		insert(T&& value)
		{
			cpprelude::usize place = _sparse.count();

			if (recycle_count > 0)
			{
			}

			_dense.emplace_back(std::make_pair(std::move(value), place));
			_sparse.insert_back(_dense.count() - 1);
			
			return place;
		}

		cpprelude::usize
		insert(const T& value)
		{
			cpprelude::usize place = _sparse.count();

			if (recycle_count > 0)
			{
			}

			_dense.emplace_back(std::make_pair(value, place));
			_sparse.insert_back(_dense.count() - 1);
			
			return place;
		}

		bool
		has(cpprelude::usize index)
		{
			return _sparse[index] < _dense.count() 
				&& _sparse[index] > -1 
				&& _dense[_sparse[index]].second == index ;
		}

		void
		remove(cpprelude::usize sparse_index)
		{
			cpprelude::usize dense_index = _sparse[sparse_index];
			
			std::swap(_dense[dense_index], _dense[_dense.count() - 1]);
			_sparse[_dense[dense_index].second] = dense_index;
			
			_sparse[sparse_index] = -1;
			_dense.remove_back();
			// here we should add some logic for next 
		}
		
		// [] and at are used to retrieve the object using its index in the sparse container
		T&
		operator[](cpprelude::usize sparse_index)
		{
			return _dense[_sparse[sparse_index]].first;
		}

		const T&
		operator[](cpprelude::usize sparse_index) const
		{
			return _dense[_sparse[sparse_index]].first;
		}

		T&
		at(cpprelude::usize sparse_index)
		{
			return _dense[_sparse[sparse_index]].first;
		}

		const T&
		at(cpprelude::usize sparse_index) const
		{
			return _dense[_sparse[sparse_index]].first;
		}
		
		cpprelude::usize
		count() const
		{
			return _dense.count();
		}

		cpprelude::usize
		sparse_count() const
		{
			return _sparse.count();
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

		// iterators for range-based loops
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
		{}
	};

	// this specialization is intended for use inside this entity-component system
	template<>
	struct sparse_unordered_set<Entity>
	{
		using iterator = cpprelude::sequential_iterator<ID>;
		using const_iterator = cpprelude::sequential_iterator<const ID>;

		cpprelude::dynamic_array<ID> _dense;
		cpprelude::dynamic_array<ID> _sparse;
		cpprelude::usize recycle_count = 0;
		ID next;

		sparse_unordered_set(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_dense(context),
			_sparse(context)
		{}

		ID
		insert_one_more()
		{
			if (recycle_count)
			{
				// here we will implement the recycling part
				_dense.insert_back(next);
				auto entity_id = next;
				next = _sparse[next.id()];
				_sparse[entity_id.id()] = ID(_dense.count() - 1, entity_id.version());
				--recycle_count;
				return entity_id;
			}
			else
			{
				_dense.insert_back(ID{ _dense.count() });
				_sparse.insert_back(ID{ _dense.count() - 1 });
				return _sparse.count() - 1;
			}
		}

		bool
		has(cpprelude::usize sparse_index)
		{
			return _sparse[sparse_index].id() < _dense.count()
				&& _dense[_sparse[sparse_index].id()].id() == sparse_index;
		}

		bool
		has(Entity entity)
		{
			return _sparse[entity.id()].version() == entity.version() && _sparse[entity.id()].id() < _dense.count();
		}

		void
		remove(cpprelude::usize sparse_index)
		{
			cpprelude::usize dense_index = _sparse[sparse_index].id();

			std::swap(_dense[dense_index], _dense[_dense.count() - 1]);
			_sparse[_dense[dense_index].id()] = ID{dense_index};
			_dense.remove_back();

			ID temp = next;
			next = _sparse[sparse_index];
			next.increment_version();
			if (recycle_count)
				_sparse[sparse_index] = temp;
					
			++recycle_count;
		}

		ID&
		operator[](cpprelude::usize sparse_index)
		{
			return _dense[_sparse[sparse_index].id()];
		}

		const ID&
		operator[](cpprelude::usize sparse_index) const
		{
			return _dense[_sparse[sparse_index].id()];
		}

		cpprelude::usize
		count() const
		{
			return _dense.count();
		}

		cpprelude::usize
		sparse_count() const
 		{
			return _sparse.count();
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
		{}
	};
}

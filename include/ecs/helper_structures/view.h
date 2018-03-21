# pragma once
#include <cpprelude/iterator.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/elements.h>
#include <ecs/utility.h>
namespace ecs
{

	struct component_pool
	{
		utility::base_type_utils* utils = nullptr;
		cpprelude::memory_context* _context = nullptr;

		cpprelude::dynamic_array<void*> components;
		cpprelude::dynamic_array<bool> managed;
		cpprelude::dynamic_array<cpprelude::usize> sparse;
		cpprelude::dynamic_array<cpprelude::usize> dense;

		component_pool(cpprelude::memory_context* context)
			:_context(context),
			components(context),
			sparse(context),
			dense(context)
		{}

		component_pool()
			:_context(nullptr)
		{}

		void
		insert_at(cpprelude::usize entity_index, void* data_ptr, bool allocated_by_pool = true)
		{
			if (components.capacity() <= entity_index)
			{
				components.expand_back(2 * (entity_index + 1), nullptr);
				managed.expand_back(2 * (entity_index + 1), false);
				sparse.expand_back(2 * (entity_index + 1), INVALID_PLACE);
			}
			components[entity_index] = data_ptr;
			managed[entity_index] = allocated_by_pool;
			sparse[entity_index] = dense.count();
			dense.insert_back(entity_index);
		}

		void
		remove(cpprelude::usize entity_index)
		{
			if (entity_index < components.count())
			{
				if (managed[entity_index])
				{
					utils->free(components[entity_index], _context);
					managed[entity_index] = false;
				}
				components[entity_index] = nullptr;
				cpprelude::usize dense_index = sparse[entity_index];
				std::swap(dense[dense_index], dense[dense.count()]);
				sparse[dense[dense_index]] = dense_index;
				sparse[entity_index] = -1;
				dense.remove_back();
			}
		}

		bool
		has(cpprelude::usize entity_index) const
		{
			return entity_index < components.count()
				&& components[entity_index] != nullptr;
		}

	};
	//struct generic_component_iterator
	//{
	//	using component_iterator = cpprelude::sequential_iterator<Internal_Component>;
	//	
	//	using iterator_category = std::forward_iterator_tag;
	//	using value_type = Internal_Component;
	//	using difference_type = cpprelude::isize;
	//	using pointer = Internal_Component*;
	//	using reference = Internal_Component&;
	//	using data_type = Internal_Component;

	//	component_iterator _component_it;
	//	cpprelude::sequential_iterator<component_pool> _pool_it;
	//	cpprelude::usize _pool_count = 0;
	//	cpprelude::usize _component_count = 0; // per pool

	//	generic_component_iterator(cpprelude::sequential_iterator<component_pool> pool_it, cpprelude::usize pool_count)
	//		:_pool_it(pool_it), _pool_count(pool_count)
	//	{
	//		if (_pool_count)
	//		{
	//			--_pool_count;
	//			_component_count = _pool_it->components.count() - 1;
	//			_component_it = _pool_it->components.begin();
	//		}
	//		else
	//			_component_count = 0;
	//	}

	//	generic_component_iterator(const generic_component_iterator& other)
	//		:_pool_it(other._pool_it), _pool_count(other._pool_count)
	//	{
	//		if (_pool_count)
	//		{
	//			--_pool_count;
	//			_component_count = _pool_it->components.count() - 1;
	//			_component_it = _pool_it->components.begin();
	//		}
	//		else
	//			_component_count = 0;
	//	}

	//	generic_component_iterator&
	//	operator++()
	//	{
	//		if (_component_count)
	//		{
	//			++_component_it;
	//			--_component_count;
	//		}
	//		else if (_pool_count)
	//		{
	//			++_pool_it;
	//			_component_count = _pool_it->components.count() - 1;
	//			_component_it = _pool_it->components.begin();
	//			--_pool_count;
	//		}
	//		else if (!_pool_count)
	//			++_pool_it;
	//	
	//		return *this; 
	//	}

	//	generic_component_iterator&
	//	operator++(int)
	//	{
	//		auto result = *this;
	//		
	//		if (_component_count)
	//		{
	//			++_component_it;
	//			--_component_count;
	//		}
	//		else if (_pool_count)
	//		{
	//			++_pool_it;
	//			_component_count = _pool_it->components.count() - 1;
	//			_component_it = _pool_it->components.begin();
	//			--_pool_count;
	//		}
	//		else if (!_pool_count)
	//			++_pool_it;

	//		return result;
	//	}

	//	bool
	//	operator==(const generic_component_iterator& other) const
	//	{
	//		return _pool_it == other._pool_it
	//			&& _component_count == other._component_count;
	//	}

	//	bool
	//	operator!=(const generic_component_iterator& other) const
	//	{
	//		return !operator==(other);
	//	}

	//	ID
	//	entity()
	//	{
	//		return _component_it->entity_id;
	//	}

	//	value_type&
	//	value()
	//	{
	//		return *_component_it;
	//	}

	//	const value_type&
	//	value() const
	//	{
	//		return *_component_it;
	//	}
	//	
	//	value_type&
	//	operator*()
	//	{
	//		return *_component_it;
	//	}

	//	const value_type&
	//	operator*() const
	//	{
	//		return *_component_it;
	//	}

	//	const char*
	//	type() const
	//	{
	//		return _pool_it->utils->type;
	//	}
	//			
	//};

	
	template<typename T>
	struct component_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = cpprelude::isize;
		using pointer = T*;
		using reference = T&;
		using data_type = T;

		component_pool& _pool;
		cpprelude::sequential_iterator<cpprelude::usize> _dense_it;
		

		component_iterator(component_pool& pool, const cpprelude::sequential_iterator<cpprelude::usize>& dense_it)
			:_pool(pool), _dense_it(dense_it)
		{}
	
		component_iterator(const component_iterator& other)
			:_pool(other._pool), _dense_it(other._dense_it)
		{}

		component_iterator&
		operator++()
		{
			++_dense_it;
			return *this;
		}

		component_iterator&
		operator++(int)
		{
			auto result = *this;
			++_dense_it;

			return result;
		}

		bool operator==(const component_iterator& other) const
		{
			return _dense_it == other._dense_it;
		}

		bool operator!=(const component_iterator& other)
		{
			return !operator==(other);
		}

		value_type&
		value()
		{
			void* data = _pool.components[*_dense_it];
			return *(static_cast<value_type*>(data));
		}

		const value_type&
		value() const
		{
			void* data = _pool.components[*_dense_it];
			return *(static_cast<value_type*>(data));
		}

		cpprelude::usize
		entity()
		{
			return *_dense_it;
		}

		value_type&
		operator*()
		{
			void* data = _pool.components[*_dense_it];
			return *(static_cast<value_type*>(data));
		}

		const value_type&
		operator*() const
		{
			void* data = _pool.components[*_dense_it];
			return *(static_cast<value_type*>(data));
		}

		value_type*
		operator->()
		{
			void* data = _pool.components[*_dense_it];
			return (static_cast<value_type*>(data));
		}
	};

	struct entity_components_iterator
	{
	/*	using component_iterator = cpprelude::sequential_iterator<Internal_Component>;

		using iterator_category = std::forward_iterator_tag;
		using value_type = Internal_Component;
		using difference_type = cpprelude::isize;
		using pointer = Internal_Component*;
		using reference = Internal_Component&;
		using data_type = Internal_Component;

		cpprelude::sequential_iterator<component_pool> _pool_it;
		cpprelude::usize _pool_count = 0;
		_id_type _entity_id;
		
		entity_components_iterator(cpprelude::sequential_iterator<component_pool> pool_it, cpprelude::usize pool_count, _id_type entity_id)
			:_pool_it(pool_it), _pool_count(pool_count), _entity_id(entity_id)
		{
			if (_pool_count)
				--pool_count;
			
			while (_pool_count && !_pool_it->components.has(_entity_id))
			{
				++_pool_it;
				--_pool_count;
			}
		}

		entity_components_iterator(const entity_components_iterator& other)
			:_pool_it(other._pool_it), _pool_count(other._pool_count), _entity_id(other._entity_id)
		{
			if (_pool_count)
				--_pool_count;
			
			while (_pool_count && !_pool_it->components.has(_entity_id))
			{
				++_pool_it;
				--_pool_count;
			}
		}

		entity_components_iterator&
		operator++()
		{
			++_pool_it;
			--_pool_count;

			while (_pool_count > 0 && !_pool_it->components.has(_entity_id))
			{
				++_pool_it;
				--_pool_count;
			}

			return *this;
		}

		entity_components_iterator&
		operator++(int)
		{
			auto result = *this;
			++_pool_it;
			--_pool_count;

			while (_pool_count > 0 && !_pool_it->components.has(_entity_id))
			{
				++_pool_it;
				--_pool_count;
			}
		
			return result;
		}
		
		bool operator==(const entity_components_iterator& other) const
		{
			return _pool_it == other._pool_it
				&& _entity_id == other._entity_id;
		}

		bool operator!=(const entity_components_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		value()
		{
			return _pool_it->components[_entity_id];
		}

		const value_type&
		value() const
		{
			return  _pool_it->components[_entity_id];
		}
				
		value_type&
		operator*()
		{
			return  _pool_it->components[_entity_id];
		}

		const value_type&
		operator*() const
		{
			return  _pool_it->components[_entity_id];
		}

		const char*
		type() const
		{
			return _pool_it->utils->type;
		}
	*/
	};

	/*struct entity_components_view
	{
		using iterator = entity_components_iterator;
		cpprelude::dynamic_array<component_pool>& _pools;
		_id_type _entity_id;

		entity_components_view(cpprelude::dynamic_array<component_pool>& pools, _id_type entity_id)
			:_pools(pools), _entity_id(entity_id)
		{}

		iterator
		begin()
		{
			return iterator(_pools.begin(), _pools.count(), _entity_id);
		}
	
		iterator
		end()
		{
			return iterator(_pools.end(), 0, _entity_id);
		}

	};

	struct generic_component_view
	{
		using iterator = generic_component_iterator;
		cpprelude::dynamic_array<component_pool>& _pools;
		
		generic_component_view(cpprelude::dynamic_array<component_pool>& pools)
			:_pools(pools)
		{}

		iterator
		begin()
		{
			return iterator(_pools.begin(), _pools.count());
		}
		
		iterator
		end()
		{
			return iterator(_pools.end(), 0);
		}
	};
	*/

	template<typename T>
	struct component_view
	{
		using iterator = component_iterator<T>;

		component_pool& _pool;

		component_view(component_pool& pool)
			:_pool(pool)
		{}

		iterator
		begin()
		{
			return iterator(_pool, _pool.dense.begin());
		}

		iterator
		end()
		{
			return iterator(_pool, _pool.dense.end());
		}
	};

}

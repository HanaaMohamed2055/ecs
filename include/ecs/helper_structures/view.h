# pragma once
#include <cpprelude/iterator.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/elements.h>
#include <ecs/utility.h>
namespace ecs
{
	struct generic_component_iterator
	{
		using component_iterator = cpprelude::sequential_iterator<std::pair<Internal_Component, cpprelude::usize>>;
		
		using iterator_category = std::forward_iterator_tag;
		using value_type = Internal_Component;
		using difference_type = cpprelude::isize;
		using pointer = Internal_Component*;
		using reference = Internal_Component&;
		using data_type = Internal_Component;

		component_iterator _component_it;
		cpprelude::sequential_iterator<component_pool> _pool_it;
		cpprelude::usize _pool_count = 0;
		cpprelude::usize _component_count = 0; // per pool

		generic_component_iterator(cpprelude::sequential_iterator<component_pool> pool_it, cpprelude::usize pool_count)
			:_pool_it(pool_it), _pool_count(pool_count)
		{
			if (_pool_count)
			{
				--_pool_count;
				_component_count = _pool_it->components.count() - 1;
				_component_it = _pool_it->components.begin();
			}
			else
				_component_count = 0;
		}

		generic_component_iterator(const generic_component_iterator& other)
			:_pool_it(other._pool_it), _pool_count(other._pool_count)
		{
			if (_pool_count)
			{
				--_pool_count;
				_component_count = _pool_it->components.count() - 1;
				_component_it = _pool_it->components.begin();
			}
			else
				_component_count = 0;
		}

		generic_component_iterator&
		operator++()
		{
			if (_component_count)
			{
				++_component_it;
				--_component_count;
			}
			else if (_pool_count)
			{
				++_pool_it;
				_component_count = _pool_it->components.count() - 1;
				_component_it = _pool_it->components.begin();
				--_pool_count;
			}
			else if (!_pool_count)
				++_pool_it;
		
			return *this; 
		}

		generic_component_iterator&
		operator++(int)
		{
			auto result = *this;
			
			if (_component_count)
			{
				++_component_it;
				--_component_count;
			}
			else if (_pool_count)
			{
				++_pool_it;
				_component_count = _pool_it->components.count() - 1;
				_component_it = _pool_it->components.begin();
				--_pool_count;
			}
			else if (!_pool_count)
				++_pool_it;

			return result;
		}

		bool
		operator==(const generic_component_iterator& other) const
		{
			return _pool_it == other._pool_it
				&& _component_count == other._component_count;
		}

		bool
		operator!=(const generic_component_iterator& other) const
		{
			return !operator==(other);
		}

		ID
		entity()
		{
			return _component_it->first.entity_id;
		}

		value_type&
		value()
		{
			return _component_it->first;
		}

		const value_type&
		value() const
		{
			return _component_it->first;
		}
		
		value_type&
		operator*()
		{
			return _component_it->first;
		}

		const value_type&
		operator*() const
		{
			return _component_it->first;
		}

		const char*
		type() const
		{
			return _pool_it->utils->type;
		}
				
	};

	
	template<typename T>
	struct component_iterator
	{
		using component_set_iterator = cpprelude::sequential_iterator<std::pair<Internal_Component, cpprelude::usize>>;

		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = cpprelude::isize;
		using pointer = T*;
		using reference = T&;
		using data_type = T;

		component_set_iterator _component_it;
		
		component_iterator(const component_set_iterator& it)
			:_component_it(it)
		{}

		component_iterator(const component_iterator& other)
			:_component_it(other._component_it)
		{}

		component_iterator&
		operator++()
		{
			++_component_it;
			return *this;
		}

		component_iterator&
		operator++(int)
		{
			auto result = *this;
			++_component_it;

			return result;
		}

		bool operator==(const component_iterator& other) const
		{
			return _component_it == other._component_it;
		}

		bool operator!=(const component_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		data()
		{
			auto component = _component_it->first;
			return *(static_cast<value_type*>(component.data));
		}

		const value_type&
		data() const
		{
			auto component = _component_it->first;
			return *(static_cast<value_type*>(component.data));
		}

		Internal_Component&
		value()
		{
			return _component_it->first;
		}

		ID
		entity()
		{
			return _component_it->first.entity_id;
		}

		std::pair<ID, value_type&>
		operator*()
		{
			auto entity = _component_it->first.entity_id;
			auto component = _component_it->first;
			auto& data = *(static_cast<value_type*>(component.data));
			return std::pair<ID, value_type&>(entity, data);
		}

		std::pair<ID, const value_type&>
		operator*() const
		{
			auto entity = _component_it->first.entity_id;
			auto component = _component_it->first;
			auto& data = *(static_cast<value_type*>(component.data));
			return std::pair<ID, const value_type&>(entity, data);
		}
	};

	struct entity_components_iterator
	{
		using component_iterator = cpprelude::sequential_iterator<std::pair<Internal_Component, cpprelude::usize>>;

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
	};

	struct entity_components_view
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
			return iterator(_pool.components.begin());
		}

		iterator
		end()
		{
			return iterator(_pool.components.end());
		}
	};

}

# pragma once
#include <cpprelude/dynamic_array.h>

#include <ecs/elements.h>
#include <ecs/utility.h>
namespace ecs
{

	struct component_pool
	{
		utility::base_type_utils* utils = nullptr;
		cpprelude::memory_context* _context = nullptr;

		cpprelude::dynamic_array<internal_component> components;
		cpprelude::dynamic_array<bool> managed;
		cpprelude::dynamic_array<cpprelude::usize> sparse;

		component_pool(cpprelude::memory_context* context)
			:_context(context),
			components(context),
			sparse(context)
		{}

		component_pool()
			:_context(nullptr)
		{}

		void
		insert_at(cpprelude::usize entity_index, void* data_ptr, bool allocated_by_pool = true)
		{
			if (sparse.capacity() <= entity_index)
			{
				sparse.expand_back(2 * (entity_index + 1), INVALID_PLACE);
				managed.expand_back(2 * (entity_index + 1), false);
				components.reserve(2 * (entity_index + 1));
			}
			sparse[entity_index] = components.count();
			components.insert_back({ data_ptr, entity_index });
			managed[entity_index] = allocated_by_pool;
		}

		void
		remove(cpprelude::usize entity_index)
		{
			if (entity_index < sparse.count())
			{
				cpprelude::usize dense_index = sparse[entity_index];
				
				if (managed[entity_index])
				{
					utils->free(components[dense_index].data, _context);
					managed[entity_index] = false;
				}
				components[dense_index].data = nullptr;
				std::swap(components[dense_index], components[components.count() - 1]);
				sparse[components[dense_index].entity_index] = dense_index;
				sparse[entity_index] = INVALID_PLACE;
				components.remove_back();
			}
		}

		bool
		has(cpprelude::usize entity_index) const
		{
			return entity_index < sparse.count()
				&& sparse[entity_index] != INVALID_PLACE;
		}

		internal_component&
		operator[](cpprelude::usize entity_index)
		{
			return components[sparse[entity_index]];
		}

		const internal_component&
		operator[](cpprelude::usize entity_index) const
		{
			return components[sparse[entity_index]];
		}

		internal_component&
		get(cpprelude::usize entity_index)
		{
			return components[sparse[entity_index]];
		}

		const internal_component&
		get(cpprelude::usize entity_index) const
		{
			return components[sparse[entity_index]];
		}
	};

	struct generic_component_iterator
	{
		using component_iterator = cpprelude::sequential_iterator<internal_component>;
		
		using iterator_category = std::forward_iterator_tag;
		using difference_type = cpprelude::isize;
		using value_type = void*;

		cpprelude::sequential_iterator<component_pool> _pool_it;
		cpprelude::usize _pool_count = 0;
		component_iterator _component_it;
		component_iterator _current_pool_end;

		generic_component_iterator(cpprelude::sequential_iterator<component_pool> pool_it, 
									component_iterator component_it, component_iterator end, cpprelude::usize pool_count)
			:_pool_it(pool_it), _component_it(component_it), _current_pool_end(end), _pool_count(pool_count)
		{
			if (_pool_count)
				--_pool_count;
			
			while (_pool_count && _component_it == _current_pool_end)
			{
				++_pool_it;
				_current_pool_end = _pool_it->components.end();
				_component_it = _pool_it->components.begin();
				--_pool_count;
				
				if (_component_it != _current_pool_end)
					break;
				
				if (!_pool_count && _component_it == _current_pool_end)
					++_pool_it;
			}
		}

		generic_component_iterator(const generic_component_iterator& other)
			:_pool_it(other._pool_it), _pool_count(other._pool_count)
		{
			if (_pool_count)
				--_pool_count;

			while (_pool_count && _component_it == _current_pool_end)
			{
				++_pool_it;
				_current_pool_end = _pool_it->components.end();
				_component_it = _pool_it->components.begin();
				--_pool_count;
				
				if (_component_it != _current_pool_end)
					break;
				
				if (!_pool_count && _component_it == _current_pool_end)
					++_pool_it;
			}
		}

		generic_component_iterator&
		operator++()
		{
			++_component_it;

			if (_component_it == _current_pool_end)
			{
				while (_pool_count)
				{
					++_pool_it;
					_current_pool_end = _pool_it->components.end();
					_component_it = _pool_it->components.begin();
					--_pool_count;
					if (_component_it != _current_pool_end)
						break;
				}
			}

			if (!_pool_count && _component_it == _current_pool_end)
				++_pool_it;
			
			return *this;
		}

		generic_component_iterator&
		operator++(int)
		{
			auto result = *this;

			++_component_it;

			if (_component_it == _current_pool_end)
			{
				while (_pool_count)
				{
					++_pool_it;
					_current_pool_end = _pool_it->components.end();
					_component_it = _pool_it->components.begin();
					--_pool_count;
					if (_component_it != _current_pool_end)
						break;
				}
			}

			if (!_pool_count && _component_it == _current_pool_end)
				++_pool_it;
		
			return result;
		}

		bool
		operator==(const generic_component_iterator& other) const
		{
			return _pool_count ? _component_it == other._component_it
								: _pool_it == other._pool_it;
		}

		bool
		operator!=(const generic_component_iterator& other) const
		{
			return !operator==(other);
		}

		cpprelude::usize
		entity()
		{
			return _component_it->entity_index;
		}

		value_type&
		value()
		{
			return  _component_it->data;
		}

		const value_type&
		value() const
		{
			return  _component_it->data;
		}
		
		const char*
		type() const
		{
			return _pool_it->utils->type;
		}

		generic_component
		operator*()
		{
			return generic_component(_component_it->data,
				                     _component_it->entity_index,
									 _pool_it->utils->type);
		}

		const generic_component
		operator*() const
		{
			return generic_component(_component_it->data,
					 				 _component_it->entity_index,
									_pool_it->utils->type);
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

		cpprelude::sequential_iterator<internal_component> _component_it;
		

		component_iterator(const cpprelude::sequential_iterator<internal_component>& component_it)
			:_component_it(component_it)
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

		bool operator!=(const component_iterator& other)
		{
			return !operator==(other);
		}

		value_type&
		value()
		{
			return *((value_type*)(_component_it->data));
		}

		const value_type&
		value() const
		{
			return *((value_type*)(_component_it->data));
		}

		cpprelude::usize
		entity()
		{
			return _component_it->entity_index;
		}

		Component<T>
		operator*()
		{
			//T* data = static_cast<value_type*>(_pool.components[*_dense_it]);
			return *_component_it;
		}

		Component<const T>
		operator*() const
		{
			//T* data = static_cast<value_type*>(_pool.components[*_dense_it]);
			return *_component_it;
		}

		value_type*
		operator->()
		{
			return ((value_type*)(_component_it->data));
		}
	};

	struct entity_components_iterator
	{
		using pool_iterator = cpprelude::sequential_iterator<component_pool>;
		
		using iterator_category = std::forward_iterator_tag;
		using value_type = internal_component;
		using difference_type = cpprelude::isize;
		using pointer = internal_component*;
		using reference = internal_component&;
		using data_type = internal_component;

		pool_iterator _pool_it;
		pool_iterator _end_it;
		cpprelude::usize _entity_id;
		
		entity_components_iterator(pool_iterator pool_it, pool_iterator end_it, cpprelude::usize entity_id)
			:_pool_it(pool_it), _end_it(end_it), _entity_id(entity_id)
		{
			while (_pool_it != _end_it && !_pool_it->has(_entity_id))
				++_pool_it;
		}

		entity_components_iterator(const entity_components_iterator& other)
			:_pool_it(other._pool_it), _end_it(other._end_it), _entity_id(other._entity_id)
		{
			
			while (_pool_it != _end_it && !_pool_it->has(_entity_id))
				++_pool_it;
		}

		entity_components_iterator&
		operator++()
		{
			++_pool_it;

			while (_pool_it != _end_it && !_pool_it->has(_entity_id))
				++_pool_it;

			return *this;
		}

		entity_components_iterator&
		operator++(int)
		{
			auto result = *this;

			++_pool_it;

			while (_pool_it != _end_it && !_pool_it->has(_entity_id))
				++_pool_it;

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

		void*
		value()
		{
			return _pool_it->get(_entity_id).data;
		}

		const void*
		value() const
		{
			return  _pool_it->get(_entity_id).data;
		}
				
		entity_component
		operator*()
		{
			return entity_component(_pool_it->get(_entity_id).data, _pool_it->utils->type);
		}

		const entity_component&
		operator*() const
		{
			return entity_component(_pool_it->get(_entity_id).data, _pool_it->utils->type);
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
		cpprelude::dynamic_array<component_pool>* _pools = nullptr;
		cpprelude::usize _entity_id;

		entity_components_view(cpprelude::dynamic_array<component_pool>* pools, cpprelude::usize entity_id)
			:_pools(pools), _entity_id(entity_id)
		{}

		iterator
		begin()
		{
			return iterator(_pools->begin(), _pools->end(), _entity_id);
		}
	
		iterator
		end()
		{
			return iterator(_pools->end(), _pools->end(), _entity_id);
		}

	};

	struct generic_component_view
	{
		using iterator = generic_component_iterator;

		using iterator_category = std::forward_iterator_tag;
		using value_type = internal_component;
		using difference_type = cpprelude::isize;
		using pointer = internal_component*;
		using reference = internal_component&;
		using data_type = internal_component;

		cpprelude::dynamic_array<component_pool>* _pools;
		
		generic_component_view(cpprelude::dynamic_array<component_pool>* pools = nullptr)
			:_pools(pools)
		{}

		generic_component_view
		operator=(const generic_component_view& other)
		{
			_pools = other._pools;
			return *this;
		}

		iterator
		begin()
		{
			if (_pools && _pools->empty())
				return iterator(_pools->end(), nullptr, nullptr, 0);
			
			return iterator(_pools->begin(),
							_pools->begin()->components.begin(),
							_pools->begin()->components.end(),
							_pools->count());

		}
		
		iterator
		end()
		{
			return iterator(_pools->end(), nullptr, nullptr, 0);
		}
	};
	
	template<typename T>
	struct component_view
	{
		using iterator = component_iterator<T>;

		component_pool* _pool;

		component_view(component_pool* pool = nullptr)
			:_pool(pool)
		{}

		component_view
		operator=(const component_pool& other)
		{
			_pool = other._pool;
			return *this;
		}

		iterator
		begin()
		{
			return iterator(_pool->components.begin());
		}

		iterator
		end()
		{
			return iterator(_pool->components.end());
		}
	};

}

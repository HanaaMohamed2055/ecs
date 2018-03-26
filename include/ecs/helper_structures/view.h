# pragma once
#include <ecs/helper_structures/component_pool.h>

namespace ecs
{
	struct generic_component_iterator
	{
		using component_iterator = cpprelude::sequential_iterator<internal_component>;
		
		using iterator_category = std::forward_iterator_tag;
		using difference_type = cpprelude::isize;
		using value_type = void*;

		cpprelude::sequential_iterator<component_pool> _pool_it = nullptr;
		cpprelude::usize _pool_count = 0;
		component_iterator _component_it = nullptr;
		component_iterator _current_pool_end = nullptr;


		generic_component_iterator(cpprelude::sequential_iterator<component_pool> pool_it, cpprelude::usize pool_count)
			:_pool_it(pool_it), _pool_count(pool_count)
		{
			if (_pool_count)
			{
				_component_it = _pool_it->components.begin();
				_current_pool_end = _pool_it->components.end();
				--_pool_count;
			}
							
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
			:_pool_it(other._pool_it),
			_pool_count(other._pool_count),
			_component_it(other._component_it),
			_current_pool_end(other._current_pool_end)
		{}

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

		cpprelude::sequential_iterator<internal_component> _component_it = nullptr;

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

		pool_iterator _pool_it = nullptr;
		pool_iterator _end_it = nullptr;
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
			
	template<typename iterator>
	struct view
	{
		iterator _begin;
		iterator _end;

		view(iterator begin, iterator end)
			:_begin(begin), _end(end)
		{}
		
		iterator
		begin()
		{
			return _begin;
		}

		iterator
		end()
		{
			return _end;
		}
	};

}

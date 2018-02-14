# pragma once

#include <cpprelude/defines.h>

#include <ecs/elements.h>
#include <ecs/bag.h>

namespace ecs
{
	template<typename T>
	struct type_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = cpprelude::isize;
		using pointer = T*;
		using reference = T&;
		using data_type = T;

		cpprelude::string _type;
		cpprelude::usize _size = 0;
		bag_iterator<Internal_Component> _element_it;
	
		type_iterator(const bag_iterator<Internal_Component>& bag_itr, cpprelude::usize size, cpprelude::memory_context* context = platform->global_memory)
			:_element_it(bag_itr), _size(size)
		{
			_type = cpprelude::string(typeid(T).name(), context);
			
			while (_size > 0 && _element_it->type != _type)
			{
				++_element_it;
				--_size;
			}
		}

		type_iterator(const type_iterator& other, cpprelude::memory_context* context = platform->global_memory)
			:_element_it(other._element_it), _size(other._size)
		{
			_type = cpprelude::string(typeid(T).name(), context);
		}

		type_iterator&
		operator++()
		{
			++_element_it;
			--_size;

			while (_size > 0 && _element_it->type != _type)
			{
				++_element_it;
				--_size;
			}

			return *this;
		}		

		type_iterator&
		operator++(int)
		{
			auto result = *this;
			++_element_it;
			--_size;

			while (_size > 0 && _element_it->type != _type)
			{
				++_element_it;
				--_size;
			}

			return result;
		}

		bool operator==(const type_iterator& other) const
		{
			return _element_it == other._element_it;
		}

		bool operator!=(const type_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		data()
		{
			return *(static_cast<T*>(_element_it->_data));
		}

		const value_type&
		data() const
		{
			return *(static_cast<T*>(_element_it->_data))
		}

		Internal_Component&
		value()
		{
			return *_element_it;
		}

		Internal_Component*
		operator->()
		{
			return _element_it;
		}

		Internal_Component&
		operator*()
		{
			return *_element_it;
		}
	};

	// very basic implementation, will be refined later
	template<typename T>
	struct  entity_type_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = cpprelude::isize;
		using pointer = T*;
		using reference = T&;
		using data_type = T;

		cpprelude::string _type;
		cpprelude::usize _size = 0;
		cpprelude::u64 entity_id;
		bag_iterator<Internal_Component> _element_it;
		

		entity_type_iterator(const bag_iterator<Internal_Component>& bag_itr, cpprelude::usize size, cpprelude::u64 id, cpprelude::memory_context* context = platform->global_memory)
			:_element_it(bag_itr), _size(size), entity_id(id)
		{
			_type = cpprelude::string(typeid(T).name(), context);

			while(_size > 0 && (_element_it->type != _type || _element_it->entity_id != entity_id))
			{
				++_element_it;
				--_size;
			}
		}

		entity_type_iterator&
		operator++()
		{
			++_element_it;
			--_size;

			while(_size > 0 && (_element_it->type != _type || _element_it->entity_id != entity_id))
			{
				++_element_it;
				--_size;
			}

			return *this;
		}

		entity_type_iterator&
		operator++(int)
		{
			auto result = *this;
			++_element_it;
			--_size;

			while(_size > 0 && (_element_it->type != _type || _element_it->entity_id != entity_id))
			{
				++_element_it;
				--_size;
			}

			return result;
		}

		bool operator==(const entity_type_iterator& other) const
		{
			return _element_it == other._element_it;
		}

		bool operator!=(const entity_type_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		data()
		{
			return *(static_cast<T*>(_element_it->_data));
		}

		const value_type&
		data() const
		{
			return *(static_cast<T*>(_element_it->_data))
		}

		Internal_Component&
		value()
		{
			return *_element_it;
		}

		Internal_Component*
		operator->()
		{
			return _element_it;
		}		
		
		Internal_Component&
		operator*()
		{
			return *_element_it;
		}
	};

	template<typename iterator>
	struct component_view
	{
		
	};
}
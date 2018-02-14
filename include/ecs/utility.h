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
	
		type_iterator(const bag_iterator<Internal_Component>& other, cpprelude::usize size, cpprelude::memory_context* context = platform->global_memory)
			:_element_it(other), _size(size)
		{
			_type = cpprelude::string(typeid(T).name(), context);
			
			while (_size > 0 && _element_it->type != _type)
			{
				++_element_it;
				--_size;
			}
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

		value_type&
		operator*()
		{
			return *_element_it;
		}
	};

	template<typename T, typename cpprelude::u64 entity_id>
	struct  entity_type_iterator
	{
		
	};

	template<typename iterator>
	struct component_view
	{
		
	};
}
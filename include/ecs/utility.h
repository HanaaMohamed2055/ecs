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

		cpprelude::string _type = typeid(T).name(); 
		bag_iterator<Internal_Component> _element_it;
		bag_iterator<Internal_Component> _limit;

		type_iterator(const bag_iterator<Internal_Component>& other, const bag_iterator<Internal_Component>& end)
			:_element_it(other), _limit(end)
		{
			while(_element_it != _limit && _element_it->type != _type)
				++_element_it;
		}

		type_iterator&
		operator++()
		{
			++_element_it;

			while(_element_it != _limit && _element_it->type != _type)
				++_element_it;	

			return *this;
		}		

		type_iterator&
		operator++(int)
		{
			auto result = *this;
			++_element_it;

			while(_element_it != _limit && _element_it->type != _type)
				++_element_it;

			return result;
		}

		bool operator==(const type_iterator& other) const
		{
			return _element_it == other;
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
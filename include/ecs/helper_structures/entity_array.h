#include <cpprelude/defines.h>
#include <cpprelude/dynamic_array.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>

#include <ecs/elements.h>

namespace ecs
{
	struct entity_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = ID;
		using difference_type = cpprelude::isize;
		using pointer = ID*;
		using reference = ID&;
		using data_type = ID;

		cpprelude::sequential_iterator<ID> _entity_it;
		cpprelude::usize _recycle_count;
		cpprelude::usize _index = 0;
		cpprelude::usize _count = 0;

		entity_iterator(cpprelude::sequential_iterator<ID> entity_it, cpprelude::usize recycle_count, cpprelude::usize count)
			:_entity_it(entity_it), _recycle_count(recycle_count), _count(count)
		{
			while (_count && _entity_it->id() != _index || (_entity_it->id() == _index && _recycle_count == 1))
			{
				++_index;
				--_count;
				++_entity_it;
			}
		}

		entity_iterator&
		operator++()
		{
			++_entity_it;
			++_index;
			--_count;

			while (_count && _entity_it->id() != _index || (_entity_it->id() == _index && _recycle_count == 1))
			{
				++_index;
				--_count;
				++_entity_it;
			}

			return *this;
		}

		entity_iterator&
		operator++(int)
		{
			auto result = *this;
			++_entity_it;
			++_index;

			while (_count && _entity_it->id() != _index || (_entity_it->id() == _index && _recycle_count == 1))
			{
				++_index;
				++_entity_it;
			}

			return result;
		}

		bool
		operator==(const entity_iterator& other) const
		{
			return _entity_it == other._entity_it;
		}

		bool
			operator!=(const entity_iterator& other) const
		{
			return !operator==(other);
		}

		value_type&
		value()
		{
			return *_entity_it;
		}

		const value_type&
		value() const
		{
			return *_entity_it;
		}

		value_type&
		operator*()
		{
			return *_entity_it;
		}

		const value_type&
		operator*() const
		{
			return *_entity_it;
		}

		value_type*
		operator->()
		{
			return _entity_it;
		}
	};
	
	struct entity_array
	{
		using iterator = entity_iterator;

		cpprelude::dynamic_array <ID> _entity_index;
		ID next;
		cpprelude::usize recycle_count = 0;

		entity_array(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_entity_index(context)
		{}

		ID
		insert()
		{
			if (recycle_count)
			{
				auto reused = next;
				auto temp = _entity_index[next.id()];
				_entity_index[next.id()] = next;
				next = temp;
				--recycle_count;
				return reused;
			}
			else
			{
				_entity_index.insert_back(_entity_index.count());
				return _entity_index.count() - 1;
			}
		}

		bool
		has(const ID entity) const
		{
			return entity.id() < _entity_index.count() 
				&& entity.number == _entity_index[entity.id()].number;
		}

		bool
		has(cpprelude::usize index)
		{
			return index < _entity_index.count()
				&& index == _entity_index[index].id();
		}

		void
		remove(cpprelude::usize index)
		{
			_entity_index[index].increment_version();
			std::swap(next, _entity_index[index]);
			++recycle_count;
		}

		ID&
		operator[](cpprelude::usize index)
		{
			return _entity_index[index];
		}

		const ID&
		operator[](cpprelude::usize index) const
		{
			return _entity_index[index];
		}

		cpprelude::usize
		count() const
		{
			return _entity_index.count();
		}

		bool
		empty()
		{
			return _entity_index.count() == 0;
		}

		cpprelude::usize
		capacity()
		{
			return _entity_index.capacity();
		}

		void
		reserve(cpprelude::usize expected_count)
		{
			_entity_index.reserve(expected_count);
		}

		iterator
		begin()
		{
			return iterator(_entity_index.begin(), recycle_count, _entity_index.count());
		}

		iterator
		end()
		{
			return iterator(_entity_index.end(), recycle_count, 0);
		}
	};

}
# pragma once

#include "cpprelude/defines.h"
#include "cpprelude/dynamic_array.h"
#include "cpprelude/queue_array.h"

using namespace cpprelude;

namespace ecs
{
	template<typename T>
	struct bag
	{
		dynamic_array<T> index;
		queue_array<usize> free_places;

		T* add(T&& element)
		{
			if (!free_places.empty())
			{
				auto place = free_places.front();
				free_places.dequeue();
				index[place] = element;
				return &index[place];
			}
			
			index.emplace_back(element);
			return &index[index.count() - 1];
		}

		void remove(T* element)
		{
			auto diff = element - &index[0];
			free_places.enqueue((usize)diff);
		}

		usize get_size()
		{
			return index.count();
		}
	};
}


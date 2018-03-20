#include <cpprelude/defines.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/elements.h>

namespace ecs
{
	struct entity_array
	{
		cpprelude::dynamic_array<ID> entity_index;
	};
}
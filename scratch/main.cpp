#include "benchmark.h"

#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/world.h>

int main()
{
	ecs::World w;

	auto e = w.create_entity();

	u32 data = 2302;
	w.add_property<u32>(e, data);

}

#include "benchmark.h"

#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/world.h>

int main()
{
	//benchmark();
	
	ecs::World world;
	auto entity = world.create_entity();
	world.add_property<u32>(entity, (u32)392);
	world.add_property<r32>(entity, (r32)823.28);
	world.add_property<r64>(entity, (r64)1029.8888888);

	std::cout << world.has<u32>(entity) << std::endl;
	std::cout << world.has<r32>(entity) << std::endl;
	std::cout << world.has<char>(entity) << std::endl;
}
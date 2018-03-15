#include "benchmark.h"

#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/world.h>

int main()
{
	benchmark();
	
	/*ecs::World world;
	auto e1 = world.create_entity();
	auto e2 = world.create_entity();
	
	world.add_property<u32>(e1, (u32)392);
	world.add_property<r32>(e1, (r32)823.28);
	world.add_property<r64>(e1, (r64)1029.8888888);
	
	world.add_property<u32>(e2, (u32)300);
	world.add_property<r32>(e2, (r32)8.28);
	world.add_property<r64>(e2, (r64)500.4739);

	std::cout << world.has<u32>(e1) << std::endl;
	std::cout << world.has<r32>(e1) << std::endl;
	std::cout << world.has<char>(e1) << std::endl;
	
	world.remove_property<u32>(e1);
	std::cout << world.has<u32>(e1) << std::endl;
	std::cout << world.has<r32>(e1) << std::endl;
	std::cout << world.has<char>(e1) << std::endl;
	
	world.kill_entity(e1);
	world.kill_entity(e2);*/
}
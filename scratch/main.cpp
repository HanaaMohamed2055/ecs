#include "benchmark.h"
#include <tuple>

#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/view.h>
#include <ecs/world.h>

/////////////////////////////////////////////////////
// the above will be placed in the details namespace






int main()
{

	ecs::World world;
	auto view = world.get_all_world_components();

	auto e1 = world.create_entity();
	auto e2 = world.create_entity();

	//auto entity_view = world.get_all_entity_properties(e1);
	auto& pool = world.get_pool<r32>();

	world.add_property<u32>(e1, (u32)392);
	world.add_property<r32>(e1, (r32)823.28);

	world.add_property<r64>(e1, (r64)1029.8888888);

	world.add_property<u32>(e2, (u32)300);
	world.add_property<r32>(e2, (r32)8.28);
	world.add_property<r64>(e2, (r64)500.4739);
	
	auto& entities = world.get_all_world_entities();
	for (auto e : entities)
	std::cout << e.id() << " " << e.version() << std::endl;
			
	auto type_view = world.get_world_components<r32>();
	for (auto it = type_view.begin(); it != type_view.end(); ++it)
	{
		std::cout << it.entity() << std::endl;
		std::cout << it.value() << std::endl;
		std::cout << world.get<u32>(it.entity()) << std::endl;
	}
	for (auto c : type_view)
	{
		std::cout << *c << std::endl;
		std::cout << world.get_related_component<u32>(c) << std::endl;
	}

	/*world.kill_entity(e1);
	world.kill_entity(e2);*/

	std::cout << std::endl;
	auto e = world.create_entity();
	world.add_property<u32>(e, 56);
	world.add_property<r32>(e, 1.45);
	auto u32_view = world.get_world_components<u32>();
	for (auto c : u32_view)
		std::cout << *c << std::endl;

	std::cout << "------------------------------------------\n";
	for (auto c : view)
	{
		std::cout << c.entity_id << std::endl;
		std::cout << c.type << std::endl;
	}
	
	auto entity_properties = world.get_all_entity_properties(e1);
	for (auto w : entity_properties)
		std::cout << w.type << std::endl;

	if (world.has_all<u32, r32, r64>(e1))
		std::cout << "Yes!\n";

	if (!world.has_all<char, r32, r64>(e1))
		std::cout << "No!\n";

}
//benchmark();


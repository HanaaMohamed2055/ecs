#include "benchmark.h"
#include <tuple>

#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/view.h>
#include <ecs/world.h>


int main()
{
	//benchmark();
	ecs::World world;
	auto view = world.get_all_world_components();

	auto e1 = world.create_entity();
	auto e2 = world.create_entity();
	
	auto entity_view = world.get_all_entity_properties(e1);
	ecs::sparse_unordered_set<ecs::Internal_Component>& components = world.component_pools[utility::get_type_identifier<r32>()].components;
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

	for (auto component : view)
	{
		std::cout << component.entity_id.id() << std::endl;
		std::cout << component.data << std::endl;
	}
	auto type_view = world.get_world_components<r32>();
	for (auto it = type_view.begin(); it != type_view.end(); ++it)
	{
		std::cout << it.entity().id() << std::endl;
		std::cout << it.data() << std::endl;
		std::cout << world.get<u32>(it.entity()) << std::endl;
	}

	for (auto p : type_view)
	{
		std::cout << p.first.id() << " " << p.second << std::endl;
		std::cout << world.get<u32>(p.first) << std::endl;
	}


	world.kill_entity(e1);
	world.kill_entity(e2);

	std::cout << std::endl;
	auto e = world.create_entity();
	world.add_property<u32>(e, 56);
	world.add_property<r32>(e, 1.45);
	auto u32_view = world.get_world_components<u32>();
	for (auto p : u32_view)
	{
		std::cout << world.get<r32>(p.first) << std::endl;
	}
	
}
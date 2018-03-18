#include "benchmark.h"

#include <cpprelude/dynamic_array.h>

#include <ecs/helper_structures/view.h>
#include <ecs/world.h>

int main()
{
	//benchmark();
	
	ecs::World world;
	auto view = world.get_all_world_components();
	auto type_view = world.get_world_components<r32>();
	
	auto e1 = world.create_entity();
	auto e2 = world.create_entity();

	auto entity_view = world.get_all_entity_properties(e1);

	world.add_property<u32>(e1, (u32)392);
	world.add_property<r32>(e1, (r32)823.28);
	world.add_property<r64>(e1, (r64)1029.8888888);
	
	world.add_property<u32>(e2, (u32)300);
	world.add_property<r32>(e2, (r32)8.28);
	world.add_property<r64>(e2, (r64)500.4739);
	
	auto& entities = world.get_all_world_entities();
	//for (auto e : entities)
	//	std::cout << e.id() << " " << e.version() << std::endl;
	//	
	//for (auto component : view)
	//{
	//	std::cout << component.entity_id << std::endl;
	//	std::cout << component.data << std::endl;
	//}

	//for (auto it = type_view.begin(); it != type_view.end(); ++it)
	//{
	//	std::cout << it.entity() << std::endl;
	//	std::cout << it.data() << std::endl;
	//}

	//for (auto component : type_view)
	//{
	//	std::cout << component << std::endl;
	//}

	std::cout << "---------------------------------\n";
	std::cout << world.type_exists<u32>() << std::endl;
	std::cout << world.type_exists<r32>() << std::endl;
	std::cout << world.type_exists<char>() << std::endl;
	std::cout << "---------------------------------\n";

	/*world.kill_entity(e1);
	world.kill_entity(e2);*/
	
	world.add_property<u32>(world.create_entity());
}
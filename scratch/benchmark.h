# pragma once

#include <iostream>

#include <cpprelude/micro_benchmark.h>
#include <cpprelude/dynamic_array.h>

#include <entt/entity/registry.hpp>

#include <ecs/world.h>

using namespace cpprelude;

// a dummy struct just to test adding components

struct Position
{
	r32 x;
	r32 y;
	
	Position()
	{}

	Position(r32 i, r32 j)
		:x(i), y(j)
	{}
};

// creating and destroying component-less entities
void 
create_ecs_entities(workbench* bench, usize limit)
{
	ecs::World world;
	bench->watch.start();

	for (usize i = 0; i < limit; ++i)
		world.create_entity();
	
	bench->watch.stop();
}

void
create_entt_entities(workbench* bench, usize limit)
{
	entt::Registry<uint64_t> registry;
	bench->watch.start();

	for (usize i = 0; i < limit; ++i)
		registry.create();

	bench->watch.stop();
}
//
//
//void
//destroy_ecs_entities(workbench* bench, usize limit)
//{
//	ecs::World world;
//	dynamic_array<ecs::Entity> entities;
//	bench->watch.start();
//
//	for (usize i = 0; i < limit; ++i)
//		entities.insert_back(world.create_entity());
//
//	for (auto entity: entities)
//		world.kill_entity(entity);
//
//	bench->watch.stop();
//}
//
//void
//destroy_entt_entities(workbench* bench, usize limit)
//{
//	entt::DefaultRegistry registry;
//	dynamic_array<entt::DefaultRegistry::entity_type> entities;
//	bench->watch.start();
//
//	for (usize i = 0; i < limit; ++i)
//		entities.insert_back(registry.create());
//
//	for (auto entity : entities)
//		registry.destroy(entity);
//
//	bench->watch.stop();
//}
//
// creating, iterating and destroying small entities (entities with only one component)
void
create_ecs_small_entities(workbench* bench, usize limit)
{
	ecs::World world;
	bench->watch.start();
	Position p;

	//for (usize i = 0; i < limit; ++i)
	//	world.add_property<Position>(world.create_entity(), p);
	
	bench->watch.stop();
}

void
create_entt_small_entities(workbench* bench, usize limit)
{
	entt::DefaultRegistry registry;
	bench->watch.start();

	for (usize i = 0; i < limit; ++i)
		registry.create<Position>();

	if (registry.has<Position>(1))
		std::cout << "stop here\n";

	bench->watch.stop();
}
//
//void
//iterate_ecs_small_entities(workbench* bench, usize limit)
//{
//	ecs::World world;
//	bench->watch.start();
//
//	for (usize i = 0; i < limit; ++i)
//		world.add_property<Position>(world.create_entity());
//
//	auto view = world.get_world_components<Position>();
//
//	for(auto p: view)
//	{}
//
//	bench->watch.stop();
//}
//
//void
//iterate_entt_small_entities(workbench* bench, usize limit)
//{
//	entt::DefaultRegistry registry;
//	bench->watch.start();
//
//	auto view = registry.view<Position>();
//
//	for (usize i = 0; i < limit; ++i)
//		registry.create<Position>();
//
//	for (auto e : view)
//	{}
//
//	bench->watch.stop();
//}
//
//void
//destroy_ecs_small_entities(workbench* bench, usize limit)
//{
//	ecs::World world;
//	bench->watch.start();
//
//	for (usize i = 0; i < limit; ++i)
//		world.add_property<Position>(world.create_entity());
//
//	auto view = world.get_all_world_entities();
//
//	for (auto entity: view)
//	{
//		world.kill_entity(entity);
//	}
//
//	bench->watch.stop();
//}
//
//void
//destroy_entt_small_entities(workbench* bench, usize limit)
//{
//	entt::DefaultRegistry registry;
//	bench->watch.start();
//
//	auto view = registry.view<Position>();
//
//	for (usize i = 0; i < limit; ++i)
//		registry.create<Position>();
//
//	for (auto entity : view)
//	{
//		registry.destroy(entity);
//	}
//
//	bench->watch.stop();
//}
//
void
benchmark()
{
	usize limit = 10000000;

	compare_benchmark(std::cout, {
	CPPRELUDE_BENCHMARK(create_entt_entities, limit),
	CPPRELUDE_BENCHMARK(create_ecs_entities, limit)
	});
	
	//compare_benchmark(std::cout, {
	//CPPRELUDE_BENCHMARK(destroy_entt_entities, limit),
	//CPPRELUDE_BENCHMARK(destroy_ecs_entities, limit)
	//});
//
	//compare_benchmark(std::cout, {
	//CPPRELUDE_BENCHMARK(create_entt_small_entities, limit),
	//CPPRELUDE_BENCHMARK(create_ecs_small_entities, limit)
	//});
//
//	compare_benchmark(std::cout, {
//	CPPRELUDE_BENCHMARK(iterate_entt_small_entities, limit),
//	CPPRELUDE_BENCHMARK(iterate_ecs_small_entities, limit)
//	});
//
//	compare_benchmark(std::cout, {
//	CPPRELUDE_BENCHMARK(destroy_entt_small_entities, limit),
//	CPPRELUDE_BENCHMARK(destroy_ecs_small_entities, limit)
//	});
}
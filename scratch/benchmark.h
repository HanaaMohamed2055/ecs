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
void
destroy_ecs_entities(workbench* bench, ecs::World* w, cpprelude::dynamic_array<ecs::Entity> entities)
{
	//ecs::World world;
	//dynamic_array<ecs::Entity> entities;
	bench->watch.start();

	/*for (usize i = 0; i < limit; ++i)
		entities.insert_back(world.create_entity());*/

	for (auto entity: entities)
		w->kill_entity(entity);

	bench->watch.stop();
}
//
void
destroy_entt_entities(workbench* bench, entt::DefaultRegistry* registry, cpprelude::dynamic_array<entt::DefaultRegistry::entity_type> entities)
{
	//entt::DefaultRegistry registry;
	//dynamic_array<entt::DefaultRegistry::entity_type> entities;
	bench->watch.start();

	/*for (usize i = 0; i < limit; ++i)
		entities.insert_back(registry.create());*/

	for (auto entity: entities)
		registry->destroy(entity);

	bench->watch.stop();
}
//
// creating, iterating and destroying small entities (entities with only one component)
void
create_ecs_small_entities(workbench* bench, ecs::World* w, cpprelude::dynamic_array<ecs::Entity> entities)
{
	bench->watch.start();

	for (auto e: entities)
		w->add_property<Position>(e, Position());
	
	bench->watch.stop();
}

void
create_entt_small_entities(workbench* bench, entt::DefaultRegistry* registry, cpprelude::dynamic_array<entt::DefaultRegistry::entity_type> entities)
{
	bench->watch.start();
	
	for (auto e: entities)
		registry->assign<Position>(e, Position());

	bench->watch.stop();
}

void
iterate_ecs_small_entities(workbench* bench, ecs::World* w)
{
	bench->watch.start();

	auto view = w->get_world_components<Position>();
		
	for(auto p: view)
	{}

	bench->watch.stop();
}

void
iterate_entt_small_entities(workbench* bench, entt::DefaultRegistry* registry)
{
	bench->watch.start();

	auto view = registry->view<Position>();

	for (auto e : view)
	{}

	bench->watch.stop();
}

void
destroy_ecs_small_entities(workbench* bench, ecs::World* w)
{
	//bench->watch.start();

	//auto view = w->get_all_world_entities();

	//for (auto entity: view)
	//{
	//	w->kill_entity(entity);
	//}

	//bench->watch.stop();
}

void
destroy_entt_small_entities(workbench* bench, entt::DefaultRegistry* registry)
{
	bench->watch.start();

	auto view = registry->view<Position>();
	
	for (auto entity : view)
	{
		registry->destroy(entity);
	}

	bench->watch.stop();
}

void
benchmark()
{
	usize limit = 100;

	compare_benchmark(std::cout, {
	CPPRELUDE_BENCHMARK(create_entt_entities, limit),
	CPPRELUDE_BENCHMARK(create_ecs_entities, limit)
	});
	
	//compare_benchmark(std::cout, {
	//CPPRELUDE_BENCHMARK(create_entt_small_entities, &registry, entities_entt),
	//CPPRELUDE_BENCHMARK(create_ecs_small_entities, &w, entities)
	//});
	//
	//compare_benchmark(std::cout, {
	//	CPPRELUDE_BENCHMARK(destroy_entt_entities, &registry, entities_entt),
	//	CPPRELUDE_BENCHMARK(destroy_ecs_entities, &w, entities)
	//});

	//ecs::World world;
	//for (usize i = 0; i < limit; ++i)
	//	w.add_property<Position>(world.create_entity(), Position());
	//	
	//entt::DefaultRegistry _registry;
	//for (usize i = 0; i < limit; ++i)
	//	registry.create<Position>();

	//compare_benchmark(std::cout, {
	//CPPRELUDE_BENCHMARK(iterate_entt_small_entities, &_registry),
	//CPPRELUDE_BENCHMARK(iterate_ecs_small_entities, &world)
	//});

	//compare_benchmark(std::cout, {
	//CPPRELUDE_BENCHMARK(destroy_entt_small_entities, &_registry),
	//CPPRELUDE_BENCHMARK(destroy_ecs_small_entities, limit)
	//});
}
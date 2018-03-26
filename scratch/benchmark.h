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
destroy_ecs_entities(workbench* bench, cpprelude::usize limit)
{
	ecs::World world;
	bench->watch.start();

	for (usize i = 0; i < limit; ++i)
		world.create_entity().entity_id;

	auto& entities = world.get_all_world_entities();
	for (auto entity: entities)
		world.kill_entity(entity);

	bench->watch.stop();
}
//
void
destroy_entt_entities(workbench* bench, cpprelude::usize limit)
{
	entt::DefaultRegistry registry;
	dynamic_array<entt::DefaultRegistry::entity_type> entities;
	bench->watch.start();

	for (usize i = 0; i < limit; ++i)
		entities.insert_back(registry.create());

	for (auto entity: entities)
		registry.destroy(entity);

	bench->watch.stop();
}
//
// creating, iterating and destroying small entities (entities with only one component)
void
create_ecs_single_component_entities(workbench* bench, cpprelude::usize limit)
{
	ecs::World world;
	bench->watch.start();
	

	for (usize i = 0; i < limit; ++i)
		world.create_entity<Position>();
	
	bench->watch.stop();
}

void
create_entt_single_component_entities(workbench* bench, cpprelude::usize limit)
{
	entt::DefaultRegistry registry;
	bench->watch.start();
		
	for (usize i = 0; i < limit; ++i)
		registry.create<Position>();

	bench->watch.stop();
}

void
iterate_ecs_single_component_entities(workbench* bench, usize limit)
{
	ecs::World world;
	bench->watch.start();

	for (usize i = 0; i < limit; ++i)
		world.create_entity<Position>();

	auto view = world.get_world_components<Position>();
	
	for(auto p: view)
	{
		world.has_all<Position>(p.entity_id);
	}

	bench->watch.stop();
}

void
iterate_entt_single_component_entities(workbench* bench, usize limit)
{
	entt::DefaultRegistry registry;
	bench->watch.start();

	for (usize i = 0; i < limit; ++i)
		registry.create<Position>();

	auto view = registry.view<Position>();

	for (auto e : view)
	{
		registry.has<Position>(e);
	}

	bench->watch.stop();
}

void
destroy_ecs_single_component_entities(workbench* bench, cpprelude::usize limit)
{
	ecs::World world;
		
	bench->watch.start();

	for (usize i = 0; i < limit; ++i)
		world.create_entity<Position>();

	auto& entities = world.get_all_world_entities();

	for (auto e : entities)
		world.kill_entity(e);

	bench->watch.stop();
}

void
destroy_entt_single_component_entities(workbench* bench, cpprelude::usize limit)
{
	entt::DefaultRegistry registry;
	bench->watch.start();
	
	for (usize i = 0; i < limit; ++i)
		registry.create<Position>();

	auto view = registry.view<Position>();
	
	for (auto entity : view)
	{
		registry.destroy(entity);
	}

	bench->watch.stop();
}

void
create_ecs_2_comp_entities(workbench* bench, cpprelude::usize limit)
{
	ecs::World world;
	bench->watch.start();
	auto& entities = world.get_all_world_entities();

	for (usize i = 0; i < limit; ++i)
		world.create_entity<Position>();
	
	for (auto entity : entities)
		world.add_property<u32>(entity);

	bench->watch.stop();
}

void
create_entt_2_comp_entities(workbench* bench, cpprelude::usize limit)
{
	entt::DefaultRegistry registry;
	bench->watch.start();

	auto entities = registry.view<Position>();

	for (usize i = 0; i < limit; ++i)
		registry.create<Position>();

	for (auto entity : entities)
		registry.assign<u32>(entity);

	bench->watch.stop();
}

void
create_ecs_5_comp_entities(workbench* bench, cpprelude::usize limit)
{
	ecs::World world;
	bench->watch.start();
	auto& entities = world.get_all_world_entities();

	for (usize i = 0; i < limit; ++i)
		world.create_entity<Position>();

	for (auto entity : entities)
	{
		world.add_property<u32>(entity);
		world.add_property<r32>(entity);
		world.add_property<char>(entity);
		world.add_property<r64>(entity);
	}
		

	bench->watch.stop();
}

void
create_entt_5_comp_entities(workbench* bench, cpprelude::usize limit)
{
	entt::DefaultRegistry registry;
	bench->watch.start();

	auto entities = registry.view<Position>();

	for (usize i = 0; i < limit; ++i)
		registry.create<Position>();

	for (auto entity : entities)
	{
		registry.assign<u32>(entity);
		registry.assign<r32>(entity);
		registry.assign<char>(entity);
		registry.assign<r64>(entity);
	}
		
	bench->watch.stop();
}

void
benchmark()
{
	usize limit = 1000000;

	compare_benchmark(std::cout, {
	CPPRELUDE_BENCHMARK(create_entt_entities, limit),
	CPPRELUDE_BENCHMARK(create_ecs_entities, limit)
	});
	
	compare_benchmark(std::cout, {
	CPPRELUDE_BENCHMARK(create_entt_single_component_entities, limit),
	CPPRELUDE_BENCHMARK(create_ecs_single_component_entities, limit)
	});
	
	compare_benchmark(std::cout, {
	CPPRELUDE_BENCHMARK(iterate_entt_single_component_entities, limit),
	CPPRELUDE_BENCHMARK(iterate_ecs_single_component_entities, limit)
	});

	compare_benchmark(std::cout, {
	CPPRELUDE_BENCHMARK(destroy_entt_entities, limit),
	CPPRELUDE_BENCHMARK(destroy_ecs_entities, limit)
	});

	compare_benchmark(std::cout, {
	CPPRELUDE_BENCHMARK(destroy_entt_single_component_entities, limit),
	CPPRELUDE_BENCHMARK(destroy_ecs_single_component_entities, limit)
	});
	
	compare_benchmark(std::cout, {
	CPPRELUDE_BENCHMARK(create_entt_2_comp_entities, limit),
	CPPRELUDE_BENCHMARK(create_ecs_2_comp_entities, limit)
	});
	
	compare_benchmark(std::cout, {
	CPPRELUDE_BENCHMARK(create_entt_5_comp_entities, limit),
	CPPRELUDE_BENCHMARK(create_ecs_5_comp_entities, limit)
	});
}
#include <catch.hpp>
#include <ecs/world.h>

using namespace cpprelude;
using namespace ecs;

TEST_CASE("world make test", "[world-make]")
{
	SECTION("Case 01")
	{
		// test make_entity
		//World world;

		//world.make_entity();
		//world.make_entity();

		//CHECK(world.entity_bag.count() == 2);

		//world.make_entity();

		//CHECK(world.entity_bag.count() == 3);
		//CHECK(world.entity_bag[0].id == 0);
		//CHECK(world.entity_bag[1].id == 1);
		//CHECK(world.entity_bag[2].id == 2);		
		//CHECK(world.entity_bag[0].world == &world);
	}

	SECTION("Case 02")
	{
		// test make_component separately from entities
		//World world;
		//world.make_component<u32>(332, INVALID_ID);
		//world.make_component<u32>(64, INVALID_ID);

		//CHECK(world.component_bag.count() == 2);
		//CHECK(world.component_types.count() == 1);
		//CHECK(world.entity_components.count() == 0);

		//world.make_component<r32>(64.57, INVALID_ID);
		//world.make_component<cpprelude::string>("Hello", INVALID_ID);

		//CHECK(world.component_types.count() == 3);
		//CHECK(world.component_bag.count() == 4);
		//CHECK(world.entity_components.count() == 0);
	}

	SECTION("Case 03")
	{
		// test make_component binded to entities
	}
}

TEST_CASE("world kill test", "[world-kill]")
{
	SECTION("Case 01")
	{
		// test kill_entity without any components
	}

	SECTION("Case 02")
	{
		// test remove_component
	}

	SECTION("Case 03")
	{
		// test kill_entity with remove_component
	}

	SECTION("Case 04")
	{
		// test clean_up
	}
}
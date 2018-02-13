#include <catch.hpp>
#include <ecs/bag.h>
#include <ecs/elements.h>

using namespace cpprelude;
using namespace ecs;

TEST_CASE("bag container test", "[bag]")
{
	bag<u32> test_bag;
	bag<Internal_Component> component_test_bag;

	SECTION("Case 01")
	{
		// basic test 

		CHECK(test_bag.count() == 0);
		CHECK(test_bag.capacity() == RESERVED);
		
		usize size = 256;
		for(u32 i = 0; i < size; ++i)
			test_bag.insert(i);
		
		CHECK(test_bag.count() == size);
		for(u32 i = 0; i < size; ++i)
			CHECK(test_bag[i] == i);
	}

	SECTION("Case 02")
	{
		// another basic test

		CHECK(component_test_bag.empty() == true);
		CHECK(component_test_bag.capacity() == RESERVED);

		u32 count = 100;
		for(u32 i = 0; i < count; ++ i)
			component_test_bag.emplace(i, i + 1, "unsigned int");

		CHECK(component_test_bag.count() == count);
		for(u32 i = 0; i < count; ++i)
		{
			CHECK(component_test_bag[i].id == i);
			CHECK(component_test_bag[i].entity_id == i + 1);
		}

		Internal_Component c(32, 33, "unsigned int");
		component_test_bag.insert(c);

		CHECK(component_test_bag.count() == count + 1);
		CHECK(component_test_bag.back().value().id == 32);
		CHECK(component_test_bag.begin().value().id == 0);
	}

	SECTION("Case 03")
	{
		// test remove function

		CHECK(component_test_bag.empty() == true);
		CHECK(component_test_bag.capacity() == RESERVED);

		u32 count = 10;
		for(u32 i = 0; i < count; ++i)
			component_test_bag.emplace(i, i, "unsigned int");

		CHECK(component_test_bag.count() == count);

		usize chosen = 2;
		component_test_bag.remove(chosen);

		CHECK(component_test_bag.valid(chosen) == false);
		CHECK(component_test_bag.count() == count - 1);

		chosen = 7;
		component_test_bag.remove(chosen);

		CHECK(component_test_bag.valid(chosen) == false);
		CHECK(component_test_bag.count() == count - 2);
	}

	SECTION("Case 04")
	{
		// test removal from the beginning, middle & end, begin& back iterators

		CHECK(component_test_bag.count() == 0);
		CHECK(component_test_bag.capacity() == RESERVED);

		u32 count = 10;
		for(u32 i = 0; i < count; ++i)
			component_test_bag.emplace(i + 1, i + 1, "unsigned int");

		CHECK(component_test_bag.count() == count);
		// check the begin& back iterators
		CHECK(component_test_bag.begin().value().id == 1);
		CHECK(component_test_bag.back().value().id == 10);

		// remove from the beginning
		component_test_bag.remove(0);

		CHECK(component_test_bag.begin().value().id == 2);

		component_test_bag.remove(1);

		CHECK(component_test_bag.begin().value().id == 3);
		CHECK(component_test_bag.back().value().id == 10);

		// remove from the middle
		component_test_bag.remove(4);

		CHECK(component_test_bag.begin().value().id == 3);
		CHECK(component_test_bag.back().value().id == 10);

		// remove from the back
		component_test_bag.remove(9);

		CHECK(component_test_bag.back().value().id == 9);
		CHECK(component_test_bag.begin().value().id == 3);

		component_test_bag.remove(8);

		CHECK(component_test_bag.back().value().id == 8);
	}

	SECTION("Case 05")
	{
		// test reusing after removal
		CHECK(component_test_bag.count() == 0);
		CHECK(component_test_bag.capacity() == RESERVED);

		u32 count = 10;
		for(u32 i = 0; i < count; ++i)
			component_test_bag.emplace(i, i, "unsigned int");

		CHECK(component_test_bag.count() == count);

		component_test_bag.remove(1);
		component_test_bag.remove(4);
		component_test_bag.remove(9);
		
		CHECK(component_test_bag.valid(1) == false);
		CHECK(component_test_bag.valid(4) == false);
		CHECK(component_test_bag.valid(9) == false);
		CHECK(component_test_bag.count() == count - 3);
		CHECK(component_test_bag.back().value().id == 8);

		Internal_Component c(11, 11, "unsigned int");
		component_test_bag.insert(c);

		CHECK(component_test_bag.count() == count - 2);
		CHECK(component_test_bag.back().value().id == 11);

		Internal_Component c2(12, 12, "unsigned int");
		component_test_bag.insert(c2);

		CHECK(component_test_bag.count() == count - 1);
		CHECK(component_test_bag.valid(4));
		CHECK(component_test_bag[4].id == 12);
	}

	SECTION("Case 06")
	{
		// test for loops using iterators and range-based loop
		CHECK(component_test_bag.count() == 0);
		CHECK(component_test_bag.capacity() == RESERVED);		
	
		u32 size = 100;
		for(u32 i = 0; i < size; ++i)
			component_test_bag.emplace(i, i, "unsigned int");

		CHECK(component_test_bag.count() == size);

		u32 count = 0;
		for (auto it = component_test_bag.begin(); it != component_test_bag.end(); ++it)
			CHECK(it->id == count++);
		
		count = 0;
		for (auto component : component_test_bag)
			CHECK(component.id == count++);

		component_test_bag.remove(1);
		component_test_bag.remove(7);
		component_test_bag.remove(99);

		count = 0;
		for (auto it = component_test_bag.begin(); it != component_test_bag.end(); ++it)
		{
			if (count == 1 || count == 7)
				++count;
			CHECK(it->id == count++);
		}

		count = 0;
		for (auto component: component_test_bag)
		{
			if (count == 1 || count == 7)
				++count;
			CHECK(component.id == count++);
		}

		CHECK(component_test_bag.back().value().id == 98);
	}
}
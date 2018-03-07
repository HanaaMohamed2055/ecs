#include "benchmark.h"
#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/world.h>
int main()
{
	ecs::sparse_unordered_set<ecs::ID> test_set;
	ecs::ID id1, id2, id3, id4, id5, id6;
	id1.number = 0, ++id1.version;
	id2.number = 1, ++id2.version;
	id3.number = 2, ++id3.version;
	id4.number = 3, ++id4.version;
	id5.number = 4, ++id5.version;
	id6.number = 2, ++id6.version;
	test_set.insert(id1);
	test_set.insert(id2);
	test_set.insert(id3);
	test_set.insert(id4);
	test_set.insert(id5);

	for (auto id : test_set)
		std::cout << id.number << std::endl;

	std::cout << std::endl;

	test_set.remove(2);

	for (auto id : test_set)
		std::cout << id.number << std::endl;

	std::cout << std::endl;

	test_set.insert(id6);

	for (auto id : test_set)
		std::cout << id.number << std::endl;

	std::cout << std::endl;
	for (auto i = 0; i < test_set.sparse_count(); ++i)
	{
		std::cout << test_set[i].number << std::endl;
	}

	test_set.remove(0);
	test_set.remove(1);
	test_set.remove(2);
	test_set.remove(3);
	test_set.remove(4);
}

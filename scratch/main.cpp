#include <cpprelude/defines.h>
#include <ecs/ecs.h>


using namespace ecs;

int main()
{	
	bag<entity> entity_bag;
	entity_bag.insert(entity(01));
	entity_bag.insert(entity(123));
	entity_bag.insert(entity(89382));

	for (auto e: entity_bag)
	{
		std::cout << e._eid << std::endl;
	}
}


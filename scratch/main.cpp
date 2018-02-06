#include <cpprelude/defines.h>
#include <ecs/ecs.h>


using namespace ecs;

int main()
{
	entity_component_manager em;
	for (int i = 0; i < PREALLOCATED_COUNT * 2; ++i)
		em.make_component<u32>(i);
	//entity_component_manager em;
	//auto eptr1 = em.make_entity();
	//auto eptr2 = em.make_entity();

	//auto comp1 = em.make_component<u32>(32);
	//auto comp2 = em.make_component<u32>(64);
	//auto comp3 = em.make_component<r32>(121.239);
	//auto comp4 = em.make_component<r32>(919.938);

	//em.bind_to_entity(eptr1, comp1);
	//em.bind_to_entity(eptr1, comp3);
	//em.bind_to_entity(eptr2, comp4);
	//em.bind_to_entity(eptr1, comp2);

	//auto e1 = em.get_all_components(eptr1->_eid);
	//for (auto e : e1)
	//	std::cout << e->_cid << std::endl;
	//std::cout << "-----------------\n";
	//em.unbind_component(comp1);
	//e1 = em.get_all_components(eptr1->_eid);
	//auto e2 = em.get_all_components(eptr2->_eid);
	//for (auto e : e1)
	//	std::cout << e->_cid << std::endl;
	//std::cout << "-------------------\n";
	//for (auto e : e2)
	//	std::cout << e->_cid << std::endl;
	//std::cout << "------------------\n";
	//em.unbind_component(comp4);
	//em.bind_to_entity(eptr1, comp4);
	//
	//e2 = em.get_all_components(eptr2->_eid);
	//e1 = em.get_all_components(eptr1->_eid);

	//for (auto e : e1)
	//{
	//	std::cout << e->position << std::endl;
	//	std::cout << e->_cid << std::endl;
	//	std::cout << "---" << std::endl;
	//}

	//if (e2.empty())
	//	std::cout << "success\n" << std:: endl;
	//auto found = std::find(em.entities_components_table[0][typeid(u32).name()].begin(),
	//	em.entities_components_table[0][typeid(u32).name()].end(), comp1->position);
	//std::cout << found - em.entities_components_table[0][typeid(u32).name()].begin() << std::endl;
}


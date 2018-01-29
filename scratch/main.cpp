#include <cpprelude/defines.h>
#include <cpprelude/allocator.h>
#include <ecs/ecs.h>

using namespace ecs;

int main()
{	

	std::cout << (unsigned)((2 << 31) - 1) << std::endl;
	u32 width = 90;
	r64 height = 10.45;
	entity_component_manager em;
	component<u32> cx(&width);
	component<r64> cy(&height);
	auto e = em.create();
	em.bind(e->eid, cx);
	em.bind(e->eid, cy);


	//auto q = reinterpret_cast<component<int>*>(e);
	//std::cout << *(q->data) << std::endl;
	return 0;
}
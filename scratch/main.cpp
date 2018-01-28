#include <cpprelude/defines.h>
#include <ecs/ecs.h>

using namespace ecs;

int main()
{	
	int* x = new int(4);
	float y = 56.939;
	auto e = new entity(1);

	component<int> cx(x);
	component<float> cy(&y);
	component<entity>ce(e);
		
	y = 4.5;

	return 0;
}
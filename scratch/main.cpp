#include <iostream>
#include <cstring>

#include <cpprelude/defines.h>
#include <symbyo/math/vector_ops.h>
#include <ecs/world.h>
#include <PixelToaster.h>

using namespace std;
using namespace ecs;
using namespace cpprelude;
using namespace symbyo;

const i32 N = 1000;
const i32 WINDOW_WIDTH = 1280;
const i32 WINDOW_HEIGHT = 720;

class LCRGF
{
public:
	LCRGF(){ mCurrentValue = 1; }

	float randf1to1()
	{
		mCurrentValue *= 16807;
		return ((float)mCurrentValue) / (float)0x80000000;
	}

private:
	int mCurrentValue;
};

math::vec3i lerp(const math::vec3i& a, const math::vec3i& b, float step)
{
	return math::vec3i(a[0] + (b[0] - a[0]) * step,
					   a[1] + (b[1] - a[1]) * step,
					   a[2] + (b[2] - a[2]) * step);
}

#pragma region Components

struct collision_component
{
	math::vec3f position;
	math::vec3f velocity;

	collision_component(LCRGF& lcrgf)
		: velocity(0.0f)
	{
		position[0] = lcrgf.randf1to1();
		position[1] = lcrgf.randf1to1();
		position[2] = lcrgf.randf1to1();
	}
};

struct rendering_component
{
	math::vec2i position;
	math::vec3i color;

	rendering_component()
		: position(0), color(0)
	{}
};

#pragma endregion

#pragma region Systems

namespace collision_system
{
	void collide(World& world, r32 step)
	{
		auto collision_components = world.get_world_components<collision_component>();
		for (auto iter = collision_components.begin(); iter != collision_components.end(); ++iter)
		{
			math::vec3f acceleration(0.0f);
			math::vec3f temp_position(0.0f);

			auto& neo = *iter;

			for (auto iter_2 = collision_components.begin(); iter_2 != collision_components.end(); ++iter_2)
			{
				if (iter == iter_2)
					continue;

				auto& agent_smith = *iter_2;

				auto delta = math::sub(agent_smith.position, neo.position);
				r32 inv;
				{
					math::simd4 result = math::simd::mul(delta._data, delta._data);
					result = _mm_hadd_ps(result, result);
					result = _mm_hadd_ps(result, result);
					result = _mm_rsqrt_ps(result);
					inv = math::simd::x(result);
				}
				auto mass = 10.0f * inv * inv * inv;

				acceleration = math::add(acceleration, math::mul(delta, mass));
			}

			temp_position = math::add(math::add(neo.position, math::mul(neo.velocity, step)), math::mul(acceleration, 0.5f * step * step));

			neo.velocity = math::add(neo.velocity, math::mul(acceleration, step));

			neo.position = temp_position;
		}
	}
}

namespace projection_system
{
	void project(World& world)
	{
		auto collision_components = world.get_world_components<collision_component>();

		for (auto iter = collision_components.begin(); iter != collision_components.end(); ++iter)
		{
			// TODO
			auto e = world.entity_set[iter.entity()];
			Entity ent(e, &world);

			auto& r = world.get<rendering_component>(ent);
			auto& c = *iter;
			auto x = c.position[0], y = c.position[1], z = c.position[2];
			r.position[0] = (int)((WINDOW_WIDTH / 2) + x * 200 / (z + 1.0f));
			r.position[1] = (int)((WINDOW_HEIGHT / 2) + y * 200 / (z + 1.0f));
		}
	}
}

namespace rendering_system
{
	void clear(dynamic_array<PixelToaster::TrueColorPixel>& pixels)
	{
		std::memset(pixels.data(), 0, pixels._data_block.size);
	}

	void draw(World& world, dynamic_array<PixelToaster::TrueColorPixel>& pixels)
	{
		auto rendering_components = world.get_world_components<rendering_component>();
		for (const auto& component : rendering_components)
		{
			// Only draw bodies that are in view
			if (component.position[0] >= 0 && component.position[0] < WINDOW_WIDTH &&
				component.position[1] >= 0 && component.position[1] < WINDOW_HEIGHT)
			{
				i32 flat_index = component.position[0] + component.position[1] * WINDOW_WIDTH;
				pixels[flat_index].r = component.color[0];
				pixels[flat_index].g = component.color[1];
				pixels[flat_index].b = component.color[2];
			}
		}
	}
}

#pragma endregion

int main()
{
	PixelToaster::Display display("N-Body Simulation", WINDOW_WIDTH, WINDOW_HEIGHT, PixelToaster::Output::Default, PixelToaster::Mode::TrueColor);
	dynamic_array<PixelToaster::TrueColorPixel> pixels(WINDOW_WIDTH * WINDOW_HEIGHT);

	World world;

	auto start_color = math::vec3i(255, 0, 0);
	auto end_color = math::vec3i(255, 255, 0);
	LCRGF lcrgf;

	for (i16 i = 0; i < N; ++i)
	{
		auto entity = world.create_entity();

		world.add_property<collision_component>(entity, lcrgf);
		auto& component = world.add_property<rendering_component>(entity);
		component.color = lerp(start_color, end_color, (i + 150) / (float)N);
	}

	PixelToaster::Timer timer;
	r64 delta = 0.01;

	while (display.open())
	{
		timer.reset();

		rendering_system::clear(pixels);

		collision_system::collide(world, 0.001f);

		projection_system::project(world);

		rendering_system::draw(world, pixels);

		delta = timer.delta();
		printf("\rone frame took %gs", delta);

		display.update(pixels.data());
	}
}

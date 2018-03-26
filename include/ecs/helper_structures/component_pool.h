#include <cpprelude/dynamic_array.h>

#include <ecs/elements.h>
#include <ecs/utility.h>

namespace ecs
{
	struct component_pool
	{
		utility::base_type_utils* utils = nullptr;
		cpprelude::memory_context* _context = nullptr;

		cpprelude::dynamic_array<internal_component> components;
		cpprelude::dynamic_array<bool> managed;
		cpprelude::dynamic_array<cpprelude::usize> sparse;

		component_pool(cpprelude::memory_context* context)
			:_context(context),
			components(context),
			sparse(context)
		{}

		component_pool()
			:_context(nullptr)
		{}

		void
		insert_at(cpprelude::usize entity_index, void* data_ptr, bool allocated_by_pool = true)
		{
			if (sparse.capacity() <= entity_index)
			{
				sparse.expand_back(2 * (entity_index + 1), INVALID_PLACE);
				managed.expand_back(2 * (entity_index + 1), false);
				components.reserve(2 * (entity_index + 1));
			}
			sparse[entity_index] = components.count();
			components.insert_back({ data_ptr, entity_index });
			managed[entity_index] = allocated_by_pool;
		}

		void
		remove(cpprelude::usize entity_index)
		{
			if (entity_index < sparse.count())
			{
				cpprelude::usize dense_index = sparse[entity_index];
				
				if (managed[entity_index])
				{
					utils->free(components[dense_index].data, _context);
					managed[entity_index] = false;
				}
				components[dense_index].data = nullptr;
				std::swap(components[dense_index], components[components.count() - 1]);
				sparse[components[dense_index].entity_index] = dense_index;
				sparse[entity_index] = INVALID_PLACE;
				components.remove_back();
			}
		}

		bool
		has(cpprelude::usize entity_index) const
		{
			return entity_index < sparse.count()
				&& sparse[entity_index] != INVALID_PLACE;
		}

		internal_component&
		operator[](cpprelude::usize entity_index)
		{
			return components[sparse[entity_index]];
		}

		const internal_component&
		operator[](cpprelude::usize entity_index) const
		{
			return components[sparse[entity_index]];
		}

		internal_component&
		get(cpprelude::usize entity_index)
		{
			return components[sparse[entity_index]];
		}

		const internal_component&
		get(cpprelude::usize entity_index) const
		{
			return components[sparse[entity_index]];
		}
	};
}
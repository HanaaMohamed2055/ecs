# pragma once

#include <atomic>


#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>

namespace utility
{
	namespace details
	{
		template<typename ...>
		struct type_number
		{
			static std::atomic<cpprelude::usize> number;
 
			template<typename ...>
			static cpprelude::usize type_ID()
			{
				static const cpprelude::usize type_id = number.fetch_add(1);
				return type_id;
			}
		};

		template<typename... types>
		std::atomic<cpprelude::usize> type_number<types...>::number{};

		
		const cpprelude::u64 FRONT_SIZE = sizeof("utility::details::type_helper<") - 1u;
		const cpprelude::u64 BACK_SIZE = sizeof(">::get_type_name") - 1u;
				
		template<typename T>
		struct type_helper
		{			
			static const char* 
			get_type_name()
			{
				// here we should account for GCC later as it is claimed not to have __FUNCTION macro
				// it has instead a similar one called __PRETTY_FUNCTION__
				static const cpprelude::usize size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
				static char type_name[size] = {};
				memcpy(type_name, __FUNCTION__ + FRONT_SIZE, size - 1u);
				return type_name;
			}
					
			static cpprelude::usize
			get_type_identifier()
			{
				return type_number<>::type_ID<T>();
			}
		};
		
		// implemented only the parts we need from struct tuple 
		// for iterating on Template types

		template<typename ...Ts>
		struct TupleTypes
		{};

		template< cpprelude::usize N, typename T>
		struct TupleElement;

		// base case
		template <typename Head, typename... Ts>
		struct TupleElement<0, TupleTypes<Head, Ts...>>
		{
			typedef Head type;
		};

		// recursive case
		template <cpprelude::usize N, typename Head, typename ... Ts>
		struct TupleElement<N, TupleTypes<Head, Ts...>>
		{
			typedef typename TupleElement<N - 1, TupleTypes<Ts...>>::type type;
		};

		template<typename ...Ts>
		struct type_list
		{
			template<cpprelude::usize N>
			using type = typename TupleElement<N, TupleTypes<Ts...>>::type;

			constexpr cpprelude::usize
			count() const
			{
				return sizeof...(Ts);
			}
		};

		template<class TypeList, cpprelude::usize... Is>
		cpprelude::dynamic_array<cpprelude::usize>
		get_types_impl(std::index_sequence<Is...>)
		{
			return { utility::get_type_identifier<TypeList::type<Is>>()... };
		}
	}


	template<class... Ts>
	cpprelude::dynamic_array<cpprelude::usize>
	get_types_identifiers()
	{		
		details::type_list<Ts...> types;
		return details::get_types_impl<decltype(types)>(std::make_index_sequence<types.count()>{});
	}
	
	template<typename T>
	const char* 
	get_type_name()
	{
		return details::type_helper<T>::get_type_name();
	}

	template<typename T>
	cpprelude::usize
	get_type_identifier()
	{
		return details::type_helper<T>::get_type_identifier();
	}

	struct base_type_utils
	{
		using free_func = void(*)(void*&, cpprelude::memory_context*);
		using copy_func = void(*)(void*, void*&, cpprelude::memory_context*);

		const char* type;
		free_func free = nullptr;
		copy_func copy = nullptr;
	};

	template<typename T>
	struct type_utils : base_type_utils
	{
		type_utils()
		{
			type = get_type_name<T>();
			free = internal_destroy<T>;
			copy = internal_clone<T>;
		}

	};

	template<typename T>
	type_utils<T>* get_type_utils()
	{
		static type_utils<T> utils;
		return &utils;
	}

	template<typename T>
	void
	internal_destroy(void*& d, cpprelude::memory_context* _context)
	{
		if (d == nullptr) return;
		T* data = (T*)d;
		data->~T();
		auto data_slice = cpprelude::make_slice(data);
		_context->free(data_slice);
		d = nullptr;
	}

	template<typename T>
	void
	internal_clone(void* from, void*& to, cpprelude::memory_context* context)
	{
		if (from != nullptr)
		{
			T value = *(static_cast<T*>(from));
			
			to = context->alloc<T>();
			new(to) T(value);
		}
	}
}

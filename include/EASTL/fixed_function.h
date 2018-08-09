/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FIXED_FUNCTION_H
#define EASTL_FIXED_FUNCTION_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/function_detail.h>

namespace eastl
{

	#define EASTL_INTERNAL_FIXED_FUNCTION_STATIC_ASSERT(TYPE) \
		static_assert(sizeof(TYPE) <= sizeof(typename Base::FunctorStorageType), "fixed_function local buffer is not large enough to hold the callable object.")

	template <int, typename>
	class fixed_function;

	template <int SIZE_IN_BYTES, typename R, typename... Args>
	class fixed_function<SIZE_IN_BYTES, R(Args...)> : public internal::function_detail<SIZE_IN_BYTES, R(Args...)>
	{
	private:
		using Base = internal::function_detail<SIZE_IN_BYTES, R(Args...)>;
	public:
		using typename Base::result_type;

		fixed_function() EA_NOEXCEPT = default;
		fixed_function(std::nullptr_t p) EA_NOEXCEPT
			: Base(p)
		{
		}

		fixed_function(const fixed_function& other)
			: Base(other)
		{
		}

		fixed_function(fixed_function&& other)
			: Base(eastl::move(other))
		{
		}

		template <typename Functor, typename = EASTL_INTERNAL_FUNCTION_VALID_FUNCTION_ARGS(Functor, R, Args..., Base, fixed_function)>
		fixed_function(Functor functor)
			: Base(eastl::move(functor))
		{
			EASTL_INTERNAL_FIXED_FUNCTION_STATIC_ASSERT(Functor);
		}

		~fixed_function() EA_NOEXCEPT = default;

		fixed_function& operator=(const fixed_function& other)
		{
			Base::operator=(other);
			return *this;
		}

		fixed_function& operator=(fixed_function&& other)
		{
			Base::operator=(eastl::move(other));
			return *this;
		}

		fixed_function& operator=(std::nullptr_t p) EA_NOEXCEPT
		{
			Base::operator=(p);
			return *this;
		}

		template <typename Functor, typename = EASTL_INTERNAL_FUNCTION_VALID_FUNCTION_ARGS(Functor, R, Args..., Base, fixed_function)>
		fixed_function& operator=(Functor&& functor)
		{
			EASTL_INTERNAL_FIXED_FUNCTION_STATIC_ASSERT(eastl::decay_t<Functor>);
			Base::operator=(eastl::forward<Functor>(functor));
			return *this;
		}

		template <typename Functor>
		fixed_function& operator=(eastl::reference_wrapper<Functor> f) EA_NOEXCEPT
		{
			EASTL_INTERNAL_FIXED_FUNCTION_STATIC_ASSERT(eastl::reference_wrapper<Functor>);
			Base::operator=(f);
			return *this;
		}

		void swap(fixed_function& other) EA_NOEXCEPT
		{
			Base::swap(other);
		}

		explicit operator bool() const EA_NOEXCEPT
		{
			return Base::operator bool();
		}

		R operator ()(Args... args) const
		{
			return Base::operator ()(eastl::forward<Args>(args)...);
		}

	#if EASTL_RTTI_ENABLED

		const std::type_info& target_type() const EA_NOEXCEPT
		{
			return Base::target_type();
		}

		template <typename Functor>
		Functor* target() EA_NOEXCEPT
		{
			return Base::target();
		}

		template <typename Functor>
		const Functor* target() const EA_NOEXCEPT
		{
			return Base::target();
		}

	#endif
	};

	template <int S, typename R, typename... Args>
	bool operator==(const fixed_function<S, R(Args...)>& f, std::nullptr_t) EA_NOEXCEPT
	{
		return !f;
	}

	template <int S, typename R, typename... Args>
	bool operator==(std::nullptr_t, const fixed_function<S, R(Args...)>& f) EA_NOEXCEPT
	{
		return !f;
	}

	template <int S, typename R, typename... Args>
	bool operator!=(const fixed_function<S, R(Args...)>& f, std::nullptr_t) EA_NOEXCEPT
	{
		return !!f;
	}

	template <int S, typename R, typename... Args>
	bool operator!=(std::nullptr_t, const fixed_function<S, R(Args...)>& f) EA_NOEXCEPT
	{
		return !!f;
	}

	template <int S, typename R, typename... Args>
	void swap(fixed_function<S, R(Args...)>& lhs, fixed_function<S, R(Args...)>& rhs)
	{
		lhs.swap(rhs);
	}

} // namespace eastl

#endif // EASTL_FIXED_FUNCTION_H

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_UNIQUE_PTR_H
#define EASTL_UNIQUE_PTR_H


#include <EABase/nullptr.h>
#include <EASTL/internal/config.h>
#include <EASTL/internal/smart_ptr.h>   // Defines smart_ptr_deleter
#include <EASTL/internal/move_help.h>   // Defines EASTL_MOVE
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>
#include <EASTL/functional.h>
#include <EASTL/bonus/compressed_pair.h>
#include <stddef.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif


namespace eastl
{
	/// class unique_ptr
	///
	/// This class implements a unique_ptr template. This is a class which is 
	/// similar to the C++ auto_ptr template, except that it prohibits copying 
	/// of itself, for safety.
	///
	/// More specifically, the unique_ptr class template stores a pointer to a 
	/// dynamically allocated object.  The object pointed to is automatically 
	/// deleted on destructor of unique_ptr or can be manually deleted via the 
	/// unique_ptr::reset function. 
	///
	/// Memory allocation notes:
	/// unique_ptr doesn't allocate memory; all allocated pointers are externally 
	/// derived. unique_ptr does deallocate memory, though always through the 
	/// user-provided deleter. You need to make sure you are consistent in providing 
	/// a deleter which frees memory in a way that matches how it was originally allocated.
	/// Deleters have instance information and are moved between containers the same way 
	/// the allocated pointers are. Thus you can allocate memory via some heap and 
	/// provide a deleter which contains a pointer to that same heap, and regardless
	/// of what you do with the unique_ptr, including moving it to another unique_ptr,
	/// the deletion will use the originally provided heap.
	///
	/// Example usage:
	///     unique_ptr<int> p(new int);
	///     *p = 4;
	///
	///     unique_ptr<int[]> pArray(new int[4]);
	///     p[0] = 4;
	///
	/// Type completeness requirements
	/// http://stackoverflow.com/questions/6012157/is-stdunique-ptrt-required-to-know-the-full-definition-of-t/6089065#6089065
	/// Here is a table which documents several members of shared_ptr and unique_ptr with respect to completeness requirements. 
	/// If the member requires a complete type, the entry has a "C", otherwise the table entry is filled with "I".
	/// 
	///                                 unique_ptr       shared_ptr
	///     +------------------------+---------------+---------------+
	///     |          P()           |      I        |      I        |
	///     |  default constructor   |               |               |
	///     +------------------------+---------------+---------------+
	///     |      P(const P&)       |     N/A       |      I        |
	///     |    copy constructor    |               |               |
	///     +------------------------+---------------+---------------+
	///     |         P(P&&)         |      I        |      I        |
	///     |    move constructor    |               |               |
	///     +------------------------+---------------+---------------+
	///     |         ~P()           |      C        |      I        |
	///     |       destructor       |               |               |
	///     +------------------------+---------------+---------------+
	///     |         P(A*)          |      I        |      C        |
	///     +------------------------+---------------+---------------+
	///     |  operator=(const P&)   |     N/A       |      I        |
	///     |    copy assignment     |               |               |
	///     +------------------------+---------------+---------------+
	///     |    operator=(P&&)      |      C        |      I        |
	///     |    move assignment     |               |               |
	///     +------------------------+---------------+---------------+
	///     |        reset()         |      C        |      I        |
	///     +------------------------+---------------+---------------+
	///     |       reset(A*)        |      C        |      C        |
	///     +------------------------+---------------+---------------+
	///
	template <typename T, typename Deleter = eastl::default_delete<T> > 
	class unique_ptr
	{
		static_assert(!is_rvalue_reference<Deleter>::value, "The supplied Deleter cannot be a r-value reference.");
	public:
		typedef Deleter                                                                  deleter_type;
		typedef T                                                                        element_type;
		typedef unique_ptr<element_type, deleter_type>                                   this_type;
		typedef typename Internal::unique_pointer_type<element_type, deleter_type>::type pointer;

	public:
		/// unique_ptr
		/// Construct a unique_ptr from a pointer allocated via new.
		/// Example usage:
		///    unique_ptr<int> ptr;
		EA_CPP14_CONSTEXPR unique_ptr() EA_NOEXCEPT
			: mPair(pointer())
		{
			static_assert(!eastl::is_pointer<deleter_type>::value, "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
		}

		/// unique_ptr
		/// Construct a unique_ptr from a null pointer.
		/// Example usage:
		///    unique_ptr<int> ptr(nullptr);
		EA_CPP14_CONSTEXPR unique_ptr(std::nullptr_t) EA_NOEXCEPT
			: mPair(pointer())
		{
			static_assert(!eastl::is_pointer<deleter_type>::value, "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
		}

		/// unique_ptr
		/// Construct a unique_ptr from a pointer allocated via new.
		/// Example usage:
		///    unique_ptr<int> ptr(new int(3));
		explicit unique_ptr(pointer pValue) EA_NOEXCEPT
			: mPair(pValue)
		{
			static_assert(!eastl::is_pointer<deleter_type>::value, "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
		}

		/// unique_ptr
		/// Constructs a unique_ptr with the owner pointer and deleter specified
		/// Example usage:
		///     eastl::smart_ptr_deleter<int> del;
		///     unique_ptr<int> ptr(new int(3), del);
		unique_ptr(pointer pValue, typename eastl::conditional<eastl::is_lvalue_reference<deleter_type>::value, deleter_type, typename eastl::add_lvalue_reference<const deleter_type>::type>::type deleter) EA_NOEXCEPT
			: mPair(pValue, deleter) {}

		/// unique_ptr
		/// Constructs a unique_ptr with the owned pointer and deleter specified (rvalue)
		/// Example usage:
		///     unique_ptr<int> ptr(new int(3), eastl::smart_ptr_deleter<int>());
		unique_ptr(pointer pValue, typename eastl::remove_reference<deleter_type>::type&& deleter) EA_NOEXCEPT
			: mPair(pValue, eastl::move(deleter))
		{
			static_assert(!eastl::is_lvalue_reference<deleter_type>::value, "deleter_type reference refers to an rvalue deleter. The reference will probably become invalid before used. Change the deleter_type to not be a reference or construct with permanent deleter.");
		}

		/// unique_ptr
		/// Move constructor
		/// Example usage:
		///     unique_ptr<int> ptr(new int(3));
		///     unique_ptr<int> newPtr = eastl::move(ptr);
		unique_ptr(this_type&& x) EA_NOEXCEPT
			: mPair(x.release(), eastl::forward<deleter_type>(x.get_deleter())) {}

		/// unique_ptr
		/// Move constructor
		/// Example usage:
		///     unique_ptr<int> ptr(new int(3));
		///     unique_ptr<int> newPtr = eastl::move(ptr);
		template <typename U, typename E>
		unique_ptr(unique_ptr<U, E>&& u, typename enable_if<!is_array<U>::value && is_convertible<typename unique_ptr<U, E>::pointer, pointer>::value && is_convertible<E, deleter_type>::value && (is_same<deleter_type, E>::value || !is_lvalue_reference<deleter_type>::value)>::type* = 0) EA_NOEXCEPT
			: mPair(u.release(), eastl::forward<E>(u.get_deleter())) {}

		/// unique_ptr
		/// Move assignment
		/// Example usage:
		///     unique_ptr<int> ptr(new int(3));
		///     unique_ptr<int> newPtr(new int(4));
		///     ptr = eastl::move(newPtr);  // Deletes int(3) and assigns mpValue to int(4)
		this_type& operator=(this_type&& x) EA_NOEXCEPT
		{
			reset(x.release());
			mPair.second() = eastl::move(eastl::forward<deleter_type>(x.get_deleter()));
			return *this;
		}

		/// unique_ptr
		/// Move assignment
		template <typename U, typename E>
		typename enable_if<!is_array<U>::value && is_convertible<typename unique_ptr<U, E>::pointer, pointer>::value && is_assignable<deleter_type&, E&&>::value, this_type&>::type
		operator=(unique_ptr<U, E>&& u) EA_NOEXCEPT
		{
			reset(u.release());
			mPair.second() = eastl::move(eastl::forward<E>(u.get_deleter()));
			return *this;
		}

		/// operator=(nullptr_t)
		this_type& operator=(std::nullptr_t) EA_NOEXCEPT
		{
			reset();
			return *this;
		}

		/// ~unique_ptr
		/// Destroys the owned pointer. The destructor for the object
		/// referred to by the owned pointer will be called.
		~unique_ptr() EA_NOEXCEPT
		{
			reset();
		}

		/// reset
		/// Deletes the owned pointer and takes ownership of the 
		/// passed in pointer. If the passed in pointer is the same
		/// as the owned pointer, nothing is done.
		/// Example usage:
		///    unique_ptr<int> ptr(new int(3));
		///    ptr.reset(new int(4));  // deletes int(3)
		///    ptr.reset(NULL);        // deletes int(4)
		void reset(pointer pValue = pointer()) EA_NOEXCEPT
		{
			if (pValue != mPair.first())
			{
				if (auto first = eastl::exchange(mPair.first(), pValue))
					get_deleter()(first);
			}
		}

		/// release
		/// This simply forgets the owned pointer. It doesn't 
		/// free it but rather assumes that the user does.
		/// Example usage:
		///    unique_ptr<int> ptr(new int(3));
		///    int* pInt = ptr.release();
		///    delete pInt;
		pointer release() EA_NOEXCEPT
		{
			pointer const pTemp = mPair.first();
			mPair.first() = pointer();
			return pTemp;
		}

		/// detach
		/// For backwards-compatibility with pre-C++11 code.
		pointer detach() EA_NOEXCEPT { return release(); } 

		/// swap
		/// Exchanges the owned pointer beween two unique_ptr objects. 
		void swap(this_type& x) EA_NOEXCEPT
		{
			mPair.swap(x.mPair);
		}

		/// operator*
		/// Returns the owner pointer dereferenced.
		/// Example usage:
		///    unique_ptr<int> ptr(new int(3));
		///    int x = *ptr;
		typename add_lvalue_reference<T>::type operator*() const // Not noexcept, because the pointer may be NULL.
		{
			return *mPair.first();
		}

		/// operator->
		/// Allows access to the owned pointer via operator->()
		/// Example usage:
		///    struct X{ void DoSomething(); }; 
		///    unique_ptr<int> ptr(new X);
		///    ptr->DoSomething();
		pointer operator->() const EA_NOEXCEPT
		{
			return mPair.first();
		}

		/// get
		/// Returns the owned pointer. Note that this class does 
		/// not provide an operator T() function. This is because such
		/// a thing (automatic conversion) is deemed unsafe.
		/// Example usage:
		///    struct X{ void DoSomething(); }; 
		///    unique_ptr<int> ptr(new X);
		///    X* pX = ptr.get();
		///    pX->DoSomething();
		pointer get() const EA_NOEXCEPT
		{
			return mPair.first();
		}

		/// get_deleter
		/// Returns the deleter used to delete the owned pointer
		/// Example usage:
		/// unique_ptr<int> ptr(new int(3));
		/// eastl::smart_ptr_deleter<int>& del = ptr.get_deleter();
		deleter_type& get_deleter() EA_NOEXCEPT
		{
			return mPair.second();
		}

		/// get_deleter
		/// Const version for getting the deleter
		const deleter_type& get_deleter() const EA_NOEXCEPT
		{
			return mPair.second();
		}

		#ifdef EA_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS
			/// Note that below we do not use operator bool(). The reason for this
			/// is that booleans automatically convert up to short, int, float, etc.
			/// The result is that this: if(uniquePtr == 1) would yield true (bad).
			typedef T* (this_type::*bool_)() const;
			operator bool_() const EA_NOEXCEPT
			{
				if(mPair.first())
					return &this_type::get;
				return NULL;
			}

			bool operator!() const EA_NOEXCEPT
			{
				return (mPair.first() == pointer());
			}
		#else
			/// operator bool
			/// Allows for using a unique_ptr as a boolean. 
			/// Example usage:
			///    unique_ptr<int> ptr(new int(3));
			///    if(ptr)
			///        ++*ptr;
			///
			explicit operator bool() const EA_NOEXCEPT
			{ 
				return (mPair.first() != pointer());
			}
		#endif

		/// These functions are deleted in order to prevent copying, for safety.
		unique_ptr(const this_type&) = delete;
		unique_ptr& operator=(const this_type&) = delete;
		unique_ptr& operator=(pointer pValue) = delete;

	protected:
		eastl::compressed_pair<pointer, deleter_type> mPair;
	}; // class unique_ptr



	/// unique_ptr specialization for unbounded arrays.
	///
	/// Differences from unique_ptr<T>:
	///     - Conversions between different types of unique_ptr<T[], D> or to or 
	///       from the non-array forms of unique_ptr produce an ill-formed program.
	///     - Pointers to types derived from T are rejected by the constructors, and by reset.
	///     - The observers operator* and operator-> are not provided.
	///     - The indexing observer operator[] is provided.
	///     - The default deleter will call delete[].
	/// 
	/// It's not possible to create a unique_ptr for arrays of a known bound (e.g. int[4] as opposed to int[]).
	///
	/// Example usage:
	///     unique_ptr<int[]> ptr(new int[10]);
	///     ptr[4] = 4;
	///
	template <typename T, typename Deleter>
	class unique_ptr<T[], Deleter>
	{
	public:
		typedef Deleter                                                                  deleter_type;
		typedef T                                                                        element_type;
		typedef unique_ptr<element_type[], deleter_type>                                 this_type;
		typedef typename Internal::unique_pointer_type<element_type, deleter_type>::type pointer;

	public:
		EA_CPP14_CONSTEXPR unique_ptr() EA_NOEXCEPT
			: mPair(pointer())
		{
			static_assert(!eastl::is_pointer<deleter_type>::value, "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
		}

		EA_CPP14_CONSTEXPR unique_ptr(std::nullptr_t) EA_NOEXCEPT
			: mPair(pointer())
		{
			static_assert(!eastl::is_pointer<deleter_type>::value, "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
		}

		template <typename P,
		          typename = eastl::enable_if_t<Internal::is_array_cv_convertible<P, pointer>::value>> // Pointers to types derived from T are rejected by the constructors, and by reset.
		 explicit unique_ptr(P pArray) EA_NOEXCEPT
		    : mPair(pArray)
		{
			static_assert(!eastl::is_pointer<deleter_type>::value,
			              "unique_ptr deleter default-constructed with null pointer. Use a different constructor or "
			              "change your deleter to a class.");
		}

		template <typename P>
		unique_ptr(P pArray, typename eastl::conditional<eastl::is_lvalue_reference<deleter_type>::value, deleter_type,
														typename eastl::add_lvalue_reference<const deleter_type>::type>::type deleter,
														typename eastl::enable_if<Internal::is_array_cv_convertible<P, pointer>::value>::type* = 0) EA_NOEXCEPT
			: mPair(pArray, deleter) {}

		template <typename P>
		unique_ptr(P pArray, typename eastl::remove_reference<deleter_type>::type&& deleter, eastl::enable_if_t<Internal::is_array_cv_convertible<P, pointer>::value>* = 0) EA_NOEXCEPT
			: mPair(pArray, eastl::move(deleter))
		{
			static_assert(!eastl::is_lvalue_reference<deleter_type>::value, "deleter_type reference refers to an rvalue deleter. The reference will probably become invalid before used. Change the deleter_type to not be a reference or construct with permanent deleter.");
		}

		unique_ptr(this_type&& x) EA_NOEXCEPT
			: mPair(x.release(), eastl::forward<deleter_type>(x.get_deleter())) {}

		template <typename U, typename E>
		unique_ptr(unique_ptr<U, E>&& u, typename enable_if<Internal::is_safe_array_conversion<T, pointer, U, typename unique_ptr<U, E>::pointer>::value && 
															eastl::is_convertible<E, deleter_type>::value &&
														   (!eastl::is_lvalue_reference<deleter_type>::value || eastl::is_same<E, deleter_type>::value)>::type* = 0) EA_NOEXCEPT
			: mPair(u.release(), eastl::forward<E>(u.get_deleter())) {}

		this_type& operator=(this_type&& x) EA_NOEXCEPT
		{
			reset(x.release());
			mPair.second() = eastl::move(eastl::forward<deleter_type>(x.get_deleter()));
			return *this;
		}

		template <typename U, typename E>
		typename enable_if<Internal::is_safe_array_conversion<T, pointer, U, typename unique_ptr<U, E>::pointer>::value && is_assignable<deleter_type&, E&&>::value, this_type&>::type
		operator=(unique_ptr<U, E>&& u) EA_NOEXCEPT
		{
			reset(u.release());
			mPair.second() = eastl::move(eastl::forward<E>(u.get_deleter()));
			return *this;
		}

		this_type& operator=(std::nullptr_t) EA_NOEXCEPT
		{
			reset();
			return *this;
		}

		~unique_ptr() EA_NOEXCEPT
		{
			reset();
		}

		void reset(pointer pArray = pointer()) EA_NOEXCEPT
		{
			if(pArray != mPair.first())
			{
				if (auto first = eastl::exchange(mPair.first(), pArray))
					get_deleter()(first);
			}
		}

		pointer release() EA_NOEXCEPT
		{
			pointer const pTemp = mPair.first();
			mPair.first() = pointer();
			return pTemp;
		}

		/// detach
		/// For backwards-compatibility with pre-C++11 code.
		pointer detach() EA_NOEXCEPT { return release(); }

		void swap(this_type& x) EA_NOEXCEPT
		{
			mPair.swap(x.mPair);
		}

		/// operator[]
		/// Returns a reference to the specified item in the owned pointer
		/// array. 
		/// Example usage:
		///    unique_ptr<int> ptr(new int[6]);
		///    int x = ptr[2];
		typename add_lvalue_reference<T>::type operator[](ptrdiff_t i) const
		{
			// assert(mpArray && (i >= 0));
			return mPair.first()[i];
		}

		pointer get() const EA_NOEXCEPT
		{
			return mPair.first();
		}

		deleter_type& get_deleter() EA_NOEXCEPT
		{
			return mPair.second();
		}

		const deleter_type& get_deleter() const EA_NOEXCEPT
		{
			return mPair.second();
		}

		#ifdef EA_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS
			typedef T* (this_type::*bool_)() const;
			operator bool_() const EA_NOEXCEPT
			{
				if(mPair.first())
					return &this_type::get;
				return NULL;
			}

			bool operator!() const EA_NOEXCEPT
			{
				return (mPair.first() == pointer());
			}
		#else
			explicit operator bool() const EA_NOEXCEPT
			{ 
				return (mPair.first() != pointer());
			}
		#endif

		/// These functions are deleted in order to prevent copying, for safety.
		unique_ptr(const this_type&) = delete;
		unique_ptr& operator=(const this_type&) = delete;
		unique_ptr& operator=(pointer pArray) = delete;

	protected:
		eastl::compressed_pair<pointer, deleter_type> mPair;
	};



	/// make_unique
	///
	/// The C++11 Standard doesn't have make_unique, but there's no agreed reason as to why.
	/// http://stackoverflow.com/questions/12580432/why-does-c11-have-make-shared-but-not-make-unique
	/// http://herbsutter.com/2013/05/29/gotw-89-solution-smart-pointers/
	/// Herb's solution is OK but doesn't support unique_ptr<[]> (array version). We do the same
	/// thing libc++ does and make a specialization of make_unique for arrays.
	///
	/// make_unique has two cases where you can't use it and need to directly use unique_ptr:
	///     - You need to construct the unique_ptr with a raw pointer.
	///     - You need to specify a custom deleter.
	///
	/// Note: This function uses global new T by default to create the ptr instance, as per 
	/// the C++11 Standard make_shared_ptr.
	///
	/// Example usage:
	///     struct Test{ Test(int, int){} };
	///     auto p = make_unique<Test>(1, 2);
	///
	///     auto pArray = make_unique<Test[]>(4);
	///
	namespace Internal
	{
		template <typename T>
		struct unique_type
			{ typedef unique_ptr<T>   unique_type_single; };

		template <typename T>
		struct unique_type<T[]>
			{ typedef unique_ptr<T[]> unique_type_unbounded_array; };

		template <typename T, size_t N>
		struct unique_type<T[N]>
			{ typedef void            unique_type_bounded_array; };
	}

	template <typename T, typename... Args>
	inline typename Internal::unique_type<T>::unique_type_single make_unique(Args&&... args)
		{ return unique_ptr<T>(new T(eastl::forward<Args>(args)...)); }

	template <typename T>
	inline typename Internal::unique_type<T>::unique_type_unbounded_array make_unique(size_t n)
	{
		typedef typename eastl::remove_extent<T>::type TBase;
		return unique_ptr<T>(new TBase[n]);
	}

	// It's not possible to create a unique_ptr for arrays of a known bound (e.g. int[4] as opposed to int[]).
	template <typename T, typename... Args>
	typename Internal::unique_type<T>::unique_type_bounded_array
	make_unique(Args&&...) = delete;




	/// hash specialization for unique_ptr.
	/// It simply returns eastl::hash(x.get()). If your unique_ptr pointer type (the return value of unique_ptr<T>::get) is 
	/// a custom type and not a built-in pointer type then you will need to independently define eastl::hash for that type.
	template <typename T, typename D>
	struct hash< unique_ptr<T, D> >
	{
		size_t operator()(const unique_ptr<T, D>& x) const EA_NOEXCEPT
			{ return eastl::hash<typename unique_ptr<T, D>::pointer>()(x.get()); }
	};

	/// swap
	/// Exchanges the owned pointer beween two unique_ptr objects.
	/// This non-member version is useful for compatibility of unique_ptr
	/// objects with the C++ Standard Library and other libraries.
	template <typename T, typename D>
	inline void swap(unique_ptr<T, D>& a, unique_ptr<T, D>& b) EA_NOEXCEPT
	{
		a.swap(b);
	}


	template <typename T1, typename D1, typename T2, typename D2>
	inline bool operator==(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b)
	{
		return (a.get() == b.get());
	}

	template <typename T1, typename D1, typename T2, typename D2>
	inline bool operator!=(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b)
	{
		return !(a.get() == b.get());
	}

	/// Returns which unique_ptr is 'less' than the other. Useful when storing
	/// sorted containers of unique_ptr objects.
	template <typename T1, typename D1, typename T2, typename D2>
	inline bool operator<(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b)
	{
		//typedef typename eastl::unique_ptr<T1, D1>::pointer P1;       // We currently need to make these temporary variables, as otherwise clang complains about CPointer being int*&&&.
		//typedef typename eastl::unique_ptr<T2, D2>::pointer P2;       // I think there's something wrong with our common_type type trait implementation.
		//typedef typename eastl::common_type<P1, P2>::type   PCommon;  // "in instantiation of function template specialization 'eastl::operator<<int, int>, no known conversion from 'element_type *' (aka 'int *') to 'int *&&&' for 1st argument"
		//return less<PCommon>()(a.get(), b.get());                     // It looks like common_type is making CPointer be (e.g.) int*&& instead of int*, though the problem may be in how less<> deals with that.

		typedef typename eastl::unique_ptr<T1, D1>::pointer P1;
		typedef typename eastl::unique_ptr<T2, D2>::pointer P2;
		typedef typename eastl::common_type<P1, P2>::type   PCommon;
		PCommon pT1 = a.get();
		PCommon pT2 = b.get();
		return less<PCommon>()(pT1, pT2);
	}

	template <typename T1, typename D1, typename T2, typename D2>
	inline bool operator>(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b) 
	{
		return (b < a);
	}

	template <typename T1, typename D1, typename T2, typename D2>
	inline bool operator<=(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b) 
	{
		return !(b < a);
	}

	template <typename T1, typename D1, typename T2, typename D2>
	inline bool operator>=(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b) 
	{
		return !(a < b);
	}


	template <typename T, typename D>
	inline bool operator==(const unique_ptr<T, D>& a, std::nullptr_t) EA_NOEXCEPT
	{
		return !a;
	}

	template <typename T, typename D>
	inline bool operator==(std::nullptr_t, const unique_ptr<T, D>& a) EA_NOEXCEPT
	{
		return !a;
	}

	template <typename T, typename D>
	inline bool operator!=(const unique_ptr<T, D>& a, std::nullptr_t) EA_NOEXCEPT
	{
		return static_cast<bool>(a);
	}

	template <typename T, typename D>
	inline bool operator!=(std::nullptr_t, const unique_ptr<T, D>& a) EA_NOEXCEPT
	{
		return static_cast<bool>(a);
	}

	template <typename T, typename D>
	inline bool operator<(const unique_ptr<T, D>& a, std::nullptr_t)
	{
		typedef typename unique_ptr<T, D>::pointer pointer;
		return less<pointer>()(a.get(), nullptr);
	}

	template <typename T, typename D>
	inline bool operator<(std::nullptr_t, const unique_ptr<T, D>& b)
	{
		typedef typename unique_ptr<T, D>::pointer pointer;
		pointer pT = b.get();
		return less<pointer>()(nullptr, pT);
	}

	template <typename T, typename D>
	inline bool operator>(const unique_ptr<T, D>& a, std::nullptr_t)
	{
		return (nullptr < a);
	}

	template <typename T, typename D>
	inline bool operator>(std::nullptr_t, const unique_ptr<T, D>& b)
	{
		return (b < nullptr);
	}

	template <typename T, typename D>
	inline bool operator<=(const unique_ptr<T, D>& a, std::nullptr_t)
	{
		return !(nullptr < a);
	}

	template <typename T, typename D>
	inline bool operator<=(std::nullptr_t, const unique_ptr<T, D>& b)
	{
		return !(b < nullptr);
	}

	template <typename T, typename D>
	inline bool operator>=(const unique_ptr<T, D>& a, std::nullptr_t)
	{
		return !(a < nullptr);
	}

	template <typename T, typename D>
	inline bool operator>=(std::nullptr_t, const unique_ptr<T, D>& b)
	{
		return !(nullptr < b);
	}


} // namespace eastl


#endif // Header include guard












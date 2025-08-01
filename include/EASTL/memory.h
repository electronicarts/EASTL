///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements the following functions from the C++ standard that
// are found in the <memory> header:
//
// Utility:
//    late_constructed                  - Extention to standard functionality.
//
// Uninitialized operations:
//    These are the same as the copy, fill, and fill_n algorithms, except that
//    they *construct* the destination with the source values rather than assign
//    the destination with the source values.
//
//    uninitialized_copy
//    uninitialized_copy_n
//    uninitialized_default_construct
//    uninitialized_default_construct_n
//    uninitialized_move
//    uninitialized_move_if_noexcept    - Extention to standard functionality.
//    uninitialized_move_n
//    uninitialized_fill
//    uninitialized_fill_n
//    uninitialized_value_construct
//    uninitialized_value_construct_n
//    uninitialized_copy_fill           - Extention to standard functionality.
//    uninitialized_fill_copy           - Extention to standard functionality.
//    uninitialized_copy_copy           - Extention to standard functionality.
//
// In-place destructor helpers:
//    destruct(T*)                      - Non-standard extension. Equivalent to destroy_at(T*)
//    destruct(first, last)             - Non-standard extension. Equivalent to destroy(first, last)
//    destroy_at(T*)
//    destroy(first, last)
//    destroy_n(first, n)
//
// Alignment
//    align
//    align_advance                     - Extention to standard functionality.
//
// Allocator-related
//    uses_allocator
//    allocator_arg_t
//    allocator_arg
//
// Pointers
//    pointer_traits
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_MEMORY_H
#define EASTL_MEMORY_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/memory_base.h>
#include <EASTL/internal/memory_uses_allocator.h>
#include <EASTL/internal/pair_fwd_decls.h>
#include <EASTL/internal/functional_base.h>
#include <EASTL/algorithm.h>
#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>
#include <EASTL/utility.h>
#include <EASTL/numeric_limits.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <stdlib.h>
#include <new>
EA_RESTORE_ALL_VC_WARNINGS()


// 4530 - C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
// 4146 - unary minus operator applied to unsigned type, result still unsigned
// 4571 - catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught.
EA_DISABLE_VC_WARNING(4530 4146 4571);


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif


namespace eastl
{

	/// EASTL_TEMP_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_TEMP_DEFAULT_NAME
		#define EASTL_TEMP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " temp" // Unless the user overrides something, this is "EASTL temp".
	#endif


	/// late_constructed
	///
	/// Implements a smart pointer type which separates the memory allocation of an object from
	/// the object's construction. The primary use case is to declare a global variable of the
	/// late_construction type, which allows the memory to be global but the constructor executes
	/// at some point after main() begins as opposed to before main, which is often dangerous
	/// for non-trivial types.
	///
	/// The autoConstruct template parameter controls whether the object is automatically default
	/// constructed upon first reference or must be manually constructed upon the first use of
	/// operator * or ->. autoConstruct is convenient but it causes * and -> to be slightly slower
	/// and may result in construction at an inconvenient time.
	///
	/// The autoDestruct template parameter controls whether the object, if constructed, is automatically
	/// destructed when ~late_constructed() is called or must be manually destructed via a call to
	/// destruct().
	///
	/// While construction can be automatic or manual, automatic destruction support is always present.
	/// Thus you aren't required in any case to manually call destruct. However, you may safely manually
	/// destruct the object at any time before the late_constructed destructor is executed.
	///
	/// You may still use late_constructed after calling destruct(), including calling construct()
	/// again to reconstruct the instance. destruct returns the late_constructed instance to a
	/// state equivalent to before construct was called.
	///
	/// Caveat: While late_constructed instances can be declared in global scope and initialize
	/// prior to main() executing, you cannot otherwise use such globally declared instances prior
	/// to main with guaranteed behavior unless you can ensure that the late_constructed instance
	/// is itself constructed prior to your use of it.
	///
	/// Example usage (demonstrating manual-construction):
	///     late_constructed<Widget, false> gWidget;
	///
	///     void main(){
	///         gWidget.construct(kScrollbarType, kVertical, "MyScrollbar");
	///         gWidget->SetValue(15);
	///         gWidget.destruct();
	///     }
	///
	/// Example usage (demonstrating auto-construction):
	///     late_constructed<Widget, true> gWidget;
	///
	///     void main(){
	///         gWidget->SetValue(15);
	///         // You may want to call destruct here, but aren't required to do so unless the Widget type requires it.
	///     }
	///
	template <typename T, bool autoConstruct = true, bool autoDestruct = true>
	class late_constructed
	{
	public:
		using this_type    = late_constructed<T, autoConstruct, autoDestruct>;
		using value_type   = T;
		using storage_type = eastl::aligned_storage_t<sizeof(value_type), eastl::alignment_of_v<value_type>>;

		constexpr late_constructed() noexcept  // In the case of the late_constructed instance being at global scope, we rely on the
		  : mStorage(), mpValue(nullptr) {}    // compiler executing this constructor or placing the instance in auto-zeroed-at-startup memory.

		~late_constructed()
		{
			if (autoDestruct && mpValue)
				(*mpValue).~value_type();
		}

		template <typename... Args>
		void construct(Args&&... args)
		{
			if(!mpValue)
				mpValue = new (&mStorage) value_type(eastl::forward<Args>(args)...);
		}

		bool is_constructed() const noexcept
			{ return mpValue != nullptr; }

		void destruct()
		{
			if(mpValue)
			{
				(*mpValue).~value_type();
				mpValue = nullptr;
			}
		}

		value_type& operator*() noexcept
		{
			if(!mpValue)
				construct();

			EA_ANALYSIS_ASSUME(mpValue);
			return *mpValue;
		}

		const value_type& operator*() const noexcept
		{
			if(!mpValue)
				construct();

			EA_ANALYSIS_ASSUME(mpValue);
			return *mpValue;
		}

		value_type* operator->() noexcept
		{
			if(!mpValue)
				construct();
			return mpValue;
		}

		const value_type* operator->() const noexcept
		{
			if(!mpValue)
				construct();
			return mpValue;
		}

		value_type* get() noexcept
		{
			if(!mpValue)
				construct();
			return mpValue;
		}

		const value_type* get() const noexcept
		{
			if(!mpValue)
				construct();
			return mpValue;
		}

	protected:
		storage_type mStorage; // Declared first because it may have aligment requirements, and it would be more space-efficient if it was first.
		value_type*  mpValue;
	};


	// Specialization that doesn't auto-construct on demand.
	template <typename T, bool autoDestruct>
	class late_constructed<T, false, autoDestruct> : public late_constructed<T, true, autoDestruct>
	{
	public:
		typedef late_constructed<T, true, autoDestruct> base_type;

		typename base_type::value_type& operator*() noexcept
			{ EASTL_ASSERT(base_type::mpValue); return *base_type::mpValue; }

		const typename base_type::value_type& operator*() const noexcept
			{ EASTL_ASSERT(base_type::mpValue); return *base_type::mpValue; }

		typename base_type::value_type* operator->() noexcept
			{ EASTL_ASSERT(base_type::mpValue); return base_type::mpValue; }

		const typename base_type::value_type* operator->() const noexcept
			{ EASTL_ASSERT(base_type::mpValue); return base_type::mpValue; }

		typename base_type::value_type* get() noexcept
			{ return base_type::mpValue; }

		const typename base_type::value_type* get() const noexcept
			{ return base_type::mpValue; }
	};


	// uninitialized_copy
	//
	namespace Internal
	{
		template<bool isTriviallyCopyable, bool isInputIteratorReferenceAddressable, bool areIteratorsContiguous>
		struct uninitialized_copy_impl
		{
			template <typename InputIterator, typename ForwardIterator>
			static ForwardIterator impl(InputIterator first, InputIterator last, ForwardIterator dest)
			{
				typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
				ForwardIterator currentDest(dest);

#if EASTL_EXCEPTIONS_ENABLED
				try
				{
#endif
					for (; first != last; ++first, ++currentDest)
						::new(static_cast<void*>(eastl::addressof(*currentDest))) value_type(*first);
#if EASTL_EXCEPTIONS_ENABLED
				}
				catch (...)
				{
					for (; dest < currentDest; ++dest)
						(*dest).~value_type();
					throw;
				}
#endif

				return currentDest;
			}
		};

		template<>
		struct uninitialized_copy_impl<true, true, false>
		{
			template <typename InputIterator, typename ForwardIterator>
			static ForwardIterator impl(InputIterator first, InputIterator last, ForwardIterator dest)
			{
				typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;

				for (; first != last; ++first, ++dest)
					memmove(eastl::addressof(*dest), eastl::addressof(*first), sizeof(value_type));

				return dest;
			}
		};

		template<>
		struct uninitialized_copy_impl<true, true, true>
		{
			template <typename InputIterator, typename ForwardIterator>
			static ForwardIterator impl(InputIterator first, InputIterator last, ForwardIterator dest)
			{
				typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;

				if (EASTL_UNLIKELY(first == last))
					return dest;

				auto count = (last - first);
				memmove(eastl::addressof(*dest), eastl::addressof(*first), sizeof(value_type) * count);
				return dest + count;
			}
		};
	}

	/// uninitialized_copy
	///
	/// Copies a source range to a destination, copy-constructing the destination with
	/// the source values (and not *assigning* the destination with the source values).
	/// Returns the end of the destination range (i.e. dest + (last - first)).
	///
	/// Declaration:
	///    template <typename InputIterator, typename ForwardIterator>
	///    ForwardIterator uninitialized_copy(InputIterator sourceFirst, InputIterator sourceLast, ForwardIterator destination);
	///
	/// Example usage:
	///    SomeClass* pArray = malloc(10 * sizeof(SomeClass));
	///    uninitialized_copy(pSourceDataBegin, pSourceDataBegin + 10, pArray);
	///
	template <typename InputIterator, typename ForwardIterator>
	inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category  IIC;
		typedef typename eastl::iterator_traits<ForwardIterator>::iterator_category OIC;
		typedef typename eastl::iterator_traits<InputIterator>::value_type         value_type_input;
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type        value_type_output;

		// isTriviallyCopyable identifies if (non-overlapping) objects may be safely copied by means of memcpy/memmove.
		const bool isTriviallyCopyable = eastl::is_same<value_type_input, value_type_output>::value && eastl::is_trivially_copyable<value_type_output>::value;
		// ie. is eastl::addressof(*first) valid? ie. invalid for iterators that return value_type&&.
		const bool isInputIteratorReferenceAddressable = eastl::is_convertible<typename eastl::add_lvalue_reference<value_type_input>::type, typename eastl::iterator_traits<InputIterator>::reference>::value;
		// can memcpy/memmove a contiguous block, not just the individual elements?
		const bool areIteratorsContiguous = (eastl::is_pointer<InputIterator>::value || internal::is_contiguous_iterator<IIC>::value) &&
											(eastl::is_pointer<ForwardIterator>::value || internal::is_contiguous_iterator<OIC>::value);

		return Internal::uninitialized_copy_impl<isTriviallyCopyable, isInputIteratorReferenceAddressable, areIteratorsContiguous>::impl(first, last, result);
	}


	/// uninitialized_copy_n
	///
	/// Copies count elements from a range beginning at first to an uninitialized memory area
	/// beginning at dest. The elements in the uninitialized area are constructed using copy constructor.
	/// If an exception is thrown during the initialization, the function has no final effects.
	///
	/// first:        Beginning of the range of the elements to copy.
	/// dest:         Beginning of the destination range.
	/// return value: Iterator of dest type to the element past the last element copied.
	///
	namespace Internal
	{
		template <typename InputIterator, typename Count, typename ForwardIterator, typename IteratorTag>
		struct uninitialized_copy_n_impl
		{
			static ForwardIterator impl(InputIterator first, Count n, ForwardIterator dest)
			{
				typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
				ForwardIterator currentDest(dest);

				#if EASTL_EXCEPTIONS_ENABLED
					try
					{
				#endif
						for(; n > 0; --n, ++first, ++currentDest)
							::new((void*)(eastl::addressof(*currentDest))) value_type(*first);
				#if EASTL_EXCEPTIONS_ENABLED
					}
					catch(...)
					{
						for(; dest < currentDest; ++dest)
							(*dest).~value_type();
						throw;
					}
				#endif

				return currentDest;
			}
		};

		template <typename InputIterator, typename Count, typename ForwardIterator>
		struct uninitialized_copy_n_impl<InputIterator, Count, ForwardIterator, eastl::random_access_iterator_tag>
		{
			static inline ForwardIterator impl(InputIterator first, Count n, ForwardIterator dest)
			{
				return eastl::uninitialized_copy(first, first + n, dest);
			}
		};
	}

	template<typename InputIterator, typename Count, typename ForwardIterator>
	inline ForwardIterator uninitialized_copy_n(InputIterator first, Count n, ForwardIterator dest)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;
		return Internal::uninitialized_copy_n_impl<InputIterator, Count, ForwardIterator, IC>::impl(first, n, dest);
	}



	/// uninitialized_copy_ptr
	///
	/// This is a specialization of uninitialized_copy for iterators that are pointers. We use it because
	/// internally it uses generic_iterator to make pointers act like regular eastl::iterator.
	///
	template <typename First, typename Last, typename Result>
	EASTL_REMOVE_AT_2024_SEPT inline Result uninitialized_copy_ptr(First first, Last last, Result result)
	{
		return eastl::uninitialized_copy(first, last, result);
	}



	/// uninitialized_move_ptr
	///
	/// This is a specialization of uninitialized_move for iterators that are pointers. We use it because
	/// internally it uses generic_iterator to make pointers act like regular eastl::iterator.
	///
	template <typename First, typename Last, typename Result>
	EASTL_REMOVE_AT_2024_SEPT inline Result uninitialized_move_ptr(First first, Last last, Result dest)
	{
		return uninitialized_move(first, last, dest);
	}




	/// uninitialized_move
	///
	/// Moves a source range to a destination, move-constructing the destination with
	/// the source values (and not *assigning* the destination with the source values).
	/// Returns the end of the destination range (i.e. dest + (last - first)).
	///
	/// Example usage:
	///    SomeClass* pArray = malloc(10 * sizeof(SomeClass));
	///    uninitialized_move(pSourceDataBegin, pSourceDataBegin + 10, pArray);
	///
	template <typename InputIterator, typename ForwardIterator>
	inline ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator dest)
	{
		return eastl::uninitialized_copy(eastl::make_move_iterator(first), eastl::make_move_iterator(last), dest);
	}


	/// uninitialized_move_if_noexcept
	///
	/// If the iterated type can be moved without exceptions, move construct the dest with the input. Else copy-construct
	/// the dest witih the input. If move isn't supported by the compiler, do regular copy.
	///
	template <typename InputIterator, typename ForwardIterator>
	inline ForwardIterator uninitialized_move_if_noexcept(InputIterator first, InputIterator last, ForwardIterator dest)
	{
#if EASTL_EXCEPTIONS_ENABLED
		return eastl::uninitialized_copy(eastl::make_move_if_noexcept_iterator(first), eastl::make_move_if_noexcept_iterator(last), dest);
#else
		return eastl::uninitialized_move(first, last, dest);
#endif
	}


	/// uninitialized_move_ptr_if_noexcept
	///
	template <typename First, typename Last, typename Result>
	EASTL_REMOVE_AT_2024_SEPT inline Result uninitialized_move_ptr_if_noexcept(First first, Last last, Result dest)
	{
		return eastl::uninitialized_move_if_noexcept(first, last, dest);
	}


	/// uninitialized_move_n
	///
	/// Moves count elements from a range beginning at first to an uninitialized memory area
	/// beginning at dest. The elements in the uninitialized area are constructed using copy constructor.
	/// If an exception is thrown during the initialization, the function has no final effects.
	///
	/// first:        Beginning of the range of the elements to move.
	/// dest:         Beginning of the destination range.
	/// return value: Iterator of dest type to the element past the last element moved.
	///
	template<typename InputIterator, typename Count, typename ForwardIterator>
	inline ForwardIterator uninitialized_move_n(InputIterator first, Count n, ForwardIterator dest)
	{
		return eastl::uninitialized_copy_n(eastl::make_move_iterator(first), n, dest);
	}

	/// uninitialized_default_construct
	///
	/// Constructs objects in the uninitialized storage designated by the range [first, last) by default-initialization.
	///
	/// Default-initialization:
	///  If T is a class, the default constructor is called; otherwise, no initialization is done, resulting in
	///  indeterminate values.
	///
	/// http://en.cppreference.com/w/cpp/memory/uninitialized_default_construct
	///
	template <typename ForwardIterator>
	inline void uninitialized_default_construct(ForwardIterator first, ForwardIterator last)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
		ForwardIterator currentDest(first);

	#if EASTL_EXCEPTIONS_ENABLED
		try
		{
	#endif
			for (; currentDest != last; ++currentDest)
				::new (eastl::addressof(*currentDest)) value_type;
	#if EASTL_EXCEPTIONS_ENABLED
		}
		catch (...)
		{
			for (; first < currentDest; ++first)
				(*first).~value_type();
			throw;
		}
	#endif
	}

	/// uninitialized_default_construct_n
	///
	/// Constructs n objects in the uninitialized storage starting at first by default-initialization.
	///
	/// http://en.cppreference.com/w/cpp/memory/uninitialized_default_construct_n
	///
	template <typename ForwardIterator, typename Count>
	inline ForwardIterator uninitialized_default_construct_n(ForwardIterator first, Count n)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
		ForwardIterator currentDest(first);

	#if EASTL_EXCEPTIONS_ENABLED
		try
		{
	#endif
			for (; n > 0; --n, ++currentDest)
				::new (eastl::addressof(*currentDest)) value_type;
			return currentDest;
	#if EASTL_EXCEPTIONS_ENABLED
		}
		catch (...)
		{
			for (; first < currentDest; ++first)
				(*first).~value_type();
			throw;
		}
	#endif
	}

	/// uninitialized_fill
	///
	/// Copy-constructs the elements in the destination range with the given input value.
	/// Returns void. It wouldn't be useful to return the end of the destination range,
	/// as that is the same as the 'last' input parameter.
	///
	/// Declaration:
	///    template <typename ForwardIterator, typename T>
	///    void uninitialized_fill(ForwardIterator destinationFirst, ForwardIterator destinationLast, const T& value);
	///
	namespace Internal
	{
		template <typename ForwardIterator, typename T>
		inline void uninitialized_fill_impl(ForwardIterator first, ForwardIterator last, const T& value, true_type)
		{
			eastl::fill(first, last, value);
		}

		template <typename ForwardIterator, typename T>
		void uninitialized_fill_impl(ForwardIterator first, ForwardIterator last, const T& value, false_type)
		{
			typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
			ForwardIterator currentDest(first);

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					for(; currentDest != last; ++currentDest)
						::new((void*)eastl::addressof(*currentDest)) value_type(value);
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					for(; first < currentDest; ++first)
						(*first).~value_type();
					throw;
				}
			#endif
		}
	}

	template <typename ForwardIterator, typename T>
	inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
		Internal::uninitialized_fill_impl(first, last, value, eastl::is_trivially_copy_assignable<value_type>());
	}

	/// uninitialized_value_construct
	///
	/// Constructs objects in the uninitialized storage range [first, last) by value-initialization.
	///
	/// Value-Initialization:
	/// If T is a class, the object is default-initialized (after being zero-initialized if T's default
	/// constructor is not user-provided/deleted); otherwise, the object is zero-initialized.
	///
	/// http://en.cppreference.com/w/cpp/memory/uninitialized_value_construct
	///
	template <class ForwardIterator>
	void uninitialized_value_construct(ForwardIterator first, ForwardIterator last)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
		ForwardIterator currentDest(first);

    #if EASTL_EXCEPTIONS_ENABLED
		try
		{
	#endif
			for (; currentDest != last; ++currentDest)
				::new (eastl::addressof(*currentDest)) value_type();
	#if EASTL_EXCEPTIONS_ENABLED
		}
		catch (...)
		{
			for (; first < currentDest; ++first)
				(*first).~value_type();
			throw;
		}
	#endif
	}

	/// uninitialized_value_construct_n
	///
	/// Constructs n objects in the uninitialized storage starting at first by value-initialization.
	///
	/// Value-Initialization:
	/// If T is a class, the object is default-initialized (after being zero-initialized if T's default
	/// constructor is not user-provided/deleted); otherwise, the object is zero-initialized.
	///
	/// http://en.cppreference.com/w/cpp/memory/uninitialized_value_construct_n
	///
	template <class ForwardIterator, class Count>
	ForwardIterator uninitialized_value_construct_n(ForwardIterator first, Count n)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
		ForwardIterator currentDest(first);

	#if EASTL_EXCEPTIONS_ENABLED
		try
		{
	#endif
			for (; n > 0; --n, ++currentDest)
				::new (eastl::addressof(*currentDest)) value_type();
			return currentDest;
	#if EASTL_EXCEPTIONS_ENABLED
		}
		catch (...)
		{
			for (; first < currentDest; ++first)
				(*first).~value_type();
			throw;
		}
	#endif
	}

	/// uninitialized_fill_ptr
	///
	/// This is a specialization of uninitialized_fill for iterators that are pointers.
	/// It exists so that we can declare a value_type for the iterator, which you
	/// can't do with a pointer by itself.
	///
	template <typename T>
	EASTL_REMOVE_AT_2024_SEPT inline void uninitialized_fill_ptr(T* first, T* last, const T& value)
	{
		uninitialized_fill(first, last, value);
	}

	/// uninitialized_fill_n
	///
	/// Copy-constructs the range of [first, first + n) with the given input value.
	/// Returns void as per the C++ standard, though returning the end input iterator
	/// value may be of use.
	///
	/// Declaration:
	///    template <typename ForwardIterator, typename Count, typename T>
	///    void uninitialized_fill_n(ForwardIterator destination, Count n, const T& value);
	///
	namespace Internal
	{
		template <typename ForwardIterator, typename Count, typename T>
		inline void uninitialized_fill_n_impl(ForwardIterator first, Count n, const T& value, true_type /* is_trivially_copy_assignable */)
		{
			// todo: implementation uses operator = but should instead be using the copy constructor, as documented.
			eastl::fill_n(first, n, value);
		}

		template <typename ForwardIterator, typename Count, typename T>
		void uninitialized_fill_n_impl(ForwardIterator first, Count n, const T& value, false_type /* is_trivially_copy_assignable */)
		{
			typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
			ForwardIterator currentDest(first);

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					for(; n > 0; --n, ++currentDest)
						::new((void*)eastl::addressof(*currentDest)) value_type(value);
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					for(; first < currentDest; ++first)
						(*first).~value_type();
					throw;
				}
			#endif
		}
	}

	template <typename ForwardIterator, typename Count, typename T>
	inline void uninitialized_fill_n(ForwardIterator first, Count n, const T& value)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
		Internal::uninitialized_fill_n_impl(first, n, value, eastl::is_trivially_copy_assignable<value_type>());
	}



	/// uninitialized_fill_n_ptr
	///
	/// This is a specialization of uninitialized_fill_n for iterators that are pointers.
	/// It exists so that we can declare a value_type for the iterator, which you
	/// can't do with a pointer by itself.
	///
	template <typename T, typename Count>
	EASTL_REMOVE_AT_2024_SEPT inline void uninitialized_fill_n_ptr(T* first, Count n, const T& value)
	{
		uninitialized_fill_n(first, n, value);
	}




	/// uninitialized_copy_fill
	///
	/// Copies [first1, last1) into [first2, first2 + (last1 - first1)) then
	/// fills [first2 + (last1 - first1), last2) with value.
	///
	template <typename InputIterator, typename ForwardIterator, typename T>
	inline void uninitialized_copy_fill(InputIterator first1, InputIterator last1,
										ForwardIterator first2, ForwardIterator last2, const T& value)
	{
		const ForwardIterator mid(eastl::uninitialized_copy(first1, last1, first2));

		#if EASTL_EXCEPTIONS_ENABLED
			typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
			try
			{
		#endif
				eastl::uninitialized_fill(mid, last2, value);
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				for(; first2 < mid; ++first2)
					(*first2).~value_type();
				throw;
			}
		#endif
	}


	/// uninitialized_move_fill
	///
	/// Moves [first1, last1) into [first2, first2 + (last1 - first1)) then
	/// fills [first2 + (last1 - first1), last2) with value.
	///
	template <typename InputIterator, typename ForwardIterator, typename T>
	inline void uninitialized_move_fill(InputIterator first1, InputIterator last1,
										ForwardIterator first2, ForwardIterator last2, const T& value)
	{
		const ForwardIterator mid(eastl::uninitialized_move(first1, last1, first2));

		#if EASTL_EXCEPTIONS_ENABLED
			typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
			try
			{
		#endif
				eastl::uninitialized_fill(mid, last2, value);
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				for(; first2 < mid; ++first2)
					(*first2).~value_type();
				throw;
			}
		#endif
	}





	/// uninitialized_fill_copy
	///
	/// Fills [result, mid) with value then copies [first, last) into [mid, mid + (last - first)).
	///
	template <typename ForwardIterator, typename T, typename InputIterator>
	inline ForwardIterator
	uninitialized_fill_copy(ForwardIterator result, ForwardIterator mid, const T& value, InputIterator first, InputIterator last)
	{
		eastl::uninitialized_fill(result, mid, value);

		#if EASTL_EXCEPTIONS_ENABLED
			typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
			try
			{
		#endif
				return eastl::uninitialized_copy(first, last, mid);
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				for(; result < mid; ++result)
					(*result).~value_type();
				throw;
			}
		#endif
	}


	/// uninitialized_fill_move
	///
	/// Fills [result, mid) with value then copies [first, last) into [mid, mid + (last - first)).
	///
	template <typename ForwardIterator, typename T, typename InputIterator>
	inline ForwardIterator
	uninitialized_fill_move(ForwardIterator result, ForwardIterator mid, const T& value, InputIterator first, InputIterator last)
	{
		eastl::uninitialized_fill(result, mid, value);

		#if EASTL_EXCEPTIONS_ENABLED
			typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
			try
			{
		#endif
				return eastl::uninitialized_move(first, last, mid);
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				for(; result < mid; ++result)
					(*result).~value_type();
				throw;
			}
		#endif
	}



	/// uninitialized_copy_copy
	///
	/// Copies [first1, last1) into [result, result + (last1 - first1)) then
	/// copies [first2, last2) into [result, result + (last1 - first1) + (last2 - first2)).
	///
	template <typename InputIterator1, typename InputIterator2, typename ForwardIterator>
	inline ForwardIterator
	uninitialized_copy_copy(InputIterator1 first1, InputIterator1 last1,
							InputIterator2 first2, InputIterator2 last2,
							ForwardIterator result)
	{
		const ForwardIterator mid(eastl::uninitialized_copy(first1, last1, result));

		#if EASTL_EXCEPTIONS_ENABLED
			typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
			try
			{
		#endif
				return eastl::uninitialized_copy(first2, last2, mid);
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				for(; result < mid; ++result)
					(*result).~value_type();
				throw;
			}
		#endif
	}



	/// destruct
	///
	/// Calls the destructor of a given object.
	///
	/// Note that we don't have a specialized version of this for objects
	/// with trivial destructors, such as integers. This is because the
	/// compiler can already see in our version here that the destructor
	/// is a no-op.
	///
	template <typename T>
	inline void destruct(T* p)
	{
		// https://msdn.microsoft.com/query/dev14.query?appId=Dev14IDEF1&l=EN-US&k=k(C4100)&rd=true
		// "C4100 can also be issued when code calls a destructor on a otherwise unreferenced parameter
		//  of primitive type. This is a limitation of the Visual C++ compiler."
		EA_UNUSED(p);
		p->~T();
	}



	// destruct(first, last)
	//
	template <typename ForwardIterator>
	inline void destruct_impl(ForwardIterator /*first*/, ForwardIterator /*last*/, true_type) // true means the type has a trivial destructor.
	{
		// Empty. The type has a trivial destructor.
	}

	template <typename ForwardIterator>
	inline void destruct_impl(ForwardIterator first, ForwardIterator last, false_type) // false means the type has a significant destructor.
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;

		for(; first != last; ++first)
			(*first).~value_type();
	}

	/// destruct
	///
	/// Calls the destructor on a range of objects.
	///
	/// We have a specialization for objects with trivial destructors, such as
	/// PODs. In this specialization the destruction of the range is a no-op.
	///
	template <typename ForwardIterator>
	inline void destruct(ForwardIterator first, ForwardIterator last)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
		destruct_impl(first, last, eastl::is_trivially_destructible<value_type>());
	}


	/// destroy_at
	///
	/// Calls the destructor of a given object.
	///
	/// Note that we don't have a specialized version of this for objects
	/// with trivial destructors, such as integers. This is because the
	/// compiler can already see in our version here that the destructor
	/// is a no-op.
	///
	/// This is the same as eastl::destruct but we included for C++17 compliance.
	///
	/// http://en.cppreference.com/w/cpp/memory/destroy_at
	///
	template <typename T>
	inline void destroy_at(T* p)
	{
		EA_UNUSED(p);
		p->~T();
	}


	/// destroy
	///
	/// Calls the destructor on a range of objects.
	///
	/// http://en.cppreference.com/w/cpp/memory/destroy
	///
	template <typename ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last)
	{
		for (; first != last; ++first)
			eastl::destroy_at(eastl::addressof(*first));
	}


	/// destroy_n
	///
	/// Calls the destructor on the n objects in the range.
	///
	/// http://en.cppreference.com/w/cpp/memory/destroy_n
	///
	template <typename ForwardIterator, typename Size>
	ForwardIterator destroy_n(ForwardIterator first, Size n)
	{
		for (; n > 0; ++first, --n)
			eastl::destroy_at(eastl::addressof(*first));

		return first;
	}


	/// align
	///
	/// Same as C++11 std::align. http://en.cppreference.com/w/cpp/memory/align
	/// If it is possible to fit size bytes of storage aligned by alignment into the buffer pointed to by
	/// ptr with length space, the function updates ptr to point to the first possible address of such storage,
	/// decreases space by the number of bytes used for alignment, and returns the new ptr value. Otherwise,
	/// the function returns NULL and leaves ptr and space unmodified.
	///
	/// Example usage:
	///     char   buffer[512];
	///     size_t space = sizeof(buffer);
	///     void*  p  = buffer;
	///     void*  p1 = eastl::align(16,  3, p, space); p = (char*)p +  3; space -=  3;
	///     void*  p2 = eastl::align(32, 78, p, space); p = (char*)p + 78; space -= 78;
	///     void*  p3 = eastl::align(64,  9, p, space); p = (char*)p +  9; space -=  9;

	inline void* align(size_t alignment, size_t size, void*& ptr, size_t& space)
	{
		if(space >= size)
		{
			char*  ptrAligned = (char*)(((size_t)ptr + (alignment - 1)) & -alignment);
			size_t offset     = (size_t)(ptrAligned - (char*)ptr);

			if((space - size) >= offset) // Have to implement this in terms of subtraction instead of addition in order to handle possible overflow.
			{
				ptr    = ptrAligned;
				space -= offset;

				return ptrAligned;
			}
		}

		return NULL;
	}


	/// align_advance
	///
	/// Same as align except ptr and space can be adjusted to reflect remaining space.
	/// Not present in the C++ Standard.
	/// Note that the example code here is similar to align but simpler.
	///
	/// Example usage:
	///     char   buffer[512];
	///     size_t space = sizeof(buffer);
	///     void*  p  = buffer;
	///     void*  p1 = eastl::align_advance(16,  3, p, space, &p, &space); // p is advanced and space reduced accordingly.
	///     void*  p2 = eastl::align_advance(32, 78, p, space, &p, &space);
	///     void*  p3 = eastl::align_advance(64,  9, p, space, &p, &space);
	///     void*  p4 = eastl::align_advance(16, 33, p, space);

	inline void* align_advance(size_t alignment, size_t size, void* ptr, size_t space, void** ptrAdvanced = NULL, size_t* spaceReduced = NULL)
	{
		if(space >= size)
		{
			char*  ptrAligned = (char*)(((size_t)ptr + (alignment - 1)) & -alignment);
			size_t offset     = (size_t)(ptrAligned - (char*)ptr);

			if((space - size) >= offset) // Have to implement this in terms of subtraction instead of addition in order to handle possible overflow.
			{
				if(ptrAdvanced)
					*ptrAdvanced = (ptrAligned + size);
				if(spaceReduced)
					*spaceReduced = (space - (offset + size));

				return ptrAligned;
			}
		}

		return NULL;
	}

	///////////////////////////////////////////////////////////////////////
	// pointer_traits
	//
	// C++11 Standard section 20.6.3
	// Provides information about a pointer type, mostly for the purpose
	// of handling the case where the pointer type isn't a built-in T* but
	// rather is a class that acts like a pointer.
	//
	// A user-defined Pointer has the following properties, by example:
	//     template <class T, class... MoreArgs>
	//     struct Pointer
	//     {
	//         typedef Pointer pointer;                         // required for use by pointer_traits.
	//         typedef T1      element_type;                    // optional for use by pointer_traits.
	//         typedef T2      difference_type;                 // optional for use by pointer_traits.
	//
	//         template <class Other>
	//         using rebind = typename Ptr<Other, MoreArgs...>; // optional for use by pointer_traits.
	//
	//         static pointer pointer_to(element_type& obj);    // required for use by pointer_traits.
	//     };
	//
	//
	// Example usage:
	//     template <typename Pointer>
	//     typename pointer_traits::element_type& GetElementPointedTo(Pointer p)
	//      { return *p; }
	//
	///////////////////////////////////////////////////////////////////////

	namespace Internal
	{
		// pointer_element_type
		template <typename Pointer>
		struct has_element_type // has_element_type<T>::value is true if T has an element_type member typedef.
		{
		private:
			template <typename U> static eastl::no_type  test(...);
			template <typename U> static eastl::yes_type test(typename U::element_type* = 0);
		public:
			static const bool value = sizeof(test<Pointer>(0)) == sizeof(eastl::yes_type);
		};

		template <typename Pointer, bool = has_element_type<Pointer>::value>
		struct pointer_element_type
		{
			using type = Pointer;
		};

		template <typename Pointer>
		struct pointer_element_type<Pointer, true>
			{ typedef typename Pointer::element_type type; };

		template <template <typename, typename...> class Pointer, typename T, typename... Args>
		struct pointer_element_type<Pointer<T, Args...>, false>
			{ typedef T type; };


		// pointer_difference_type
		template <typename Pointer>
		struct has_difference_type // has_difference_type<T>::value is true if T has an difference_type member typedef.
		{
		private:
			template <typename U> static eastl::no_type  test(...);
			template <typename U> static eastl::yes_type test(typename U::difference_type* = 0);
		public:
			static const bool value = sizeof((test<Pointer>(0))) == sizeof(eastl::yes_type);
		};

		template <typename Pointer, bool = has_difference_type<Pointer>::value>
		struct pointer_difference_type
			{ typedef typename Pointer::difference_type type; };

		template <typename Pointer>
		struct pointer_difference_type<Pointer, false>
			{ typedef ptrdiff_t type; };


		// pointer_rebind
		// The following isn't correct, as it is unilaterally requiring that Pointer typedef its
		// own rebind. We can fix this if needed to make it optional (in which case it would return
		// its own type), but we don't currently use rebind in EASTL (as we have a different allocator
		// system than the C++ Standard Library has) and this is currently moot.
		template <typename Pointer, typename U>
		struct pointer_rebind
		{
			typedef typename Pointer::template rebind<U> type;
		};


	} // namespace Internal


	template <typename Pointer>
	struct pointer_traits
	{
		typedef Pointer                                                   pointer;
		typedef typename Internal::pointer_element_type<pointer>::type    element_type;
		typedef typename Internal::pointer_difference_type<pointer>::type difference_type;

		#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
			template <typename U>
			struct rebind { typedef typename Internal::pointer_rebind<pointer, U>::type other; };
		#else
			template <typename U>
			using rebind = typename Internal::pointer_rebind<pointer, U>::type;
		#endif

	public:
		static pointer pointer_to(typename eastl::conditional<eastl::is_void<element_type>::value, void, element_type>::type& r) // 20.6.3.2: if element_type is (possibly cv-qualified) void, the type of r is unspecified; otherwise, it is T&.
			{ return pointer::pointer_to(r); } // The C++11 Standard requires that Pointer provides a static pointer_to function.
	};


	template <typename T>
	struct pointer_traits<T*>
	{
		typedef T*        pointer;
		typedef T         element_type;
		typedef ptrdiff_t difference_type;

		#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
			template <typename U>
			struct rebind { typedef U* other; };
		#else
			template <typename U>
			using rebind = U*;
		#endif

	public:
		static pointer pointer_to(typename eastl::conditional<eastl::is_void<element_type>::value, void, element_type>::type& r) EA_NOEXCEPT
			{ return eastl::addressof(r); } // 20.6.3.2: if element_type is (possibly cv-qualified) void, the type of r is unspecified; otherwise, it is T&.
	};

	///////////////////////////////////////////////////////////////////////
	// to_address
	//
	// Helper that call the customization point in pointer_traits<T>::to_address for retrieving the address of a pointer.
	// This is useful if you are using fancy-pointers.
	///////////////////////////////////////////////////////////////////////

	namespace Internal
	{
		template <class T>
		using detect_pointer_traits_to_address = decltype(eastl::pointer_traits<T>::to_address(eastl::declval<const T&>()));

		template <class T>
		using result_detect_pointer_traits_to_address = eastl::is_detected<detect_pointer_traits_to_address, T>;
	}

	template<class T>
	EA_CPP14_CONSTEXPR T* to_address(T* p) noexcept
	{
		static_assert(!eastl::is_function<T>::value, "Cannot call to_address with a function pointer. C++20 20.2.4.1 - Pointer conversion.");
		return p;
	}

	template <class Ptr, typename eastl::enable_if<Internal::result_detect_pointer_traits_to_address<Ptr>::value, int>::type = 0>
	EA_CPP14_CONSTEXPR auto to_address(const Ptr& ptr) noexcept -> decltype(eastl::pointer_traits<Ptr>::to_address(ptr))
	{
		return eastl::pointer_traits<Ptr>::to_address(ptr);
	}

	template <class Ptr, typename eastl::enable_if<!Internal::result_detect_pointer_traits_to_address<Ptr>::value, int>::type = 0>
	EA_CPP14_CONSTEXPR auto to_address(const Ptr& ptr) noexcept -> decltype(to_address(ptr.operator->()))
	{
		return to_address(ptr.operator->());
	}
} // namespace eastl


EA_RESTORE_VC_WARNING();


#endif // Header include guard

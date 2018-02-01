/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_FILL_HELP_H
#define EASTL_INTERNAL_FILL_HELP_H


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/config.h>

namespace eastl
{
	// fill
	//
	// We implement some fill helper functions in order to allow us to optimize it
	// where possible.
	//
	template <bool bIsScalar>
	struct fill_imp
	{
		template <typename ForwardIterator, typename T>
		static void do_fill(ForwardIterator first, ForwardIterator last, const T& value)
		{
			// The C++ standard doesn't specify whether we need to create a temporary
			// or not, but all std STL implementations are written like what we have here.
			for(; first != last; ++first)
				*first = value;
		}
	};

	template <>
	struct fill_imp<true>
	{
		template <typename ForwardIterator, typename T>
		static void do_fill(ForwardIterator first, ForwardIterator last, const T& value)
		{
			typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;
			// We create a temp and fill from that because value might alias to the 
			// destination range and so the compiler would be forced into generating 
			// less efficient code.
			for(const T temp = value; first != last; ++first)
			{
				EA_UNUSED(temp);
				*first = static_cast<value_type>(temp);
			}
		}
	};

	/// fill
	///
	/// fill is like memset in that it assigns a single value repeatedly to a 
	/// destination range. It allows for any type of iterator (not just an array)
	/// and the source value can be any type, not just a byte.
	/// Note that the source value (which is a reference) can come from within 
	/// the destination range.
	///
	/// Effects: Assigns value through all the iterators in the range [first, last).
	///
	/// Complexity: Exactly 'last - first' assignments.
	///
	/// Note: The C++ standard doesn't specify anything about the value parameter
	/// coming from within the first-last range. All std STL implementations act
	/// as if the standard specifies that value must not come from within this range.
	///
	template <typename ForwardIterator, typename T>
	inline void fill(ForwardIterator first, ForwardIterator last, const T& value)
	{
		eastl::fill_imp< is_scalar<T>::value >::do_fill(first, last, value);

		// Possibly better implementation, as it will deal with small PODs as well as scalars:
		// bEasyCopy is true if the type has a trivial constructor (e.g. is a POD) and if 
		// it is small. Thus any built-in type or any small user-defined struct will qualify.
		//const bool bEasyCopy = eastl::type_and<eastl::has_trivial_constructor<T>::value, 
		//                                       eastl::integral_constant<bool, (sizeof(T) <= 16)>::value;
		//eastl::fill_imp<bEasyCopy>::do_fill(first, last, value);

	}

	#if(defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) && (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64))
		#if defined(EA_PROCESSOR_X86_64)
			template <typename Value>
			inline void fill(uint64_t* first, uint64_t* last, Value c)
			{
				uintptr_t count = (uintptr_t)(last - first);
				uint64_t  value = (uint64_t)(c);

				__asm__ __volatile__ ("cld\n\t"
									  "rep stosq\n\t"
									   : "+c" (count), "+D" (first), "=m" (first)
									   : "a" (value)
									   : "cc" );
			}


			template <typename Value>
			inline void fill(int64_t* first, int64_t* last, Value c)
			{
				uintptr_t count = (uintptr_t)(last - first);
				int64_t   value = (int64_t)(c);

				__asm__ __volatile__ ("cld\n\t"
									  "rep stosq\n\t"
									   : "+c" (count), "+D" (first), "=m" (first)
									   : "a" (value)
									   : "cc" );
			}
		#endif

		template <typename Value>
		inline void fill(uint32_t* first, uint32_t* last, Value c)
		{
			uintptr_t count = (uintptr_t)(last - first);
			uint32_t  value = (uint32_t)(c);

			__asm__ __volatile__ ("cld\n\t"
								  "rep stosl\n\t"
								   : "+c" (count), "+D" (first), "=m" (first)
								   : "a" (value)
								   : "cc" );
		}


		template <typename Value>
		inline void fill(int32_t* first, int32_t* last, Value c)
		{
			uintptr_t count = (uintptr_t)(last - first);
			int32_t   value = (int32_t)(c);

			__asm__ __volatile__ ("cld\n\t"
								  "rep stosl\n\t"
								   : "+c" (count), "+D" (first), "=m" (first)
								   : "a" (value)
								   : "cc" );
		}


		template <typename Value>
		inline void fill(uint16_t* first, uint16_t* last, Value c)
		{
			uintptr_t count = (uintptr_t)(last - first);
			uint16_t  value = (uint16_t)(c);

			__asm__ __volatile__ ("cld\n\t"
								  "rep stosw\n\t"
								   : "+c" (count), "+D" (first), "=m" (first)
								   : "a" (value)
								   : "cc" );
		}


		template <typename Value>
		inline void fill(int16_t* first, int16_t* last, Value c)
		{
			uintptr_t count = (uintptr_t)(last - first);
			int16_t   value = (int16_t)(c);

			__asm__ __volatile__ ("cld\n\t"
								  "rep stosw\n\t"
								   : "+c" (count), "+D" (first), "=m" (first)
								   : "a" (value)
								   : "cc" );
		}

	#elif defined(EA_COMPILER_MICROSOFT) && (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64))
		#if defined(EA_PROCESSOR_X86_64)
			template <typename Value>
			inline void fill(uint64_t* first, uint64_t* last, Value c)
			{
				__stosq(first, (uint64_t)c, (size_t)(last - first));
			}

			template <typename Value>
			inline void fill(int64_t* first, int64_t* last, Value c)
			{
				__stosq((uint64_t*)first, (uint64_t)c, (size_t)(last - first));
			}
		#endif

		template <typename Value>
		inline void fill(uint32_t* first, uint32_t* last, Value c)
		{
			__stosd((unsigned long*)first, (unsigned long)c, (size_t)(last - first));
		}

		template <typename Value>
		inline void fill(int32_t* first, int32_t* last, Value c)
		{
			__stosd((unsigned long*)first, (unsigned long)c, (size_t)(last - first));
		}

		template <typename Value>
		inline void fill(uint16_t* first, uint16_t* last, Value c)
		{
			__stosw(first, (uint16_t)c, (size_t)(last - first));
		}

		template <typename Value>
		inline void fill(int16_t* first, int16_t* last, Value c)
		{
			__stosw((uint16_t*)first, (uint16_t)c, (size_t)(last - first));
		}
	#endif


	inline void fill(char* first, char* last, const char& c) // It's debateable whether we should use 'char& c' or 'char c' here.
	{
		memset(first, (unsigned char)c, (size_t)(last - first));
	}

	inline void fill(char* first, char* last, const int c) // This is used for cases like 'fill(first, last, 0)'.
	{
		memset(first, (unsigned char)c, (size_t)(last - first));
	}

	inline void fill(unsigned char* first, unsigned char* last, const unsigned char& c)
	{
		memset(first, (unsigned char)c, (size_t)(last - first));
	}

	inline void fill(unsigned char* first, unsigned char* last, const int c)
	{
		memset(first, (unsigned char)c, (size_t)(last - first));
	}

	inline void fill(signed char* first, signed char* last, const signed char& c)
	{
		memset(first, (unsigned char)c, (size_t)(last - first));
	}

	inline void fill(signed char* first, signed char* last, const int c)
	{
		memset(first, (unsigned char)c, (size_t)(last - first));
	}

	#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__ICL) // ICL = Intel compiler
		inline void fill(bool* first, bool* last, const bool& b)
		{
			memset(first, (char)b, (size_t)(last - first));
		}
	#endif




	// fill_n
	//
	// We implement some fill helper functions in order to allow us to optimize it
	// where possible.
	//
	template <bool bIsScalar>
	struct fill_n_imp
	{
		template <typename OutputIterator, typename Size, typename T>
		static OutputIterator do_fill(OutputIterator first, Size n, const T& value)
		{
			for(; n-- > 0; ++first)
				*first = value;
			return first;
		}
	};

	template <>
	struct fill_n_imp<true>
	{
		template <typename OutputIterator, typename Size, typename T>
		static OutputIterator do_fill(OutputIterator first, Size n, const T& value)
		{
			typedef typename eastl::iterator_traits<OutputIterator>::value_type value_type;

			// We create a temp and fill from that because value might alias to 
			// the destination range and so the compiler would be forced into 
			// generating less efficient code.
			for(const T temp = value; n-- > 0; ++first)
				*first = static_cast<value_type>(temp);
			return first;
		}
	};

	/// fill_n
	///
	/// The fill_n function is very much like memset in that a copies a source value
	/// n times into a destination range. The source value may come from within 
	/// the destination range.
	///
	/// Effects: Assigns value through all the iterators in the range [first, first + n).
	///
	/// Complexity: Exactly n assignments.
	///
	template <typename OutputIterator, typename Size, typename T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value)
	{
		return eastl::fill_n_imp<is_scalar<T>::value>::do_fill(first, n, value);
	}

	template <typename Size>
	inline char* fill_n(char* first, Size n, const char& c)
	{
		return (char*)memset(first, (char)c, (size_t)n) + n;
	}

	template <typename Size>
	inline unsigned char* fill_n(unsigned char* first, Size n, const unsigned char& c)
	{
		return (unsigned char*)memset(first, (unsigned char)c, (size_t)n) + n;
	}

	template <typename Size>
	inline signed char* fill_n(signed char* first, Size n, const signed char& c)
	{
		return (signed char*)memset(first, (signed char)c, n) + (size_t)n;
	}

	#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__ICL) // ICL = Intel compiler
		template <typename Size>
		inline bool* fill_n(bool* first, Size n, const bool& b)
		{
			return (bool*)memset(first, (char)b, n) + (size_t)n;
		}
	#endif

	#if(defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) && (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64))
		#if defined(EA_PROCESSOR_X86_64)
			template <typename Size, typename Value>
			inline uint64_t* fill_n(uint64_t* first, Size n, Value c)
			{
				uintptr_t count = (uintptr_t)(n);
				uint64_t  value = (uint64_t)(c);

				__asm__ __volatile__ ("cld\n\t"
									  "rep stosq\n\t"
									   : "+c" (count), "+D" (first), "=m" (first)
									   : "a" (value)
									   : "cc" );
				return first; // first is updated by the code above.
			}


			template <typename Size, typename Value>
			inline int64_t* fill_n(int64_t* first, Size n, Value c)
			{
				uintptr_t count = (uintptr_t)(n);
				int64_t   value = (int64_t)(c);

				__asm__ __volatile__ ("cld\n\t"
									  "rep stosq\n\t"
									   : "+c" (count), "+D" (first), "=m" (first)
									   : "a" (value)
									   : "cc" );
				return first; // first is updated by the code above.
			}
		#endif

		template <typename Size, typename Value>
		inline uint32_t* fill_n(uint32_t* first, Size n, Value c)
		{
			uintptr_t count = (uintptr_t)(n);
			uint32_t  value = (uint32_t)(c);

			__asm__ __volatile__ ("cld\n\t"
								  "rep stosl\n\t"
								   : "+c" (count), "+D" (first), "=m" (first)
								   : "a" (value)
								   : "cc" );
			return first; // first is updated by the code above.
		}


		template <typename Size, typename Value>
		inline int32_t* fill_n(int32_t* first, Size n, Value c)
		{
			uintptr_t count = (uintptr_t)(n);
			int32_t   value = (int32_t)(c);

			__asm__ __volatile__ ("cld\n\t"
								  "rep stosl\n\t"
								   : "+c" (count), "+D" (first), "=m" (first)
								   : "a" (value)
								   : "cc" );
			return first; // first is updated by the code above.
		}


		template <typename Size, typename Value>
		inline uint16_t* fill_n(uint16_t* first, Size n, Value c)
		{
			uintptr_t count = (uintptr_t)(n);
			uint16_t  value = (uint16_t)(c);

			__asm__ __volatile__ ("cld\n\t"
								  "rep stosw\n\t"
								   : "+c" (count), "+D" (first), "=m" (first)
								   : "a" (value)
								   : "cc" );
			return first; // first is updated by the code above.
		}


		template <typename Size, typename Value>
		inline int16_t* fill_n(int16_t* first, Size n, Value c)
		{
			uintptr_t count = (uintptr_t)(n);
			int16_t   value = (int16_t)(c);

			__asm__ __volatile__ ("cld\n\t"
								  "rep stosw\n\t"
								   : "+c" (count), "+D" (first), "=m" (first)
								   : "a" (value)
								   : "cc" );
			return first; // first is updated by the code above.
		}

	#elif defined(EA_COMPILER_MICROSOFT) && (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64))
		#if defined(EA_PROCESSOR_X86_64)
			template <typename Size, typename Value>
			inline uint64_t* fill_n(uint64_t* first, Size n, Value c)
			{
				__stosq(first, (uint64_t)c, (size_t)n);
				return first + n;
			}

			template <typename Size, typename Value>
			inline int64_t* fill_n(int64_t* first, Size n, Value c)
			{
				__stosq((uint64_t*)first, (uint64_t)c, (size_t)n);
				return first + n;
			}
		#endif

		template <typename Size, typename Value>
		inline uint32_t* fill_n(uint32_t* first, Size n, Value c)
		{
			__stosd((unsigned long*)first, (unsigned long)c, (size_t)n);
			return first + n;
		}

		template <typename Size, typename Value>
		inline int32_t* fill_n(int32_t* first, Size n, Value c)
		{
			__stosd((unsigned long*)first, (unsigned long)c, (size_t)n);
			return first + n;
		}

		template <typename Size, typename Value>
		inline uint16_t* fill_n(uint16_t* first, Size n, Value c)
		{
			__stosw(first, (uint16_t)c, (size_t)n);
			return first + n;
		}

		template <typename Size, typename Value>
		inline int16_t* fill_n(int16_t* first, Size n, Value c)
		{
			__stosw((uint16_t*)first, (uint16_t)c, (size_t)n);
			return first + n;
		}
	#endif

} // namespace eastl

#endif // Header include guard
















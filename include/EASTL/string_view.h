/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements the eastl::string_view which is part of the C++ standard
// STL library specification.
//
// http://en.cppreference.com/w/cpp/header/string_view
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_STRING_VIEW_H
#define EASTL_STRING_VIEW_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

#include <EASTL/internal/config.h>
#include <EASTL/internal/char_traits.h>
#include <EASTL/algorithm.h>
#include <EASTL/iterator.h>
#include <EASTL/numeric_limits.h>

#if EASTL_EXCEPTIONS_ENABLED
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <stdexcept> // std::out_of_range.
	EA_RESTORE_ALL_VC_WARNINGS()
#endif

EA_DISABLE_VC_WARNING(4814)

namespace eastl
{
	template <typename T>
	class basic_string_view
	{
	public:
		typedef basic_string_view<T>						this_type;
		typedef T 											value_type;
		typedef T* 											pointer;
		typedef const T* 									const_pointer;
		typedef T& 											reference;
		typedef const T& 									const_reference;
		typedef T* 											iterator;
		typedef const T* 									const_iterator;
		typedef eastl::reverse_iterator<iterator> 			reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator> 	const_reverse_iterator;
		typedef size_t 										size_type;
		typedef ptrdiff_t 									difference_type;

		static const EA_CONSTEXPR size_type npos = size_type(-1);

	protected:
		const_pointer mpBegin = nullptr;
		size_type mnCount = 0;

	public:
		// 21.4.2.1, construction and assignment
		EA_CONSTEXPR basic_string_view() EA_NOEXCEPT : mpBegin(nullptr), mnCount(0) {}
		EA_CONSTEXPR basic_string_view(const basic_string_view& other) EA_NOEXCEPT = default;
		EA_CONSTEXPR basic_string_view(const T* s, size_type count) : mpBegin(s), mnCount(count) {}
		EA_CONSTEXPR basic_string_view(const T* s) : mpBegin(s), mnCount(s != nullptr ? CharStrlen(s) : 0) {}
		basic_string_view& operator=(const basic_string_view& view) = default;

		// 21.4.2.2, iterator support
		EA_CONSTEXPR const_iterator begin() const EA_NOEXCEPT { return mpBegin; }
		EA_CONSTEXPR const_iterator cbegin() const EA_NOEXCEPT { return mpBegin; }
		EA_CONSTEXPR const_iterator end() const EA_NOEXCEPT { return mpBegin + mnCount; }
		EA_CONSTEXPR const_iterator cend() const EA_NOEXCEPT { return mpBegin + mnCount; }
		EA_CONSTEXPR const_reverse_iterator rbegin() const EA_NOEXCEPT { return const_reverse_iterator(mpBegin + mnCount); }
		EA_CONSTEXPR const_reverse_iterator crbegin() const EA_NOEXCEPT { return const_reverse_iterator(mpBegin + mnCount); }
		EA_CONSTEXPR const_reverse_iterator rend() const EA_NOEXCEPT { return const_reverse_iterator(mpBegin); }
		EA_CONSTEXPR const_reverse_iterator crend() const EA_NOEXCEPT { return const_reverse_iterator(mpBegin); }


		// 21.4.2.4, element access
		EA_CONSTEXPR const_pointer data() const { return mpBegin; }
		EA_CONSTEXPR const_reference front() const
		{
			return [&] { EASTL_ASSERT_MSG(!empty(), "behavior is undefined if string_view is empty"); }(), mpBegin[0];
		}

		EA_CONSTEXPR const_reference back() const
		{
			return [&] { EASTL_ASSERT_MSG(!empty(), "behavior is undefined if string_view is empty"); }(), mpBegin[mnCount - 1];
		}

		EA_CONSTEXPR const_reference operator[](size_type pos) const
		{
			// As per the standard spec: No bounds checking is performed: the behavior is undefined if pos >= size().
			return mpBegin[pos];
		}

		EA_CPP14_CONSTEXPR const_reference at(size_type pos) const
		{
			#if EASTL_EXCEPTIONS_ENABLED
				if(EASTL_UNLIKELY(pos >= mnCount))
					throw std::out_of_range("string_view::at -- out of range");
			#elif EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY(pos >= mnCount))
					EASTL_FAIL_MSG("string_view::at -- out of range");
			#endif

			return mpBegin[pos];
		}


		// 21.4.2.3, capacity
		EA_CONSTEXPR size_type size() const EA_NOEXCEPT { return mnCount; }
		EA_CONSTEXPR size_type length() const EA_NOEXCEPT { return mnCount; }

		// avoid macro expansion of max(...) from windows headers (potentially included before this file)
		// by wrapping function name in brackets
		EA_CONSTEXPR size_type max_size() const EA_NOEXCEPT { return (numeric_limits<size_type>::max)(); }
		EA_CONSTEXPR bool empty() const EA_NOEXCEPT { return mnCount == 0; }


		// 21.4.2.5, modifiers
		EA_CPP14_CONSTEXPR void swap(basic_string_view& v)
		{
			eastl::swap(mpBegin, v.mpBegin);
			eastl::swap(mnCount, v.mnCount);
		}

		EA_CPP14_CONSTEXPR void remove_prefix(size_type n)
		{
			EASTL_ASSERT_MSG(n <= mnCount, "behavior is undefined if moving past the end of the string");
			mpBegin += n;
			mnCount -= n;
		}

		EA_CPP14_CONSTEXPR void remove_suffix(size_type n)
		{
			EASTL_ASSERT_MSG(n <= mnCount, "behavior is undefined if moving past the beginning of the string");
			mnCount -= n;
		}


		// 21.4.2.6, string operations
		size_type copy(T* pDestination, size_type count, size_type pos = 0) const
		{
			#if EASTL_EXCEPTIONS_ENABLED
				if(EASTL_UNLIKELY(pos > mnCount))
					throw std::out_of_range("string_view::copy -- out of range");
			#elif EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY(pos > mnCount))
					EASTL_FAIL_MSG("string_view::copy -- out of range");
			#endif

			count = eastl::min<size_type>(count, mnCount - pos);
			auto* pResult = CharStringUninitializedCopy(mpBegin + pos, mpBegin + pos + count, pDestination);
			// *pResult = 0; // don't write the null-terminator
			return pResult - pDestination;
		}

		EA_CPP14_CONSTEXPR basic_string_view substr(size_type pos = 0, size_type count = npos) const
		{
			#if EASTL_EXCEPTIONS_ENABLED
				if(EASTL_UNLIKELY(pos > mnCount))
					throw std::out_of_range("string_view::substr -- out of range");
			#elif EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY(pos > mnCount))
					EASTL_FAIL_MSG("string_view::substr -- out of range");
			#endif

			count = eastl::min<size_type>(count, mnCount - pos);
			return this_type(mpBegin + pos, count);
		}

		static EA_CPP14_CONSTEXPR int compare(const T* pBegin1, const T* pEnd1, const T* pBegin2, const T* pEnd2)
		{
			const ptrdiff_t n1   = pEnd1 - pBegin1;
			const ptrdiff_t n2   = pEnd2 - pBegin2;
			const ptrdiff_t nMin = eastl::min_alt(n1, n2);
			const int       cmp  = Compare(pBegin1, pBegin2, (size_type)nMin);

			return (cmp != 0 ? cmp : (n1 < n2 ? -1 : (n1 > n2 ? 1 : 0)));
		}

		EA_CPP14_CONSTEXPR int compare(basic_string_view sw) const EA_NOEXCEPT
		{
			return compare(mpBegin, mpBegin + mnCount, sw.mpBegin, sw.mpBegin + sw.mnCount);
		}

		EA_CONSTEXPR int compare(size_type pos1, size_type count1, basic_string_view sw) const
		{
			return substr(pos1, count1).compare(sw);
		}

		EA_CONSTEXPR int compare(size_type pos1,
		                         size_type count1,
		                         basic_string_view sw,
		                         size_type pos2,
		                         size_type count2) const
		{
			return substr(pos1, count1).compare(sw.substr(pos2, count2));
		}

		EA_CONSTEXPR int compare(const T* s) const { return compare(basic_string_view(s)); }

		EA_CONSTEXPR int compare(size_type pos1, size_type count1, const T* s) const
		{
			return substr(pos1, count1).compare(basic_string_view(s));
		}

		EA_CONSTEXPR int compare(size_type pos1, size_type count1, const T* s, size_type count2) const
		{
			return substr(pos1, count1).compare(basic_string_view(s, count2));
		}

		EA_CPP14_CONSTEXPR size_type find(basic_string_view sw, size_type pos = 0) const EA_NOEXCEPT
		{
			auto* pEnd = mpBegin + mnCount;
			if (EASTL_LIKELY(((npos - sw.size()) >= pos) && (pos + sw.size()) <= mnCount))
			{
				const value_type* const pTemp = eastl::search(mpBegin + pos, pEnd, sw.data(), sw.data() + sw.size());

				if ((pTemp != pEnd) || (sw.size() == 0))
					return (size_type)(pTemp - mpBegin);
			}
			return npos;
		}

		EA_CONSTEXPR size_type find(T c, size_type pos = 0) const EA_NOEXCEPT
		{
			return find(basic_string_view(&c, 1), pos);
		}

		EA_CONSTEXPR size_type find(const T* s, size_type pos, size_type count) const
		{
			return find(basic_string_view(s, count), pos);
		}

		EA_CONSTEXPR size_type find(const T* s, size_type pos = 0) const { return find(basic_string_view(s), pos); }

		EA_CONSTEXPR size_type rfind(basic_string_view sw, size_type pos = npos) const EA_NOEXCEPT
		{
			return rfind(sw.mpBegin, pos, sw.mnCount);
		}

		EA_CPP14_CONSTEXPR size_type rfind(T c, size_type pos = npos) const EA_NOEXCEPT
		{
			if (EASTL_LIKELY(mnCount))
			{
				const value_type* const pEnd = mpBegin + eastl::min_alt(mnCount - 1, pos) + 1;
				const value_type* const pResult = CharTypeStringRFind(pEnd, mpBegin, c);

				if (pResult != mpBegin)
					return (size_type)((pResult - 1) - mpBegin);
			}
			return npos;
		}

		EA_CPP14_CONSTEXPR size_type rfind(const T* s, size_type pos, size_type n) const
		{
			// Disabled because it's not clear what values are valid for position.
			// It is documented that npos is a valid value, though. We return npos and
			// don't crash if postion is any invalid value.
			//#if EASTL_ASSERT_ENABLED
			//    if(EASTL_UNLIKELY((position != npos) && (position > (size_type)(mpEnd - mpBegin))))
			//        EASTL_FAIL_MSG("basic_string::rfind -- invalid position");
			//#endif

			// Note that a search for a zero length string starting at position = end() returns end() and not npos.
			// Note by Paul Pedriana: I am not sure how this should behave in the case of n == 0 and position > size.
			// The standard seems to suggest that rfind doesn't act exactly the same as find in that input position
			// can be > size and the return value can still be other than npos. Thus, if n == 0 then you can
			// never return npos, unlike the case with find.
			if (EASTL_LIKELY(n <= mnCount))
			{
				if (EASTL_LIKELY(n))
				{
					const const_iterator pEnd = mpBegin + eastl::min_alt(mnCount - n, pos) + n;
					const const_iterator pResult = CharTypeStringRSearch(mpBegin, pEnd, s, s + n);

					if (pResult != pEnd)
						return (size_type)(pResult - mpBegin);
				}
				else
					return eastl::min_alt(mnCount, pos);
			}
			return npos;
		}

		EA_CONSTEXPR size_type rfind(const T* s, size_type pos = npos) const
		{
			return rfind(s, pos, (size_type)CharStrlen(s));
		}

		EA_CONSTEXPR size_type find_first_of(basic_string_view sw, size_type pos = 0) const EA_NOEXCEPT
		{
			return find_first_of(sw.mpBegin, pos, sw.mnCount);
		}

		EA_CONSTEXPR size_type find_first_of(T c, size_type pos = 0) const EA_NOEXCEPT { return find(c, pos); }

		EA_CPP14_CONSTEXPR size_type find_first_of(const T* s, size_type pos, size_type n) const
		{
			// If position is >= size, we return npos.
			if (EASTL_LIKELY((pos < mnCount)))
			{
				const value_type* const pBegin = mpBegin + pos;
				const value_type* const pEnd = mpBegin + mnCount;
				const const_iterator pResult = CharTypeStringFindFirstOf(pBegin, pEnd, s, s + n);

				if (pResult != pEnd)
					return (size_type)(pResult - mpBegin);
			}
			return npos;
		}

		EA_CONSTEXPR size_type find_first_of(const T* s, size_type pos = 0) const
		{
			return find_first_of(s, pos, (size_type)CharStrlen(s));
		}

		EA_CONSTEXPR size_type find_last_of(basic_string_view sw, size_type pos = npos) const EA_NOEXCEPT
		{
			return find_last_of(sw.mpBegin, pos, sw.mnCount);
		}

		EA_CONSTEXPR size_type find_last_of(T c, size_type pos = npos) const EA_NOEXCEPT { return rfind(c, pos); }

		EA_CPP14_CONSTEXPR size_type find_last_of(const T* s, size_type pos, size_type n) const
		{
			// If n is zero or position is >= size, we return npos.
			if (EASTL_LIKELY(mnCount))
			{
				const value_type* const pEnd = mpBegin + eastl::min_alt(mnCount - 1, pos) + 1;
				const value_type* const pResult = CharTypeStringRFindFirstOf(pEnd, mpBegin, s, s + n);

				if (pResult != mpBegin)
					return (size_type)((pResult - 1) - mpBegin);
			}
			return npos;
		}

		EA_CONSTEXPR size_type find_last_of(const T* s, size_type pos = npos) const
		{
			return find_last_of(s, pos, (size_type)CharStrlen(s));
		}

		EA_CONSTEXPR size_type find_first_not_of(basic_string_view sw, size_type pos = 0) const EA_NOEXCEPT
		{
			return find_first_not_of(sw.mpBegin, pos, sw.mnCount);
		}

		EA_CPP14_CONSTEXPR size_type find_first_not_of(T c, size_type pos = 0) const EA_NOEXCEPT
		{
			if (EASTL_LIKELY(pos <= mnCount))
			{
				const auto pEnd = mpBegin + mnCount;
				// Todo: Possibly make a specialized version of CharTypeStringFindFirstNotOf(pBegin, pEnd, c).
				const const_iterator pResult = CharTypeStringFindFirstNotOf(mpBegin + pos, pEnd, &c, &c + 1);

				if (pResult != pEnd)
					return (size_type)(pResult - mpBegin);
			}
			return npos;
		}

		EA_CPP14_CONSTEXPR size_type find_first_not_of(const T* s, size_type pos, size_type n) const
		{
			if (EASTL_LIKELY(pos <= mnCount))
			{
				const auto pEnd = mpBegin + mnCount;
				const const_iterator pResult = CharTypeStringFindFirstNotOf(mpBegin + pos, pEnd, s, s + n);

				if (pResult != pEnd)
					return (size_type)(pResult - mpBegin);
			}
			return npos;
		}

		EA_CONSTEXPR size_type find_first_not_of(const T* s, size_type pos = 0) const
		{
			return find_first_not_of(s, pos, (size_type)CharStrlen(s));
		}

		EA_CONSTEXPR size_type find_last_not_of(basic_string_view sw, size_type pos = npos) const EA_NOEXCEPT
		{
			return find_last_not_of(sw.mpBegin, pos, sw.mnCount);
		}

		EA_CPP14_CONSTEXPR size_type find_last_not_of(T c, size_type pos = npos) const EA_NOEXCEPT
		{
			if (EASTL_LIKELY(mnCount))
			{
				// Todo: Possibly make a specialized version of CharTypeStringRFindFirstNotOf(pBegin, pEnd, c).
				const value_type* const pEnd = mpBegin + eastl::min_alt(mnCount - 1, pos) + 1;
				const value_type* const pResult = CharTypeStringRFindFirstNotOf(pEnd, mpBegin, &c, &c + 1);

				if (pResult != mpBegin)
					return (size_type)((pResult - 1) - mpBegin);
			}
			return npos;
		}

		EA_CPP14_CONSTEXPR size_type find_last_not_of(const T* s, size_type pos, size_type n) const
		{
			if (EASTL_LIKELY(mnCount))
			{
				const value_type* const pEnd = mpBegin + eastl::min_alt(mnCount - 1, pos) + 1;
				const value_type* const pResult = CharTypeStringRFindFirstNotOf(pEnd, mpBegin, s, s + n);

				if (pResult != mpBegin)
					return (size_type)((pResult - 1) - mpBegin);
			}
			return npos;
		}

		EA_CONSTEXPR size_type find_last_not_of(const T* s, size_type pos = npos) const
		{
			return find_last_not_of(s, pos, (size_type)CharStrlen(s));
		}

		// starts_with
		EA_CONSTEXPR bool starts_with(basic_string_view x) const EA_NOEXCEPT
		{
			return (size() >= x.size()) && (compare(0, x.size(), x) == 0);
		}

		EA_CONSTEXPR bool starts_with(T x) const EA_NOEXCEPT
		{
			return starts_with(basic_string_view(&x, 1));
		}

		EA_CONSTEXPR bool starts_with(const T* s) const
		{
			return starts_with(basic_string_view(s));
		}

		// ends_with
		EA_CONSTEXPR bool ends_with(basic_string_view x) const EA_NOEXCEPT
		{
			return (size() >= x.size()) && (compare(size() - x.size(), npos, x) == 0);
		}

		EA_CONSTEXPR bool ends_with(T x) const EA_NOEXCEPT
		{
			return ends_with(basic_string_view(&x, 1));
		}

		EA_CONSTEXPR bool ends_with(const T* s) const
		{
			return ends_with(basic_string_view(s));
		}
	};


	// global operators

	// Extra template parameter is to get around a known limitation in MSVC's ABI (name decoration)
	template <class CharT, int = 0>
	inline EA_CONSTEXPR bool operator==(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return (lhs.size() == rhs.size()) && (lhs.compare(rhs) == 0);
	}
	
	// type_identity_t is used in this context to forcefully trigger conversion operators towards basic_string_view.
	// Mostly we want basic_string::operator basic_string_view() to kick-in to be able to compare strings and string_views.
	template <class CharT, int = 1>
	inline EA_CONSTEXPR bool operator==(type_identity_t<basic_string_view<CharT>> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return (lhs.size() == rhs.size()) && (lhs.compare(rhs) == 0);
	}

	template <class CharT, int = 2>
	inline EA_CONSTEXPR bool operator==(basic_string_view<CharT> lhs, type_identity_t<basic_string_view<CharT>> rhs) EA_NOEXCEPT
	{
		return (lhs.size() == rhs.size()) && (lhs.compare(rhs) == 0);
	}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	template <class CharT>
	inline EA_CONSTEXPR auto operator<=>(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return static_cast<std::weak_ordering>(lhs.compare(rhs) <=> 0);
	}

	template <class CharT>
	inline EA_CONSTEXPR auto operator<=>(basic_string_view<CharT> lhs, typename basic_string_view<CharT>::const_pointer rhs) EA_NOEXCEPT
	{
		typedef basic_string_view<CharT> view_type;
		return static_cast<std::weak_ordering>(lhs <=> static_cast<view_type>(rhs));
	}

#else
	// Extra template parameter is to get around a known limitation in MSVC's ABI (name decoration)
	template <class CharT, int = 0>
	inline EA_CONSTEXPR bool operator!=(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return !(lhs == rhs);
	}
	
	template <class CharT, int = 1>
	inline EA_CONSTEXPR bool operator!=(type_identity_t<basic_string_view<CharT>> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return !(lhs == rhs);
	}

	template <class CharT>
	inline EA_CONSTEXPR bool operator!=(basic_string_view<CharT> lhs, type_identity_t<basic_string_view<CharT>> rhs) EA_NOEXCEPT
	{
		return !(lhs == rhs);
	}

	// Extra template parameter is to get around a known limitation in MSVC's ABI (name decoration)
	template <class CharT, int = 0>
	inline EA_CONSTEXPR bool operator<(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return lhs.compare(rhs) < 0;
	}
	
	template <class CharT, int = 1>
	inline EA_CONSTEXPR bool operator<(type_identity_t<basic_string_view<CharT>> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return lhs.compare(rhs) < 0;
	}

	template <class CharT>
	inline EA_CONSTEXPR bool operator<(basic_string_view<CharT> lhs, type_identity_t<basic_string_view<CharT>> rhs) EA_NOEXCEPT
	{
		return lhs.compare(rhs) < 0;
	}

	// Extra template parameter is to get around a known limitation in MSVC's ABI (name decoration)
	template <class CharT, int = 0>
	inline EA_CONSTEXPR bool operator<=(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return !(rhs < lhs);
	}
	
	template <class CharT, int = 1>
	inline EA_CONSTEXPR bool operator<=(type_identity_t<basic_string_view<CharT>> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return !(rhs < lhs);
	}

	template <class CharT>
	inline EA_CONSTEXPR bool operator<=(basic_string_view<CharT> lhs, type_identity_t<basic_string_view<CharT>> rhs) EA_NOEXCEPT
	{
		return !(rhs < lhs);
	}

	// Extra template parameter is to get around a known limitation in MSVC's ABI (name decoration)
	template <class CharT, int = 0>
	inline EA_CONSTEXPR bool operator>(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return rhs < lhs;
	}
	
	template <class CharT, int = 1>
	inline EA_CONSTEXPR bool operator>(type_identity_t<basic_string_view<CharT>> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return rhs < lhs;
	}

	template <class CharT>
	inline EA_CONSTEXPR bool operator>(basic_string_view<CharT> lhs, type_identity_t<basic_string_view<CharT>> rhs) EA_NOEXCEPT
	{
		return rhs < lhs;
	}

	// Extra template parameter is to get around a known limitation in MSVC's ABI (name decoration)
	template <class CharT, int = 0>
	inline EA_CONSTEXPR bool operator>=(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return !(lhs < rhs);
	}
	
	template <class CharT, int = 1>
	inline EA_CONSTEXPR bool operator>=(type_identity_t<basic_string_view<CharT>> lhs, basic_string_view<CharT> rhs) EA_NOEXCEPT
	{
		return !(lhs < rhs);
	}

	template <class CharT>
	inline EA_CONSTEXPR bool operator>=(basic_string_view<CharT> lhs, type_identity_t<basic_string_view<CharT>> rhs) EA_NOEXCEPT
	{
		return !(lhs < rhs);
	}
#endif
	// string_view / wstring_view 
	typedef basic_string_view<char>    string_view;
	typedef basic_string_view<wchar_t> wstring_view;

	// C++17 string types
	typedef basic_string_view<char8_t>  u8string_view;  // C++20 feature, but always present for consistency.
	typedef basic_string_view<char16_t> u16string_view;
	typedef basic_string_view<char32_t> u32string_view;


	/// hash<string_view>
	///
	/// We provide EASTL hash function objects for use in hash table containers.
	///
	/// Example usage:
	///    #include <EASTL/hash_set.h>
	///    hash_set<string_view> stringHashSet;
	///
	template <typename T> struct hash;

	template<> struct hash<string_view>
	{
		size_t operator()(const string_view& x) const
		{
			string_view::const_iterator p = x.cbegin();
			string_view::const_iterator end = x.cend();
			uint32_t result = 2166136261U; // We implement an FNV-like string hash.
			while (p != end)
				result = (result * 16777619) ^ (uint8_t)*p++;
			return (size_t)result;
		}
	};

	#if defined(EA_CHAR8_UNIQUE) && EA_CHAR8_UNIQUE
		template<> struct hash<u8string_view>
		{
			size_t operator()(const u8string_view& x) const
			{
				u8string_view::const_iterator p = x.cbegin();
				u8string_view::const_iterator end = x.cend();
				uint32_t result = 2166136261U;
				while (p != end)
					result = (result * 16777619) ^ (uint8_t)*p++;
				return (size_t)result;
			}
		};
	#endif

	template<> struct hash<u16string_view>
	{
		size_t operator()(const u16string_view& x) const
		{
			u16string_view::const_iterator p = x.cbegin();
			u16string_view::const_iterator end = x.cend();
			uint32_t result = 2166136261U;
			while (p != end)
				result = (result * 16777619) ^ (uint16_t)*p++;
			return (size_t)result;
		}
	};

	template<> struct hash<u32string_view>
	{
		size_t operator()(const u32string_view& x) const
		{
			u32string_view::const_iterator p = x.cbegin();
			u32string_view::const_iterator end = x.cend();
			uint32_t result = 2166136261U;
			while (p != end)
				result = (result * 16777619) ^ (uint32_t)*p++;
			return (size_t)result;
		}
	};

	#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
		template<> struct hash<wstring_view>
		{
			size_t operator()(const wstring_view& x) const
			{
				wstring_view::const_iterator p = x.cbegin();
				wstring_view::const_iterator end = x.cend();
				uint32_t result = 2166136261U;
				while (p != end)
					result = (result * 16777619) ^ (uint32_t)*p++;
				return (size_t)result;
			}
		};
	#endif


	#if EASTL_USER_LITERALS_ENABLED && EASTL_INLINE_NAMESPACES_ENABLED
		// Disabling the Clang/GCC/MSVC warning about using user
		// defined literals without a leading '_' as they are reserved
		// for standard libary usage.
		EA_DISABLE_VC_WARNING(4455)
		EA_DISABLE_CLANG_WARNING(-Wuser-defined-literals)
		EA_DISABLE_GCC_WARNING(-Wliteral-suffix)

	    inline namespace literals
	    {
		    inline namespace string_view_literals
		    {
			    EA_CONSTEXPR inline string_view operator "" sv(const char* str, size_t len) EA_NOEXCEPT { return {str, len}; }
			    EA_CONSTEXPR inline u16string_view operator "" sv(const char16_t* str, size_t len) EA_NOEXCEPT { return {str, len}; }
			    EA_CONSTEXPR inline u32string_view operator "" sv(const char32_t* str, size_t len) EA_NOEXCEPT { return {str, len}; }
			    EA_CONSTEXPR inline wstring_view operator "" sv(const wchar_t* str, size_t len) EA_NOEXCEPT { return {str, len}; }

				// We've seen _sv trigger the following warning on clang:
				// identifier '_sv' is reserved because it starts with '_' at global scope [-Wreserved-identifier]
				// Temporarily disable the warning until we figure out why it thinks _sv is "at global scope".
				EA_DISABLE_CLANG_WARNING(-Wreserved-identifier)
				// Backwards compatibility.
			    EA_CONSTEXPR inline string_view operator "" _sv(const char* str, size_t len) EA_NOEXCEPT { return {str, len}; }
			    EA_CONSTEXPR inline u16string_view operator "" _sv(const char16_t* str, size_t len) EA_NOEXCEPT { return {str, len}; }
			    EA_CONSTEXPR inline u32string_view operator "" _sv(const char32_t* str, size_t len) EA_NOEXCEPT { return {str, len}; }
			    EA_CONSTEXPR inline wstring_view operator "" _sv(const wchar_t* str, size_t len) EA_NOEXCEPT { return {str, len}; }
				EA_RESTORE_CLANG_WARNING()	// -Wreserved-identifier

				// C++20 char8_t support.
				#if EA_CHAR8_UNIQUE
					EA_CONSTEXPR inline u8string_view operator "" sv(const char8_t* str, size_t len) EA_NOEXCEPT { return {str, len}; }
					EA_CONSTEXPR inline u8string_view operator "" _sv(const char8_t* str, size_t len) EA_NOEXCEPT { return {str, len}; }
				#endif
		    }
	    }

		EA_RESTORE_GCC_WARNING()	// -Wliteral-suffix
		EA_RESTORE_CLANG_WARNING()	// -Wuser-defined-literals
		EA_RESTORE_VC_WARNING()		// warning: 4455
	#endif

} // namespace eastl

EA_RESTORE_VC_WARNING()
#endif // EASTL_STRING_VIEW_H

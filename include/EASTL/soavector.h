///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_SOAVECTOR_H
#define EASTL_SOAVECTOR_H

#include <EASTL/internal/config.h>
#include <EASTL/utility.h>

namespace eastl
{

// forward declarations
template <typename... Ts>
class soa_vector;

template <size_t I, typename SoaVector>
class soavec_element;

template <size_t I, typename SoaVector>
using soavec_element_t = typename soavec_element<I, SoaVector>::type;

namespace Internal
{
	template <typename Indices, typename... Ts>
	struct SoaVecImpl;
}

// soavec_element helper to be able to isolate a type given an index
template <typename... Ts>
struct SoaVecTypes
{
};

template <size_t I, typename T>
class soavec_element
{
};

// attempt to isolate type given an index
template <size_t I>
class soavec_element<I, soa_vector<>> // dcrooks-todo tuple implementation doesn't need this to be the literal soa_vector type, and is able to use "SoaVecTypes"
{
public:
	static_assert(I != I, "soavec_element index out of range");
};

template <typename H, typename... Ts>
class soavec_element<0, soa_vector<H, Ts...>>
{
public:
	typedef H type;
};

template <size_t I, typename H, typename... Ts>
class soavec_element<I, soa_vector<H, Ts...>>
{
public:
	typedef soavec_element_t<I - 1, soa_vector<Ts...>> type;
};

template <size_t I, typename Indices, typename... Ts>
class soavec_element<I, Internal::SoaVecImpl<Indices, Ts...>> : public soavec_element<I, soa_vector<Ts...>>
{
};

// attempt to isolate index given a type
template <typename T, typename SoaVector>
class soavec_index
{
};

template <typename T>
class soavec_index<T, soa_vector<>>
{
public:
	typedef void DuplicateTypeCheck;
};

template <typename T, typename... TsRest>
class soavec_index<T, soa_vector<T, TsRest...>>
{
public:
	typedef int DuplicateTypeCheck;
	static_assert(is_void<typename soavec_index<T, soa_vector<TsRest...>>::DuplicateTypeCheck>::value, "duplicate type T in soa_vector::get<T>(); unique types must be provided in declaration, or only use get<size_t>()");

	static const size_t index = 0;
};

template <typename T, typename Ts, typename... TsRest>
class soavec_index<T, soa_vector<Ts, TsRest...>>
{
public:
	static const size_t index = soavec_index<T, soa_vector<TsRest...>>::index + 1;
};

template <typename T, typename Indices, typename... Ts>
class soavec_index<T, Internal::SoaVecImpl<Indices, Ts...>> : public soavec_index<T, soa_vector<Ts...>>
{
};

namespace Internal
{

// SoaVecLeaf
template <size_t I, typename ValueType>
class SoaVecLeaf
{
public:
	SoaVecLeaf() : mVec() {}

	// swallowed functions requires non-void return types
	int push_back()
	{
		mVec.push_back();
		return 0;
	}

	int push_back(const ValueType& value)
	{
		mVec.push_back(value);
		return 0;
	}

	int push_back_uninitialized()
	{
		mVec.push_back_uninitialized();
		return 0;
	}

	size_t size() const
	{
		return mVec.size();
	}


	vector<ValueType>& getInternal() { return mVec; }
private:
	vector<ValueType> mVec;
};

// SoaVecImpl

// swallow allows for parameter pack expansion of arguments as means of expanding operations performed
template <typename... Ts>
void swallow(Ts&&...)
{
}


template <size_t... Indices, typename... Ts>
class SoaVecImpl<integer_sequence<size_t, Indices...>, Ts...> : public SoaVecLeaf<Indices, Ts>...
{
public:
	typedef soavec_element_t<0, soa_vector<Ts...>> BaseType;
	typedef SoaVecLeaf<0, BaseType> BaseLeaf;
	EA_CONSTEXPR SoaVecImpl() = default;

	// dcrooks-todo we should be conscious of what the vector sizes are here, and do a single resize on behalf of the soavecleaf's.
	// dcrooks-todo (at some point, should we even be wrapping vector functionality?)
	size_t push_back()
	{
		swallow(SoaVecLeaf<Indices, Ts>::push_back()...);
		return BaseLeaf::size() - 1;
	}
	void push_back(const Ts&... args)
	{
		swallow(SoaVecLeaf<Indices, Ts>::push_back(args)...);
	}
	void push_back_uninitialized()
	{
		swallow(SoaVecLeaf<Indices, Ts>::push_back_uninitialized()...);
	}

	size_t size() const
	{
		return BaseLeaf::size();
		// dcrooks-todo should assert that _all_ of the leaves are the same size. 
	}

};

template <size_t I, typename Indices, typename... Ts>
vector<soavec_element_t<I, SoaVecImpl<Indices, Ts...>>>& get(SoaVecImpl<Indices, Ts...>& t)
{
	typedef soavec_element_t<I, SoaVecImpl<Indices, Ts...>> Type;
	return static_cast<Internal::SoaVecLeaf<I, Type>&>(t).getInternal();
}

template <typename T, typename Indices, typename... Ts>
vector<T>& get(SoaVecImpl<Indices, Ts...>& t)
{
	typedef soavec_index<T, SoaVecImpl<Indices, Ts...>> Index;
	return static_cast<Internal::SoaVecLeaf<Index::index, T>&>(t).getInternal();
}


}  // namespace Internal


// External interface of soa_vector
template <typename... Ts>
class soa_vector
{
private:
	typedef Internal::SoaVecImpl<make_index_sequence<sizeof...(Ts)>, Ts...> Impl;

public:
	
	// soa_vector_element interface:
	// - created by fetching push_back, dereferencing iterator
	// - get<>() function which returns value&
	// internals:
	// - reference to soa_vec
	// - index [or store iterator to first vec and derive after the fact? eh, probably just index...]
	template <typename... Ts>
	struct element // dcrooks-todo maybe this'll just be iterator?
	{
	public:
		element(soa_vector<Ts...>& soaVector, size_t index = 0)
		: mIndex(index)
		, mSoaVector(soaVector)
		{	}

		template<size_t I>
		soavec_element_t<I, soa_vector<Ts...>>& get() { return mSoaVector.get<I>()[mIndex];	}

		template<typename T> 
		T& get() { return mSoaVector.get<T>()[mIndex]; }

		element& operator++()
		{
			++mIndex;
			return *this;
		}

		// dcrooks-todo need to compare vectors, too
		bool operator==(const element& other) const	{ return mIndex == other.mIndex; }
		bool operator!=(const element& other) const { return mIndex != other.mIndex; }
		element& operator*() { return *this; }

	private:
		size_t mIndex;
		soa_vector<Ts...> &mSoaVector;
	};

	typedef element<Ts...> element_type;
public:

	EA_CONSTEXPR soa_vector() = default;
	
	element_type push_back();
	void push_back(const Ts&... args);
	void push_back_uninitialized();

	size_t size();

	element_type begin();
	element_type end();
	
	template<size_t I>
	vector<soavec_element_t<I, soa_vector<Ts...>>>& get();

	template<typename T>
	vector<T>& get();

private:
	Impl mImpl;

};

template <typename... Ts>
typename soa_vector<Ts...>::element_type soa_vector<Ts...>::push_back()
{
	size_t newIndex = mImpl.push_back();
	return element<Ts...>((*this), newIndex);
}

template <typename... Ts>
void soa_vector<Ts...>::push_back(const Ts&... args)
{
	mImpl.push_back(args...);
}

template <typename... Ts>
void soa_vector<Ts...>::push_back_uninitialized()
{
	mImpl.push_back_uninitialized();
}

template <typename... Ts>
size_t soa_vector<Ts...>::size()
{
	return mImpl.size();
}

template <typename... Ts>
typename soa_vector<Ts...>::element_type soa_vector<Ts...>::begin()
{
	return element<Ts...>(*this, 0);
}

template <typename... Ts>
typename soa_vector<Ts...>::element_type soa_vector<Ts...>::end()
{
	return element<Ts...>(*this, size() - 1);
}

template <typename... Ts>
template<size_t I>
vector<eastl::soavec_element_t<I, soa_vector<Ts...>>>&
soa_vector<Ts...>::get()
{
	return Internal::get<I>(mImpl);
}


template <typename... Ts>
template<typename T>
eastl::vector<T>&
eastl::soa_vector<Ts...>::get()
{
	return Internal::get<T>(mImpl);
}



// Vector_Decl macros
#pragma region 
#define SOA_VECTOR_DECL_START(className, ... ) \
	class className : public soa_vector<##__VA_ARGS__> \
	{\
		public: \

#define SOA_VECTOR_DECL_TYPENAME(type, name, iter) vector<type>& name() { return get<iter>(); } \

#define SOA_VECTOR_DECL_END() };

#define SOA_VECTOR_DECL_2(className, type0, name0, type1, name1) \
	SOA_VECTOR_DECL_START(className, type0, type1) \
	SOA_VECTOR_DECL_TYPENAME(type0, name0, 0) \
	SOA_VECTOR_DECL_TYPENAME(type1, name1, 1) \
	SOA_VECTOR_DECL_END()

#define SOA_VECTOR_DECL_3(className, type0, name0, type1, name1, type2, name2) \
	SOA_VECTOR_DECL_START(className, type0, type1, type2) \
	SOA_VECTOR_DECL_TYPENAME(type0, name0, 0) \
	SOA_VECTOR_DECL_TYPENAME(type1, name1, 1) \
	SOA_VECTOR_DECL_TYPENAME(type2, name2, 2) \
	SOA_VECTOR_DECL_END()

#define SOA_VECTOR_DECL_4(className, type0, name0, type1, name1, type2, name2, type3, name3) \
	SOA_VECTOR_DECL_START(className, type0, type1, type2, type3) \
	SOA_VECTOR_DECL_TYPENAME(type0, name0, 0) \
	SOA_VECTOR_DECL_TYPENAME(type1, name1, 1) \
	SOA_VECTOR_DECL_TYPENAME(type2, name2, 2) \
	SOA_VECTOR_DECL_TYPENAME(type3, name3, 3) \
	SOA_VECTOR_DECL_END()
#pragma endregion

}  // namespace eastl

#endif  // EASTL_SOAVECTOR_H

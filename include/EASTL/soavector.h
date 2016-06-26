///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_SOAVECTOR_H
#define EASTL_SOAVECTOR_H

#include <EASTL/internal/config.h>
#include <EASTL/utility.h>

namespace eastl
{
// non-recursive soa_vector implementation

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

template <size_t I>
class soavec_element<I, soa_vector<>> // todo tuple implementation doesn't need this to be the literal soa_vector type, and is able to use "SoaVecTypes"
{
public:
	static_assert(I != I, "tuple_element index out of range");
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

namespace Internal
{

// SoaVecLeaf
template <size_t I, typename ValueType>
class SoaVecLeaf
{
public:
	SoaVecLeaf() : mVec() {}

	// swallow function requires non-void return types
	void* push_back()
	{
		mVec.push_back();
		return nullptr;
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
struct SoaVecImpl<integer_sequence<size_t, Indices...>, Ts...> : public SoaVecLeaf<Indices, Ts>...
{
public:
	EA_CONSTEXPR SoaVecImpl() = default;

	void push_back()
	{
		swallow(SoaVecLeaf<Indices, Ts>::push_back()...);
	}
};

template <size_t I, typename Indices, typename... Ts>
vector<soavec_element_t<I, SoaVecImpl<Indices, Ts...>>>& get(SoaVecImpl<Indices, Ts...>& t)
{
	typedef soavec_element_t<I, SoaVecImpl<Indices, Ts...>> Type;
	return static_cast<Internal::SoaVecLeaf<I, Type>&>(t).getInternal();
}

}  // namespace Internal


// External interface of soa_vector
template <typename... Ts>
class soa_vector
{
public:
	EA_CONSTEXPR soa_vector() = default;

	void push_back() { mImpl.push_back(); }
	
	template<size_t I>
	vector<soavec_element_t<I, soa_vector<Ts...>>>& get()
	{
		return Internal::get<I>(mImpl);
	}

private:
	typedef Internal::SoaVecImpl<make_index_sequence<sizeof...(Ts)>, Ts...> Impl;
	Impl mImpl;

};

}  // namespace eastl

#endif  // EASTL_SOAVECTOR_H

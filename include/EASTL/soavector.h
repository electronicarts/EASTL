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

namespace Internal
{

template <typename SoaVecIndices, typename... Ts>
struct SoaVecImpl;

// SoaVecLeaf
template <size_t I, typename ValueType>
class SoaVecLeaf
{
public:
	SoaVecLeaf() : mVec() {}

	void* push_back()
	{
		mVec.push_back();
		return nullptr;
	}
private:
	vector<ValueType> mVec;
};

// SoaVecImpl
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


}  // namespace Internal


// External interface of soa_vector
template <typename... Ts>
class soa_vector
{
public:
	EA_CONSTEXPR soa_vector() = default;

	void push_back() { mImpl.push_back(); }

private:
	typedef Internal::SoaVecImpl<make_index_sequence<sizeof...(Ts)>, Ts...> Impl;
	Impl mImpl;

};

}  // namespace eastl

#endif  // EASTL_TUPLE_H

///////////////////////////////////////////////////////////////////////////////
// eathread_atomic.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EATHREAD_EATHREAD_ATOMIC_H
#define EATHREAD_EATHREAD_ATOMIC_H

#include <atomic>

namespace EA {
namespace Thread {

	template <class T>
	class AtomicInt
	{
	public:
		typedef T ValueType;
		typedef AtomicInt<T> ThisType;

		AtomicInt() {}
		AtomicInt(ValueType n) : mValue(n) {} 
		AtomicInt(const ThisType& n) { mValue = n.mValue.load(); }
		ValueType GetValue() const { return mValue; }
		void SetValue(ValueType value) { mValue = value; }
		ValueType Add(ValueType value) { return (mValue += value); }
		AtomicInt& operator=(const ThisType& x) { mValue = x.GetValue(); return *this; }
		ValueType operator=(ValueType n) { mValue = n; return n; }

		operator const ValueType() const { return mValue; }

	private:
		std::atomic<ValueType> mValue;
	};

	typedef AtomicInt<int32_t> AtomicInt32;

}} // namespace EA::Thread


#endif  // EATHREAD_EATHREAD_ATOMIC_H

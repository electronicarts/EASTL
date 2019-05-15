// The purpose of this compilation unit is to test EABase in the absence of other system headers.
// For example TestEABase.cpp directly includes system headers like ctype.h, stddef.h, stdarg, etc.
// However, these headers make it impossible to verify that certain definitions are being provided
// by EABase instead of the system headers being included directly.

#include <EABase/eabase.h>

// This structure tests that EABase types are properly defined.
struct EABaseDefinedTypesStruct
{
	char8_t     mChar8_t;
	char16_t    mChar16_t;
	char32_t    mChar32_t;
	wchar_t     mWchar_t;
	bool8_t     mBool8_t;
	int8_t      mInt8_t;
	int16_t     mInt16_t;
	int32_t     mInt32_t;
	int64_t     mInt64_t;
	uint8_t     mUint8_t;
	uint16_t    mUint16_t;
	uint32_t    mUint32_t;
	uint64_t    mUint64_t;
	intmax_t    mIntmax_t;
	uintmax_t   mUintmax_t;
	size_t      mSize_t;
	ssize_t     mSsize_t;
	float_t     mFloat_t;
	double_t    mDouble_t;
	intptr_t    mIntptr_t;
	uintptr_t   mUintptr_t;
	ptrdiff_t   mPtrdiff_t;
};


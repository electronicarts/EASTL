/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_TYPE_TRANFORMATIONS_H
#define EASTL_INTERNAL_TYPE_TRANFORMATIONS_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <limits.h>


namespace eastl
{

	///////////////////////////////////////////////////////////////////////
	// add_const
	//
	// Add const to a type.
	//
	// Tor a given type T, add_const<T>::type is equivalent to T 
	// const if is_const<T>::value == false, and
	//    - is_void<T>::value == true, or
	//    - is_object<T>::value == true.
	//
	// Otherwise, add_const<T>::type is equivalent to T.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_add_const_CONFORMANCE 1    // add_const is conforming.   

	template <typename T, bool = eastl::is_const<T>::value || eastl::is_reference<T>::value || eastl::is_function<T>::value>
	struct add_const_helper
		{ typedef T type; };

	template <typename T>
	struct add_const_helper<T, false>
		{ typedef const T type; };

	template <typename T>
	struct  add_const
		{ typedef typename eastl::add_const_helper<T>::type type; };
	
	// add_const_t is the C++17 using typedef for typename add_const<T>::type.
	// We provide a backwards-compatible means to access it through a macro for pre-C++11 compilers.
	#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		#define EASTL_ADD_CONST_T(T) typename add_const<T>::type
	#else
		template <typename T>
		using add_const_t = typename add_const<T>::type;
		#define EASTL_ADD_CONST_T(T) add_const_t<T>
	#endif


	///////////////////////////////////////////////////////////////////////
	// add_volatile
	//
	// Add volatile to a type.
	// 
	// For a given type T, add_volatile<T>::type is equivalent to T volatile 
	// if is_volatile<T>::value == false, and
	//   - is_void<T>::value == true, or
	//   - is_object<T>::value == true.
	//
	// Otherwise, add_volatile<T>::type is equivalent to T.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_add_volatile_CONFORMANCE 1    // add_volatile is conforming.

	template <typename T, bool = eastl::is_volatile<T>::value || eastl::is_reference<T>::value || eastl::is_function<T>::value>
	struct add_volatile_helper
		{ typedef T type; };

	template <typename T>
	struct add_volatile_helper<T, false>
		{ typedef volatile T type; };

	template <typename T> struct add_volatile
		{ typedef typename eastl::add_volatile_helper<T>::type type; };

	template <class T> using add_volatile_t = typename add_volatile<T>::type;


    ///////////////////////////////////////////////////////////////////////
	// add_cv
	//
	// The add_cv transformation trait adds const and volatile qualification 
	// to the type to which it is applied. For a given type T, 
	// add_volatile<T>::type is equivalent to add_const<add_volatile<T>::type>::type.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_add_cv_CONFORMANCE 1    // add_cv is conforming.

	template<typename T>
	struct add_cv
	{
		typedef typename add_const<typename add_volatile<T>::type>::type type;
	};

	template <class T> using add_cv_t = typename add_cv<T>::type;


    ///////////////////////////////////////////////////////////////////////
	// make_signed
	//
	// Used to convert an integral type to its signed equivalent, if not already.
	// T shall be a (possibly const and/or volatile-qualified) integral type
	// or enumeration but not a bool type.;
	//
	// The user can define their own make_signed overrides for their own
	// types by making a template specialization like done below and adding
	// it to the user's code.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_make_signed_CONFORMANCE 1

	namespace internal
	{
		template <typename T, bool = eastl::is_enum_v<T> || eastl::is_integral_v<T>>
		struct make_signed_helper_0
		{
			struct char_helper
			{
				typedef signed char type;
			};

			struct short_helper
			{
				typedef signed short type;
			};

			struct int_helper
			{
				typedef signed int type;
			};

			struct long_helper
			{
				typedef signed long type;
			};

			struct longlong_helper
			{
				typedef signed long long type;
			};

			struct int128_helper
			{
				#if EASTL_GCC_STYLE_INT128_SUPPORTED
					typedef __int128_t type;
				#endif
			};

			struct no_type_helper
			{
			};

			typedef typename
			eastl::conditional<sizeof(T) <= sizeof(signed char), char_helper,
			eastl::conditional_t<sizeof(T) <= sizeof(signed short), short_helper,
			eastl::conditional_t<sizeof(T) <= sizeof(signed int), int_helper,
			eastl::conditional_t<sizeof(T) <= sizeof(signed long), long_helper,
			eastl::conditional_t<sizeof(T) <= sizeof(signed long long), longlong_helper,
			#if EASTL_GCC_STYLE_INT128_SUPPORTED
				eastl::conditional_t<sizeof(T) <= sizeof(__int128_t), int128_helper,
					no_type_helper
				>
			#else
				no_type_helper
			#endif
			>
			>
			>
			>
		    >::type type;
		};

		template <typename T>
		struct make_signed_helper_0<T, false>
		{
			struct no_type_helper
			{
			};

			typedef no_type_helper type;
		};

		template <typename T>
		struct make_signed_helper_1
		{
			typedef typename T::type type;
		};

		template <typename T>
		struct make_signed_helper
		{
			typedef typename eastl::internal::make_signed_helper_1<typename eastl::internal::make_signed_helper_0<T>::type>::type type;
		};

	} // namespace internal

	template <typename T>
	struct make_signed
	{
		typedef typename eastl::internal::make_signed_helper<T>::type type;
	};

	template <> struct make_signed<bool> {};
	template <> struct make_signed<signed char>              { typedef signed char            type; };
	template <> struct make_signed<unsigned char>            { typedef signed char            type; };
	template <> struct make_signed<signed short>             { typedef signed short           type; };
	template <> struct make_signed<unsigned short>           { typedef signed short           type; };
	template <> struct make_signed<signed int>               { typedef signed int             type; };
	template <> struct make_signed<unsigned int>             { typedef signed int             type; };
	template <> struct make_signed<signed long>              { typedef signed long            type; };
	template <> struct make_signed<unsigned long>            { typedef signed long            type; };
	template <> struct make_signed<signed long long>         { typedef signed long long       type; };
	template <> struct make_signed<unsigned long long>       { typedef signed long long       type; };
	#if EASTL_GCC_STYLE_INT128_SUPPORTED
		template <> struct make_signed<__int128_t>           { typedef __int128_t			  type; };
		template <> struct make_signed<__uint128_t>          { typedef __int128_t			  type; };
	#endif


	#if (defined(CHAR_MAX) && defined(UCHAR_MAX) && (CHAR_MAX == UCHAR_MAX)) // If char is unsigned, we convert char to signed char. However, if char is signed then make_signed returns char itself and not signed char.
		template <> struct make_signed<char>                 { typedef signed char            type; };
	#endif

	template <typename T>
	struct make_signed<const T>
	{
		typedef eastl::add_const_t<typename eastl::make_signed<T>::type> type;
	};

	template <typename T>
	struct make_signed<volatile T>
	{
		typedef eastl::add_volatile_t<typename eastl::make_signed<T>::type> type;
	};

	template <typename T>
	struct make_signed<const volatile T>
	{
		typedef eastl::add_cv_t<typename eastl::make_signed<T>::type> type;
	};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <typename T>
		using make_signed_t = typename make_signed<T>::type;
	#endif


	///////////////////////////////////////////////////////////////////////
	// add_signed
	//
	// This is not a C++11 type trait, and is here for backwards compatibility
	// only. Use the C++11 make_unsigned type trait instead.
	///////////////////////////////////////////////////////////////////////

	template<class T>
	struct add_signed : public make_signed<T>
	{ typedef typename eastl::make_signed<T>::type type; };




	///////////////////////////////////////////////////////////////////////
	// make_unsigned
	//
	// Used to convert an integral type to its unsigned equivalent, if not already.
	// T shall be a (possibly const and/or volatile-qualified) integral type
	// or enumeration but not a bool type.;
	//
	// The user can define their own make_unsigned overrides for their own
	// types by making a template specialization like done below and adding
	// it to the user's code.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_make_unsigned_CONFORMANCE 1

	namespace internal
	{

		template <typename T, bool = eastl::is_enum<T>::value || eastl::is_integral<T>::value>
		struct make_unsigned_helper_0
		{
			struct char_helper
			{
				typedef unsigned char type;
			};

			struct short_helper
			{
				typedef unsigned short type;
			};

			struct int_helper
			{
				typedef unsigned int type;
			};

			struct long_helper
			{
				typedef unsigned long type;
			};

			struct longlong_helper
			{
				typedef unsigned long long type;
			};

			struct int128_helper
			{
				#if EASTL_GCC_STYLE_INT128_SUPPORTED
					typedef __uint128_t type;
				#endif
			};

			struct no_type_helper
			{
			};


			typedef typename
			eastl::conditional<sizeof(T) <= sizeof(unsigned char), char_helper,
			eastl::conditional_t<sizeof(T) <= sizeof(unsigned short), short_helper,
			eastl::conditional_t<sizeof(T) <= sizeof(unsigned int), int_helper,
			eastl::conditional_t<sizeof(T) <= sizeof(unsigned long), long_helper,
			eastl::conditional_t<sizeof(T) <= sizeof(unsigned long long), longlong_helper,
			#if EASTL_GCC_STYLE_INT128_SUPPORTED
				eastl::conditional_t<sizeof(T) <= sizeof(__uint128_t), int128_helper,
					no_type_helper
				>
			#else
				 no_type_helper
			#endif
			  >
			  >
			  >
			  >
			  >::type type;
		};


		template <typename T>
		struct make_unsigned_helper_0<T, false>
		{
			struct no_type_helper
			{
			};

			typedef no_type_helper type;
		};

		template <typename T>
		struct make_unsigned_helper_1
		{
			typedef typename T::type type;
		};

		template <typename T>
		struct make_unsigned_helper
		{
			typedef typename eastl::internal::make_unsigned_helper_1<typename eastl::internal::make_unsigned_helper_0<T>::type>::type type;
		};

	} // namespace internal

	template <typename T>
	struct make_unsigned
	{
		typedef typename eastl::internal::make_unsigned_helper<T>::type type;
	};

	template <> struct make_unsigned<bool> {};
	template <> struct make_unsigned<signed char>            { typedef unsigned char            type; };
	template <> struct make_unsigned<unsigned char>          { typedef unsigned char            type; };
	template <> struct make_unsigned<signed short>           { typedef unsigned short           type; };
	template <> struct make_unsigned<unsigned short>         { typedef unsigned short           type; };
	template <> struct make_unsigned<signed int>             { typedef unsigned int             type; };
	template <> struct make_unsigned<unsigned int>           { typedef unsigned int             type; };
	template <> struct make_unsigned<signed long>            { typedef unsigned long            type; };
	template <> struct make_unsigned<unsigned long>          { typedef unsigned long            type; };
	template <> struct make_unsigned<signed long long>       { typedef unsigned long long       type; };
	template <> struct make_unsigned<unsigned long long>     { typedef unsigned long long       type; };
	#if EASTL_GCC_STYLE_INT128_SUPPORTED
		template <> struct make_unsigned<__int128_t>         { typedef __uint128_t				type; };
		template <> struct make_unsigned<__uint128_t>        { typedef __uint128_t				type; };
	#endif

	#if (CHAR_MIN < 0) // If char is signed, we convert char to unsigned char. However, if char is unsigned then make_unsigned returns char itself and not unsigned char.
		template <> struct make_unsigned<char>                 { typedef unsigned char          type; };
	#endif

	#if defined(EA_CHAR8_UNIQUE) && EA_CHAR8_UNIQUE
	template <> struct make_unsigned<char8_t>                 { typedef unsigned char           type; };
	#endif

	template <typename T>
	struct make_unsigned<const T>
	{
		typedef eastl::add_const_t<typename eastl::make_unsigned<T>::type> type;
	};

	template <typename T>
	struct make_unsigned<volatile T>
	{
		typedef eastl::add_volatile_t<typename eastl::make_unsigned<T>::type> type;
	};

	template <typename T>
	struct make_unsigned<const volatile T>
	{
		typedef eastl::add_cv_t<typename eastl::make_unsigned<T>::type> type;
	};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <typename T>
		using make_unsigned_t = typename make_unsigned<T>::type;
	#endif



	///////////////////////////////////////////////////////////////////////
	// add_unsigned
	//
	// This is not a C++11 type trait, and is here for backwards compatibility
	// only. Use the C++11 make_unsigned type trait instead.
	// 
	// Adds unsigned-ness to the given type. 
	// Modifies only integral values; has no effect on others.
	// add_unsigned<int>::type is unsigned int
	// add_unsigned<unsigned int>::type is unsigned int
	//
	///////////////////////////////////////////////////////////////////////

	template<class T>
	struct add_unsigned : public make_unsigned<T>
	{ typedef typename eastl::make_signed<T>::type type; };



	///////////////////////////////////////////////////////////////////////
	// remove_pointer
	//
	// Remove pointer from a type.
	//
	// The remove_pointer transformation trait removes top-level indirection 
	// by pointer (if any) from the type to which it is applied. Pointers to 
	// members are not affected. For a given type T, remove_pointer<T*>::type 
	// is equivalent to T.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_remove_pointer_CONFORMANCE 1

	template<typename T> struct remove_pointer                    { typedef T type; };
	template<typename T> struct remove_pointer<T*>                { typedef T type; };
	template<typename T> struct remove_pointer<T* const>          { typedef T type; };
	template<typename T> struct remove_pointer<T* volatile>       { typedef T type; };
	template<typename T> struct remove_pointer<T* const volatile> { typedef T type; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		using remove_pointer_t = typename remove_pointer<T>::type;
    #endif


	///////////////////////////////////////////////////////////////////////
	// add_pointer
	//
	// Add pointer to a type.
	// Provides the member typedef type which is the type T*. If T is a 
	// reference type, then type is a pointer to the referred type. 
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_add_pointer_CONFORMANCE 1

	template<class T>
	struct add_pointer { typedef typename eastl::remove_reference<T>::type* type; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		using add_pointer_t = typename add_pointer<T>::type;
    #endif



	///////////////////////////////////////////////////////////////////////
	// remove_extent
	//
	// The remove_extent transformation trait removes a dimension from an array.
	// For a given non-array type T, remove_extent<T>::type is equivalent to T.
	// For a given array type T[N], remove_extent<T[N]>::type is equivalent to T.
	// For a given array type const T[N], remove_extent<const T[N]>::type is equivalent to const T.
	// For example, given a multi-dimensional array type T[M][N], remove_extent<T[M][N]>::type is equivalent to T[N].
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_remove_extent_CONFORMANCE 1    // remove_extent is conforming.

	template<class T>           struct remove_extent       { typedef T type; };
	template<class T>           struct remove_extent<T[]>  { typedef T type; };
	template<class T, size_t N> struct remove_extent<T[N]> { typedef T type; };

	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template <typename T>
		using remove_extent_t = typename remove_extent<T>::type;
	#endif


	///////////////////////////////////////////////////////////////////////
	// remove_all_extents
	//
	// The remove_all_extents transformation trait removes all dimensions from an array.
	// For a given non-array type T, remove_all_extents<T>::type is equivalent to T.
	// For a given array type T[N], remove_all_extents<T[N]>::type is equivalent to T.
	// For a given array type const T[N], remove_all_extents<const T[N]>::type is equivalent to const T.
	// For example, given a multi-dimensional array type T[M][N], remove_all_extents<T[M][N]>::type is equivalent to T.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_remove_all_extents_CONFORMANCE 1    // remove_all_extents is conforming.

	template<typename T>           struct remove_all_extents       { typedef T type; };
	template<typename T, size_t N> struct remove_all_extents<T[N]> { typedef typename eastl::remove_all_extents<T>::type type; };
	template<typename T>           struct remove_all_extents<T[]>  { typedef typename eastl::remove_all_extents<T>::type type; };

	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template <typename T>
		using remove_all_extents_t = typename remove_all_extents<T>::type;
	#endif



	///////////////////////////////////////////////////////////////////////
	// aligned_storage
	//
	// The aligned_storage transformation trait provides a type that is 
	// suitably aligned to store an object whose size is does not exceed length 
	// and whose alignment is a divisor of alignment. When using aligned_storage, 
	// length must be non-zero, and alignment must >= alignment_of<T>::value 
	// for some type T. We require the alignment value to be a power-of-two.
	//
	// GCC versions prior to 4.4 don't properly support this with stack-based
	// variables. The EABase EA_ALIGN_MAX_AUTOMATIC define identifies the 
	// extent to which stack (automatic) variables can be aligned for the 
	// given compiler/platform combination.
	//
	// Example usage:
	//     aligned_storage<sizeof(Widget), alignment_of(Widget)>::type widget;
	//     Widget* pWidget = new(&widget) Widget;
	//
	//     aligned_storage<sizeof(Widget), 64>::type widgetAlignedTo64;
	//     Widget* pWidget = new(&widgetAlignedTo64) Widget;
	//
	//     aligned_storage<sizeof(Widget), alignment_of(Widget)>::type widgetArray[37];
	//     Widget* pWidgetArray = new(widgetArray) Widget[37];
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_aligned_storage_CONFORMANCE 1    // aligned_storage is conforming.

	#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4008)
		// New versions of GCC do not support using 'alignas' with a value greater than 128.
		// However, this code using the GNU standard alignment attribute works properly.
		template<size_t N, size_t Align = EASTL_ALIGN_OF(double)>
		struct aligned_storage
		{
			struct type { unsigned char mCharData[N]; } EA_ALIGN(Align);
		};
	#elif (EABASE_VERSION_N >= 20040) && !defined(EA_COMPILER_NO_ALIGNAS) // If C++11 alignas is supported...
		template<size_t N, size_t Align = EASTL_ALIGN_OF(double)>
		struct aligned_storage
		{
			typedef struct {
				alignas(Align) unsigned char mCharData[N];
			} type;
		};

	#elif defined(EA_COMPILER_MSVC) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION < 4007)) || defined(EA_COMPILER_EDG) // At some point GCC fixed their attribute(align) to support non-literals, though it's not clear what version aside from being no later than 4.7 and no earlier than 4.2.
		// Some compilers don't allow you to to use EA_ALIGNED with anything by a numeric literal, 
		// so we can't use the simpler code like we do further below for other compilers. We support
		// only up to so much of an alignment value here.
		template<size_t N, size_t Align>
		struct aligned_storage_helper { struct type{ unsigned char mCharData[N]; }; };

		template<size_t N> struct aligned_storage_helper<N,    2> { struct EA_ALIGN(   2) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N,    4> { struct EA_ALIGN(   4) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N,    8> { struct EA_ALIGN(   8) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N,   16> { struct EA_ALIGN(  16) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N,   32> { struct EA_ALIGN(  32) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N,   64> { struct EA_ALIGN(  64) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N,  128> { struct EA_ALIGN( 128) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N,  256> { struct EA_ALIGN( 256) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N,  512> { struct EA_ALIGN( 512) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N, 1024> { struct EA_ALIGN(1024) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N, 2048> { struct EA_ALIGN(2048) type{ unsigned char mCharData[N]; }; };
		template<size_t N> struct aligned_storage_helper<N, 4096> { struct EA_ALIGN(4096) type{ unsigned char mCharData[N]; }; };

		template<size_t N, size_t Align = EASTL_ALIGN_OF(double)>
		struct aligned_storage
		{
			typedef typename aligned_storage_helper<N, Align>::type type;
		};

	#else
		template<size_t N, size_t Align = EASTL_ALIGN_OF(double)>
		struct aligned_storage
		{
			union type
			{
				unsigned char mCharData[N];
				struct EA_ALIGN(Align) mStruct{ }; 
			};
		};
	#endif

	#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		#define EASTL_ALIGNED_STORAGE_T(N, Align) typename eastl::aligned_storage_t<N, Align>::type
	#else
		template <size_t N, size_t Align = EASTL_ALIGN_OF(double)>
		using aligned_storage_t = typename aligned_storage<N, Align>::type;
		#define EASTL_ALIGNED_STORAGE_T(N, Align) eastl::aligned_storage_t<N, Align>
	#endif



	///////////////////////////////////////////////////////////////////////
	// aligned_union
	//
	// The member typedef type shall be a POD type suitable for use as
	// uninitialized storage for any object whose type is listed in Types; 
	// its size shall be at least Len. The static member alignment_value 
	// shall be an integral constant of type std::size_t whose value is 
	// the strictest alignment of all types listed in Types.
	// Note that the resulting type is not a C/C++ union, but simply memory 
	// block (of pod type) that can be used to placement-new an actual 
	// C/C++ union of the types. The actual union you declare can be a non-POD union.
	//
	// Example usage:
	//     union MyUnion {
	//         char  c;
	//         int   i;
	//         float f;
	//
	//         MyUnion(float fValue) : f(fValue) {}
	//     };
	// 
	//     aligned_union<sizeof(MyUnion), char, int, float>::type myUnionStorage;
	//     MyUnion* pMyUnion = new(&myUnionStorage) MyUnion(21.4f);
	//     pMyUnion->i = 37;
	//
	///////////////////////////////////////////////////////////////////////

	#if defined(EA_COMPILER_NO_VARIADIC_TEMPLATES) || !EASTL_TYPE_TRAIT_static_max_CONFORMANCE
		#define EASTL_TYPE_TRAIT_aligned_union_CONFORMANCE 0    // aligned_union is not conforming, as it supports only a two-member unions.

		// To consider: Expand this to include more possible types. We may want to convert this to be a recursive 
		//              template instead of like below.
		template <size_t minSize, typename Type0, typename Type1 = char, typename Type2 = char, typename Type3 = char>
		struct aligned_union
		{
			static const size_t size0           = eastl::static_max<minSize, sizeof(Type0)>::value;
			static const size_t size1           = eastl::static_max<size0,   sizeof(Type1)>::value;
			static const size_t size2           = eastl::static_max<size1,   sizeof(Type2)>::value;
			static const size_t size            = eastl::static_max<size2,   sizeof(Type3)>::value;

			static const size_t alignment0      = eastl::static_max<EA_ALIGN_OF(Type0), EA_ALIGN_OF(Type1)>::value;
			static const size_t alignment1      = eastl::static_max<alignment0,         EA_ALIGN_OF(Type2)>::value;
			static const size_t alignment_value = eastl::static_max<alignment1,         EA_ALIGN_OF(Type3)>::value;

			typedef typename eastl::aligned_storage<size, alignment_value>::type type;
		};

		#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
			// To do: define macro.
		#else
			template <size_t minSize, typename Type0, typename Type1 = char, typename Type2 = char, typename Type3 = char>
			using aligned_union_t = typename aligned_union<minSize, Type0, Type1, Type2, Type3>::type;
		#endif
	#else
		#define EASTL_TYPE_TRAIT_aligned_union_CONFORMANCE 1    // aligned_union is conforming.

		template <size_t minSize, typename Type0, typename ...TypeN>
		struct aligned_union
		{
			static const size_t size            = eastl::static_max<minSize, sizeof(Type0), sizeof(TypeN)...>::value;
			static const size_t alignment_value = eastl::static_max<EA_ALIGN_OF(Type0), EA_ALIGN_OF(TypeN)...>::value;

			typedef typename eastl::aligned_storage<size, alignment_value>::type type;
		};

		#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
			// To do: define macro.
		#else
			template <size_t minSize, typename... TypeN>
			using aligned_union_t = typename aligned_union<minSize, TypeN...>::type;
        #endif

	#endif


	///////////////////////////////////////////////////////////////////////
	// union_cast
	//
	// Safely converts between unrelated types that have a binary equivalency.
	// This appoach is required by strictly conforming C++ compilers because
	// directly using a C or C++ cast between unrelated types is fraught with 
	// the possibility of undefined runtime behavior due to type aliasing.
	// The Source and Dest types must be POD types due to the use of a union 
	// in C++ versions prior to C++11. C++11 relaxes the definition of a POD
	// such that it allows a classes with trivial default constructors whereas 
	// previous versions did not, so beware of this when writing portable code.
	//
	// Example usage:
	//    float f32 = 1.234f;
	//    uint32_t n32 = union_cast<uint32_t>(f32);
	//
	// Example possible mis-usage:
	// The following is valid only if you are aliasing the pointer value and 
	// not what it points to. Most of the time the user intends the latter, 
	// which isn't strictly possible.
	//    Widget* pWidget = CreateWidget();
	//    Foo*    pFoo    = union_cast<Foo*>(pWidget);
	///////////////////////////////////////////////////////////////////////

	template <typename DestType, typename SourceType>
	DestType union_cast(SourceType sourceValue)
	{
		EASTL_CT_ASSERT((sizeof(DestType) == sizeof(SourceType)) && 
						(EA_ALIGN_OF(DestType) == EA_ALIGN_OF(SourceType)));               // To support differening alignments, we would need to use a memcpy-based solution or find a way to make the two union members align with each other.
		//EASTL_CT_ASSERT(is_pod<DestType>::value && is_pod<SourceType>::value);           // Disabled because we don't want to restrict what the user can do, as some compiler's definitions of is_pod aren't up to C++11 Standards.
		//EASTL_CT_ASSERT(!is_pointer<DestType>::value && !is_pointer<SourceType>::value); // Disabled because it's valid to alias pointers as long as you are aliasong the pointer value and not what it points to.

		union {
			SourceType sourceValue;
			DestType   destValue;
		} u;
		u.sourceValue = sourceValue;

		return u.destValue;
	}

} // namespace eastl


#endif // Header include guard






















/*-----------------------------------------------------------------------------
 * nullptr.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *---------------------------------------------------------------------------*/


#include <EABase/eabase.h>
#include <EABase/eahave.h>


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once /* Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result. */
#endif


#if defined(EA_COMPILER_CPP11_ENABLED) && !defined(EA_COMPILER_NO_NULLPTR) && !defined(EA_HAVE_nullptr_t_IMPL)
	// The compiler supports nullptr, but the standard library doesn't implement a declaration for std::nullptr_t. So we provide one.
	namespace std { typedef decltype(nullptr) nullptr_t; }
#endif



#if defined(EA_COMPILER_NO_NULLPTR) // If the compiler lacks a native version... 

	namespace std
	{
		class nullptr_t
		{
		public:
			template<class T>               // When tested a pointer, acts as 0.
			operator T*() const
				{ return 0; }
		 
			template<class C, class T>      // When tested as a member pointer, acts as 0.
			operator T C::*() const
				{ return 0; }

			typedef void* (nullptr_t::*bool_)() const;
			operator bool_() const          // An rvalue of type std::nullptr_t can be converted to an rvalue of type bool; the resulting value is false.
				{ return false; }           // We can't use operator bool(){ return false; } because bool is convertable to int which breaks other required functionality.

			// We can't enable this without generating warnings about nullptr being uninitialized after being used when created without "= {}".
			//void* mSizeofVoidPtr;         // sizeof(nullptr_t) == sizeof(void*). Needs to be public if nullptr_t is to be a POD.

		private:
			void operator&() const;         // Address cannot be taken.
		};

		inline nullptr_t nullptr_get()
		{
			nullptr_t n = { };              // std::nullptr exists.
			return n;
		}

		#if !defined(nullptr) // If somebody hasn't already defined nullptr in a custom way...
			#define nullptr nullptr_get()
		#endif

	} // namespace std


	template<class T>
	inline bool operator==(T* p, const std::nullptr_t)
	{ return p == 0; }

	template<class T>
	inline bool operator==(const std::nullptr_t, T* p)
	{ return p == 0; }

	template<class T, class U>
	inline bool operator==(T U::* p, const std::nullptr_t)
	{ return p == 0; }

	template<class T, class U>
	inline bool operator==(const std::nullptr_t, T U::* p)
	{ return p == 0; }

	inline bool operator==(const std::nullptr_t, const std::nullptr_t)
	{ return true; }

	inline bool operator!=(const std::nullptr_t, const std::nullptr_t)
	{ return false; }

	inline bool operator<(const std::nullptr_t, const std::nullptr_t)
	{ return false; }

	inline bool operator>(const std::nullptr_t, const std::nullptr_t)
	{ return false; }

	inline bool operator<=(const std::nullptr_t, const std::nullptr_t)
	{ return true; }

	inline bool operator>=(const std::nullptr_t, const std::nullptr_t)
	{ return true; }


	using std::nullptr_t;   // exported to global namespace.
	using std::nullptr_get; // exported to global namespace.

#endif // EA_COMPILER_NO_NULLPTR


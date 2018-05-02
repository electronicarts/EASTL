/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Implements an EASTL allocator that uses an ICoreAllocator.
// However, this header file is not dependent on ICoreAllocator or its package.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_CORE_ALLOCATOR_ADAPTER_H
#define EASTL_CORE_ALLOCATOR_ADAPTER_H

#if EASTL_CORE_ALLOCATOR_ENABLED


#include <EASTL/internal/config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif


/// EASTL_CORE_ALLOCATOR_ADAPTER_GET_DEFAULT_CORE_ALLOCATOR
///
/// This allows the application to override the default name for the default global core allocator.
/// However, you must be careful in your usage of this, as if this file is shared between uses then 
/// you will need to be careful that your override of this doesn't conflict with others. 
///
#ifndef EASTL_CORE_ALLOCATOR_ADAPTER_GET_DEFAULT_CORE_ALLOCATOR
	#define EASTL_CORE_ALLOCATOR_ADAPTER_GET_DEFAULT_CORE_ALLOCATOR AllocatorType::GetDefaultAllocator
#endif



namespace EA
{
	namespace Allocator
	{
		/// CoreAllocatorAdapter
		///
		/// Implements the EASTL allocator interface.
		/// Allocates memory from an instance of ICoreAllocator or another class with an equivalent interface.
		/// ICoreAllocator is a pure-virtual memory allocation interface used by a number of EA games and 
		/// shared libraries. It's completely unrelated to EASTL, but it's prevalent enough that it's useful
		/// for EASTL to have a built-in adapter for this interface. ICoreAllocator is declared in the 
		/// CoreAllocator package icoreallocator_interface.h header, but CoreAllocatorAdapter can work with
		/// any equivalent interface, as defined below.
		///
		/// Expected interface:
		///     enum AllocFlags {
		///         kFlagTempMemory = 0,
		///         kFlagPermMemory = 1
		///     };
		///     
		///     struct CoreAllocator {
		///         void* Alloc(size_t size, const char* name, unsigned int allocFlags);
		///         void* Alloc(size_t size, const char* name, unsigned int allocFlags,     // Not required unless you are working with types that require custom alignment.
		///                      unsigned int align, unsigned int alignOffset = 0);
		///         void Free(void* block, size_t size = 0);
		///         static CoreAllocator* GetDefaultAllocator();
		///     };
		///
		/// Example usage:
		///     #include <coreallocator/icoreallocator_interface.h>
		///     typedef EA::Allocator::CoreAllocatorAdapter<EASTLTestCoreAllocator> Adapter;
		///     eastl::list<Widget, Adapter> widgetList(Adapter("UI/WidgetList", pSomeCoreAllocator));
		///     widgetList.push_back(Widget());
		///
		/// Example usage:
		///     #include <MyEquivalentCoreAllocatorInterface.h>
		///     eastl::list<Widget, CoreAllocatorAdapter<MyCoreAllocatorInterface> > widgetList;
		///     widgetList.push_back(Widget());
		///
		/// Example usage:
		///     #include <coreallocator/icoreallocator_interface.h>
		///     typedef EA::Allocator::CoreAllocatorAdapter<EASTLTestCoreAllocator> Adapter;
		///     typedef eastl::list<Widget, Adapter> WidgetList;
		///     CoreAllocatorFixed<WidgetList::node_type> widgetCoreAllocator(pFixedAllocatorForWidgetListValueType); // CoreAllocatorFixed is a hypothetical implementation of the ICoreAllocator interface.
		///     WidgetList widgetList(Adapter("UI/WidgetList", &widgetCoreAllocator));                                // Note that the widgetCoreAllocator is declared before and thus destroyed after the widget list.
		///
		template<class AllocatorType>
		class CoreAllocatorAdapter
		{
		public:
			typedef CoreAllocatorAdapter<AllocatorType> this_type;

		public:
			// To do: Make this constructor explicit, when there is no known code dependent on it being otherwise.
			CoreAllocatorAdapter(const char* pName = EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME), AllocatorType* pAllocator = EASTL_CORE_ALLOCATOR_ADAPTER_GET_DEFAULT_CORE_ALLOCATOR());
			CoreAllocatorAdapter(const char* pName, AllocatorType* pAllocator, int flags);
			CoreAllocatorAdapter(const CoreAllocatorAdapter& x);
			CoreAllocatorAdapter(const CoreAllocatorAdapter& x, const char* pName);

			CoreAllocatorAdapter& operator=(const CoreAllocatorAdapter& x);

			void* allocate(size_t n, int flags = 0);
			void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);
			void  deallocate(void* p, size_t n);

			AllocatorType* get_allocator() const;
			void           set_allocator(AllocatorType* pAllocator);

			int  get_flags() const;
			void set_flags(int flags);

			const char* get_name() const;
			void        set_name(const char* pName);

		public: // Public because otherwise VC++ generates (possibly invalid) warnings about inline friend template specializations.
			AllocatorType* mpCoreAllocator;
			int            mnFlags;    // Allocation flags. See ICoreAllocator/AllocFlags.

			#if EASTL_NAME_ENABLED
				const char* mpName; // Debug name, used to track memory.
			#endif
		};

		template<class AllocatorType>
		bool operator==(const CoreAllocatorAdapter<AllocatorType>& a, const CoreAllocatorAdapter<AllocatorType>& b);

		template<class AllocatorType>
		bool operator!=(const CoreAllocatorAdapter<AllocatorType>& a, const CoreAllocatorAdapter<AllocatorType>& b);



		/// EASTLICoreAllocator
		///
		/// Provides a standardized typedef for ICoreAllocator;
		/// 
		/// Example usage:
		///     eastl::list<Widget, EASTLICoreAllocator> widgetList("UI/WidgetList", pSomeCoreAllocator);
		///     widgetList.push_back(Widget());
		///
		class ICoreAllocator;
		class EASTLCoreAllocatorImpl;

		typedef CoreAllocatorAdapter<ICoreAllocator> EASTLICoreAllocatorAdapter;
		typedef CoreAllocatorAdapter<EASTLCoreAllocatorImpl> EASTLCoreAllocatorAdapter;
		typedef EASTLICoreAllocatorAdapter EASTLICoreAllocator;  // for backwards compatibility



		/// EASTLICoreDeleter
		///
		/// Implements a functor which can free memory from the specified
		/// ICoreAllocator interface.  This is a convenience object provided for
		/// users who wish to have EASTL containers deallocate memory obtained from
		/// ICoreAllocator interfaces.
		///
		template <class AllocatorType>
		class CoreDeleterAdapter
		{
		public:
			typedef CoreDeleterAdapter<AllocatorType> this_type;
			AllocatorType* mpCoreAllocator;

		public:
			CoreDeleterAdapter(AllocatorType* pAllocator = EASTL_CORE_ALLOCATOR_ADAPTER_GET_DEFAULT_CORE_ALLOCATOR()) EA_NOEXCEPT 
			: mpCoreAllocator(pAllocator) {}

			~CoreDeleterAdapter() EA_NOEXCEPT {}

			template <typename T>
			void operator()(T* p) { mpCoreAllocator->Free(p); }

			CoreDeleterAdapter(const CoreDeleterAdapter& in) { mpCoreAllocator = in.mpCoreAllocator; }

		#if EASTL_MOVE_SEMANTICS_ENABLED
			CoreDeleterAdapter(CoreDeleterAdapter&& in)
			{
				mpCoreAllocator = in.mpCoreAllocator;
				in.mpCoreAllocator = nullptr;
			}

			CoreDeleterAdapter& operator=(const CoreDeleterAdapter& in)
			{
				mpCoreAllocator = in.mpCoreAllocator;
				return *this;
			}

			CoreDeleterAdapter& operator=(CoreDeleterAdapter&& in)
			{
				mpCoreAllocator = in.mpCoreAllocator;
				in.mpCoreAllocator = nullptr;
				return *this;
			}
		#endif

		};



		/// EASTLICoreDeleter
		///
		/// Provides a standardized typedef for ICoreAllocator implementations.
		///
		/// Example usage: 
		///     eastl::shared_ptr<A> foo(pA, EASTLCoreDeleter());
		///
		typedef CoreDeleterAdapter<ICoreAllocator> EASTLICoreDeleterAdapter;
		typedef CoreDeleterAdapter<EASTLCoreAllocatorImpl> EASTLCoreDeleterAdapter;

	} // namespace Allocator

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
	namespace Allocator
	{
		template<class AllocatorType>
		inline CoreAllocatorAdapter<AllocatorType>::CoreAllocatorAdapter(const char* EASTL_NAME(pName), AllocatorType* pCoreAllocator)
			: mpCoreAllocator(pCoreAllocator), mnFlags(0)
		{
			#if EASTL_NAME_ENABLED
				mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
			#endif
		}

		template<class AllocatorType>
		inline CoreAllocatorAdapter<AllocatorType>::CoreAllocatorAdapter(const char* EASTL_NAME(pName), AllocatorType* pCoreAllocator, int flags)
			: mpCoreAllocator(pCoreAllocator), mnFlags(flags)
		{
			#if EASTL_NAME_ENABLED
				mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
			#endif
		}

		template<class AllocatorType>
		inline CoreAllocatorAdapter<AllocatorType>::CoreAllocatorAdapter(const CoreAllocatorAdapter& x)
			: mpCoreAllocator(x.mpCoreAllocator), mnFlags(x.mnFlags)
		{
			#if EASTL_NAME_ENABLED
				mpName = x.mpName;
			#endif
		}

		template<class AllocatorType>
		inline CoreAllocatorAdapter<AllocatorType>::CoreAllocatorAdapter(const CoreAllocatorAdapter& x, const char* EASTL_NAME(pName))
			: mpCoreAllocator(x.mpCoreAllocator), mnFlags(x.mnFlags)
		{
			#if EASTL_NAME_ENABLED
				mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
			#endif
		}

		template<class AllocatorType>
		inline CoreAllocatorAdapter<AllocatorType>& CoreAllocatorAdapter<AllocatorType>::operator=(const CoreAllocatorAdapter& x)
		{
			mpCoreAllocator = x.mpCoreAllocator;
			mnFlags         = x.mnFlags;

			#if EASTL_NAME_ENABLED
				mpName = x.mpName;
			#endif
				
			return *this;
		}

		template<class AllocatorType>
		inline void* CoreAllocatorAdapter<AllocatorType>::allocate(size_t n, int /*flags*/)
		{
			// It turns out that EASTL itself doesn't use the flags parameter, 
			// whereas the user here might well want to specify a flags 
			// parameter. So we use ours instead of the one passed in.
			return mpCoreAllocator->Alloc(n, EASTL_NAME_VAL(mpName), (unsigned)mnFlags);
		}

		template<class AllocatorType>
		inline void* CoreAllocatorAdapter<AllocatorType>::allocate(size_t n, size_t alignment, size_t offset, int /*flags*/)
		{
			// It turns out that EASTL itself doesn't use the flags parameter, 
			// whereas the user here might well want to specify a flags 
			// parameter. So we use ours instead of the one passed in.
			return mpCoreAllocator->Alloc(n, EASTL_NAME_VAL(mpName), (unsigned)mnFlags, (unsigned)alignment, (unsigned)offset);
		}

		template<class AllocatorType>
		inline void CoreAllocatorAdapter<AllocatorType>::deallocate(void* p, size_t n)
		{
			return mpCoreAllocator->Free(p, n);
		}

		template<class AllocatorType>
		inline AllocatorType* CoreAllocatorAdapter<AllocatorType>::get_allocator() const
		{
			return mpCoreAllocator;
		}

		template<class AllocatorType>
		inline void CoreAllocatorAdapter<AllocatorType>::set_allocator(AllocatorType* pAllocator)
		{
			mpCoreAllocator = pAllocator;
		}

		template<class AllocatorType>
		inline int CoreAllocatorAdapter<AllocatorType>::get_flags() const
		{
			return mnFlags;
		}

		template<class AllocatorType>
		inline void CoreAllocatorAdapter<AllocatorType>::set_flags(int flags)
		{
			mnFlags = flags;
		}

		template<class AllocatorType>
		inline const char* CoreAllocatorAdapter<AllocatorType>::get_name() const
		{
			#if EASTL_NAME_ENABLED
				return mpName;
			#else
				return EASTL_ALLOCATOR_DEFAULT_NAME;
			#endif
		}

		template<class AllocatorType>
		inline void CoreAllocatorAdapter<AllocatorType>::set_name(const char* pName)
		{
			#if EASTL_NAME_ENABLED
				mpName = pName;
			#else
				(void)pName;
			#endif
		}



		template<class AllocatorType>
		inline bool operator==(const CoreAllocatorAdapter<AllocatorType>& a, const CoreAllocatorAdapter<AllocatorType>& b)
		{
			return (a.mpCoreAllocator == b.mpCoreAllocator) &&
				   (a.mnFlags         == b.mnFlags);
		}

		template<class AllocatorType>
		inline bool operator!=(const CoreAllocatorAdapter<AllocatorType>& a, const CoreAllocatorAdapter<AllocatorType>& b)
		{
			return (a.mpCoreAllocator != b.mpCoreAllocator) ||
				   (a.mnFlags         != b.mnFlags);
		}


	} // namespace Allocator

} // namespace EA


#endif // EASTL_CORE_ALLOCATOR_ENABLED
#endif // Header include guard









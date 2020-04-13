/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EASTL/bitvector.h>
#include <EASTL/vector.h>
#include <EASTL/deque.h>
#include <EASTL/string.h>




// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::bitvector<>;
template class eastl::bitvector<MallocAllocator>;
template class eastl::bitvector<EASTLAllocatorType, uint8_t>;
template class eastl::bitvector<EASTLAllocatorType, int16_t>;
template class eastl::bitvector<EASTLAllocatorType, int32_t>;
template class eastl::bitvector<EASTLAllocatorType, int64_t, eastl::vector<int64_t, EASTLAllocatorType> >;

// bitvector doesn't yet support deque.
//template class eastl::bitvector<EASTLAllocatorType, uint8_t, eastl::deque<uint64_t, EASTLAllocatorType> >;
//template class eastl::bitvector<EASTLAllocatorType, uint8_t, eastl::deque<int32_t, EASTLAllocatorType, 64> >;



int TestBitVector()
{
	using namespace eastl;

	int nErrorCount = 0;

	{
		// typedef bitvector<Allocator, Element>           this_type;
		// typedef bool                                    value_type;
		// typedef bitvector_reference<Element>            reference;
		// typedef bool                                    const_reference;
		// typedef bitvector_iterator<Element>             iterator;
		// typedef bitvector_const_iterator<Element>       const_iterator;
		// typedef eastl::reverse_iterator<iterator>       reverse_iterator;
		// typedef eastl::reverse_iterator<const_iterator> const_reverse_iterator;
		// typedef Allocator                               allocator_type;
		// typedef Element                                 element_type;
		// typedef Container                               container_type;
		// typedef eastl_size_t                            size_type;

		bitvector<>::this_type              this_typeVariable;
		bitvector<>::value_type             value_typeVariable = 0;
		bitvector<>::const_reference        const_referenceVariable(false);
		bitvector<>::iterator               iteratorVariable(NULL, 0);
		bitvector<>::const_iterator         const_iteratorVariable(NULL, 0);
		bitvector<>::reverse_iterator       reverse_iteratorVariable(iteratorVariable);
		bitvector<>::const_reverse_iterator const_reverse_iteratorVariable(const_iteratorVariable);
		bitvector<>::allocator_type         allocator_typeVariable;
		bitvector<>::element_type           element_typeVariable = 0;
		bitvector<>::container_type         container_typeVariable;
		bitvector<>::size_type              size_typeVariable = 0;

		string sAddresses(string::CtorSprintf(), "%p %p %p %p %p %p %p %p %p %p %p", 
						   &this_typeVariable, &value_typeVariable, &const_referenceVariable, &iteratorVariable,
						   &const_iteratorVariable, &reverse_iteratorVariable,&const_reverse_iteratorVariable,
						   &allocator_typeVariable, &element_typeVariable, &container_typeVariable, &size_typeVariable);
		EATEST_VERIFY(sAddresses.size() > 0); 
	}

	{
		// bitvector();
		// explicit bitvector(const allocator_type& allocator);
		// explicit bitvector(size_type n, const allocator_type& allocator = EASTL_BITVECTOR_DEFAULT_ALLOCATOR);
		// bitvector(size_type n, value_type value, const allocator_type& allocator = EASTL_BITVECTOR_DEFAULT_ALLOCATOR);
		// bitvector(const bitvector& copy);
		// template <typename InputIterator> bitvector(InputIterator first, InputIterator last);
		// bitvector& operator=(const bitvector& x);
		// reference       operator[](size_type n);            // behavior is undefined if n is invalid.
		// const_reference operator[](size_type n) const;
		MallocAllocator              mallocAllocator;
		bitvector<>                  bv0;
		bitvector<MallocAllocator>   bv1(mallocAllocator);
		bitvector<>                  bv2(200);
		bitvector<>                  bv3(300, true);
		bitvector<MallocAllocator>   bv4(400, false, mallocAllocator);
		const bitvector<>            bv5(bv2);
		bool                         boolArray[] = { true, false, true };
		bitvector<>                  bv6(boolArray, boolArray + EAArrayCount(boolArray));
		bitvector<>                  bv7(bv3.begin(), bv3.end());
  
		{
			// Validate the above constructions
			EATEST_VERIFY(bv0.validate());
			EATEST_VERIFY(bv0.empty());

			EATEST_VERIFY(bv1.validate());
			EATEST_VERIFY(bv1.empty());

			EATEST_VERIFY(bv2.validate());
			EATEST_VERIFY(bv2.size() == 200);
			for(eastl_size_t i = 0; i < bv2.size(); i++)
				EATEST_VERIFY(bv2[i] == false);

			EATEST_VERIFY(bv3.validate());
			EATEST_VERIFY(bv3.size() == 300);
			for(eastl_size_t i = 0; i < bv3.size(); i++)
				EATEST_VERIFY(bv3[i] == true);

			EATEST_VERIFY(bv4.validate());
			EATEST_VERIFY(bv4.size() == 400);
			for(eastl_size_t i = 0; i < bv4.size(); i++)
				EATEST_VERIFY(bv4[i] == false);

			EATEST_VERIFY(bv5.validate());
			EATEST_VERIFY(bv5 == bv2);
			for(eastl_size_t i = 0; i < bv5.size(); i++)
				EATEST_VERIFY(bv5[i] == false);

			EATEST_VERIFY(bv6.validate());
			EATEST_VERIFY(bv6.size() == EAArrayCount(boolArray));
			for(eastl_size_t i = 0; i < bv6.size(); i++)
				EATEST_VERIFY(bv6[i] == boolArray[i]);

			EATEST_VERIFY(bv7.validate());
			EATEST_VERIFY(bv7.size() == bv3.size()); // The == test theoretically includes this test, be we check anyway.
			for(eastl_size_t j = 0; j < bv7.size(); j++)
				EATEST_VERIFY(bv7[j] == bv3[j]);
			EATEST_VERIFY(bv7 == bv3);
			for(eastl_size_t i = 0; (i < bv3.size()) && (i < bv7.size()); i++)
				EATEST_VERIFY(bv3[i] == bv7[i]);
		}

		{
			// void swap(this_type& x);

			bv7.swap(bv7); // Test swapping against self.
			EATEST_VERIFY(bv7.validate());
			EATEST_VERIFY(bv7 == bv3);
			EATEST_VERIFY(bv7.size() == bv3.size()); // The == test theoretically includes this test, be we check anyway.
			for(eastl_size_t i = 0; (i < bv3.size()) && (i < bv7.size()); i++)
				EATEST_VERIFY(bv3[i] == bv7[i]);

			bv3.swap(bv2);  // Note that bv3 and bv4 use different allocators, so we are exercizing that.
			EATEST_VERIFY(bv3.validate());
			EATEST_VERIFY(bv3.size() == 200);
			for(eastl_size_t i = 0; i < bv3.size(); i++)
				EATEST_VERIFY(bv3[i] == false);

			EATEST_VERIFY(bv2.validate());
			EATEST_VERIFY(bv2.size() == 300);
			for(eastl_size_t i = 0; i < bv2.size(); i++)
				EATEST_VERIFY(bv2[i] == true);


			// bitvector& operator=(const bitvector& x);

			bv6 = bv7;
			EATEST_VERIFY(bv6.validate());
			EATEST_VERIFY(bv6 == bv7);


			// template <typename InputIterator> void assign(InputIterator first, InputIterator last);
			bv0.assign(bv3.begin(), bv3.end());
			EATEST_VERIFY(bv0 == bv3);

			bv0.assign(boolArray, boolArray + EAArrayCount(boolArray));
			EATEST_VERIFY(bv0 == bitvector<>(boolArray, boolArray + EAArrayCount(boolArray)));

			bv0.resize(0);
			EATEST_VERIFY(bv0.begin()==bv0.end());//should not crash
			bv3.resize(0);
			EATEST_VERIFY(bv0 == bv3);
		}
	}


	{
		// iterator       begin();
		// const_iterator begin() const;
		// iterator       end();
		// const_iterator end() const;

		bool                        boolArray[] = { true, false, true, true, false, true };
		const bitvector<>           bv0(boolArray, boolArray + EAArrayCount(boolArray));
		bitvector<>::const_iterator it;
		eastl_size_t                i;

		for(it = bv0.begin(), i = 0; it != bv0.end(); ++it, ++i)        // Iterate forward by 1.
		{
			const bool value = *it;
			EATEST_VERIFY(value == boolArray[i]);
		}

		for(--it, --i; (eastl_ssize_t)i >= 0; --it, --i)                // Iterate backward by 1. Problem: this test code does --it for it == begin(), which isn't strictly allowed.
		{
			const bool value = *it;
			EATEST_VERIFY(value == boolArray[i]);
		}

		// The following code asssumes an even number of elements.
		EASTL_CT_ASSERT((EAArrayCount(boolArray) % 2) == 0);  
		for(it = bv0.begin(), ++i; it != bv0.end(); it += 2, i += 2)  // Iterate forward by 2.
		{
			const bool value = *it;
			EATEST_VERIFY(value == boolArray[i]);
		}

		for(it -= 2, i -= 2; (eastl_ssize_t)i >= 0; it -= 2, i -= 2)    // Iterate backward by 1. Problem: this test code does it -= 2 for it == begin(), which isn't strictly allowed.
		{
			const bool value = *it;
			EATEST_VERIFY(value == boolArray[i]);
		}


		// reverse_iterator       rbegin();
		// const_reverse_iterator rbegin() const;
		// reverse_iterator       rend();
		// const_reverse_iterator rend() const;

		bitvector<>::const_reverse_iterator rit;
		i = (bv0.size() - 1);

		for(rit = bv0.rbegin(); rit != bv0.rend(); ++rit, --i)              // Reverse-iterate forward by 1.
		{
			//const bool value = *rit; // This is currently broken and will require a bit of work to fix.
			const bool value = *--rit.base();
			EATEST_VERIFY(value == boolArray[i]);
		}

		for(--rit, ++i; i < bv0.size(); --rit, ++i)                  // Reverse-iterate backward by 1.
		{
			//const bool value = *rit; // This is currently broken and will require a bit of work to fix.
			const bool value = *--rit.base();
			EATEST_VERIFY(value == boolArray[i]);
		}

		// The following code asssumes an even number of elements.
		EASTL_CT_ASSERT((EAArrayCount(boolArray) % 2) == 0);  
		for(rit = bv0.rbegin(), --i; rit != bv0.rend(); rit += 2, i -= 2) // Reverse-iterate forward by 2.
		{
			//const bool value = *rit; // This is currently broken and will require a bit of work to fix.
			const bool value = *--rit.base();
			EATEST_VERIFY(value == boolArray[i]);
		}

		for(rit -= 2, i += 2; i < bv0.size(); rit -= 2, i += 2)            // Reverse-iterate backward by 2.
		{
			//const bool value = *rit; // This is currently broken and will require a bit of work to fix.
			const bool value = *--rit.base();
			EATEST_VERIFY(value == boolArray[i]);
		}


		// find_first, etc.
		/* This work is not complete.
		{
			bitvector<> bv(30, false);

			bitvector<>::iterator it = bv.find_first();
			EATEST_VERIFY(it == bv.begin());
		}
		*/
	}

	{
		MallocAllocator            mallocAllocator;
		bitvector<MallocAllocator> bv0(mallocAllocator);

		// bool      empty() const;
		// size_type size() const;
		// size_type capacity() const;

		EATEST_VERIFY(bv0.empty());
		EATEST_VERIFY(bv0.size() == 0);
		EATEST_VERIFY(bv0.capacity() == 0);  // EASTL requires that newly constructed containers have 0 capacity.

		bool boolArray[] = { false, true, true };
		bv0.assign(boolArray, boolArray + EAArrayCount(boolArray));

		EATEST_VERIFY(!bv0.empty());
		EATEST_VERIFY(bv0.size() == EAArrayCount(boolArray));
		EATEST_VERIFY((bv0.capacity() > 0) && (bv0.capacity() <= (8 * sizeof(bitvector<>::element_type))));


		// reference       front();
		// const_reference front() const;
		// reference       back();
		// const_reference back() const;

		EATEST_VERIFY(bv0.front() == false);
		EATEST_VERIFY(bv0.back() == true);
		bv0.erase(bv0.begin());
		EATEST_VERIFY(bv0.front() == true);
		bv0.erase(bv0.rbegin());
		EATEST_VERIFY(bv0.back() == true);

		//  void set_capacity(size_type n = npos);

		bv0.reserve(17);
		EATEST_VERIFY((bv0.capacity() >= 17) && (bv0.capacity() <= 100)); // It's hard to make a unit test to portably test an upper limit.

		int allocCountBefore = MallocAllocator::mAllocCountAll;
		while(bv0.size() < 17)
			bv0.push_back(false);
		EATEST_VERIFY(allocCountBefore == MallocAllocator::mAllocCountAll); // Verify no new memory was allocated.

		bv0.set_capacity();
		EATEST_VERIFY(bv0.capacity() >= bv0.size());

		bv0.set_capacity(0);
		EATEST_VERIFY(bv0.capacity() == 0);
		EATEST_VERIFY(bv0.empty());


		// void resize(size_type n, value_type value);
		// void resize(size_type n);
		// void reserve(size_type n);

		bv0.reserve(800);
		EATEST_VERIFY(bv0.capacity() >= 800);
		allocCountBefore = MallocAllocator::mAllocCountAll;
		bv0.resize(800, true);
		EATEST_VERIFY(allocCountBefore == MallocAllocator::mAllocCountAll); // Verify no new memory was allocated.


		// void push_back();
		// void push_back(value_type value);
		// void pop_back();
		// reference       operator[](size_type n);
		// const_reference operator[](size_type n) const;

		bv0.push_back();
		bv0.back() = true;
		bv0.push_back(false);
		bv0.push_back(true);

		EATEST_VERIFY(bv0[bv0.size()-1] == true);
		EATEST_VERIFY(bv0[bv0.size()-2] == false);
		EATEST_VERIFY(bv0[bv0.size()-3] == true);


		// reference       at(size_type n);
		// const_reference at(size_type n) const;

		EATEST_VERIFY(bv0.at(bv0.size()-1) == true);
		EATEST_VERIFY(bv0.at(bv0.size()-2) == false);
		EATEST_VERIFY(bv0.at(bv0.size()-3) == true);


		// void clear();
		// bool test(size_type n, bool defaultValue) const;
		// void set(bool value, size_type n);

		bv0.clear();
		bv0.resize(17, true);
		EATEST_VERIFY(bv0.test(0, false) == true);
		EATEST_VERIFY(bv0.test(17, false) == false); // Test past the end.
		EATEST_VERIFY(bv0.test(17, true) == true);

		bv0.set(3, false);
		EATEST_VERIFY(bv0.test(3, true) == false);

		bv0.set(100, true);
		EATEST_VERIFY(bv0.test(100, false) == true);


		// container_type&       get_container();
		// const container_type& get_container() const;

		EATEST_VERIFY(!bv0.get_container().empty());


		// bool validate() const;
		// int  validate_iterator(const_iterator i) const;

		EATEST_VERIFY(bv0.validate());
		bitvector<>::iterator it;
		EATEST_VERIFY(bv0.validate_iterator(it) == isf_none);
		for(it = bv0.begin(); it != bv0.end(); ++it)
			EATEST_VERIFY(bv0.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
		EATEST_VERIFY(bv0.validate_iterator(it) == (isf_valid | isf_current));
		


		// iterator insert(iterator position, value_type value);
		// void     insert(iterator position, size_type n, value_type value);

		bv0.clear();
		bv0.resize(17, true);
		bv0.insert(bv0.begin() + 5, false);
		EATEST_VERIFY(bv0[5] == false);
		bv0[5] = true;
		EATEST_VERIFY(bv0[5] == true);

		bv0.insert(bv0.begin() + 5, 7, false);
		EATEST_VERIFY((bv0[5] == false) && (bv0[11] == false));

		EATEST_VERIFY(bv0.back() == true);
		bv0.insert(bv0.end(), false);
		EATEST_VERIFY(bv0.back() == false);


		// iterator erase(iterator position);
		// iterator erase(iterator first, iterator last);

		EATEST_VERIFY((bv0[10] == false) && (bv0[11] == false));
		bv0.erase(bv0.begin() + 11);
		EATEST_VERIFY((bv0[10] == false) && (bv0[11] == true));

		EATEST_VERIFY(bv0[5] == false);
		bool bv06 = bv0[6];
		bv0.erase(bv0.begin() + 5, bv0.begin() + 6);
		EATEST_VERIFY(bv0[5] == bv06);


		// reverse_iterator erase(reverse_iterator position);
		// reverse_iterator erase(reverse_iterator first, reverse_iterator last);

		bv0.clear();
		bv0.resize(10, true);
		bv0.back() = false;
		bv0.erase(bv0.rbegin());
		EATEST_VERIFY((bv0.size() == 9) && (bv0.back() == true));

		bv0.erase(bv0.rbegin(), bv0.rend());
		EATEST_VERIFY(bv0.empty());


		// template <typename InputIterator> Not yet implemented. See below for disabled definition.
		// void      insert(iterator position, InputIterator first, InputIterator last);
		//
		// Disabled because insert isn't implemented yet.
		// const bool boolArray2[4] = { false, true, false, true };        
		// bv0.insert(bv0.end(), boolArray2, boolArray2 + EAArrayCount(boolArray));
		// EATEST_VERIFY(bv0.size() == EAArrayCount(boolArray2));


		// element_type*         data();
		// const element_type*   data() const;

		EATEST_VERIFY(bv0.data() != NULL);
		bv0.set_capacity(0);
		EATEST_VERIFY(bv0.data() == NULL);


		// void reset_lose_memory(); // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

		bv0.resize(100, true);
		void* pSaved = MallocAllocator::mpLastAllocation;
		bv0.reset_lose_memory();
		EATEST_VERIFY(bv0.validate());
		free(pSaved); // Call the C free function.
		MallocAllocator::mpLastAllocation = NULL;
	}

	return nErrorCount;
}













/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/bonus/intrusive_sdlist.h>
#include <EASTL/string.h>
#include <EABase/eabase.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdarg.h>
#include <stdio.h>

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif



using namespace eastl;


namespace TestSDListLocal
{

	struct IntNode : public intrusive_sdlist_node
	{
		IntNode() {}
		IntNode(int x) : mX(x) {}
		operator int() const { return mX; }

		int mX;
	};

	typedef intrusive_sdlist<IntNode> IntrusiveSDList;

	template <class T>
	eastl::string IntListToString8(const T& cont)
	{
		eastl::string s("<");
		char          buf[64];

		for(typename T::const_iterator it(cont.begin()), itEnd(cont.end()); it != itEnd; ++it)
		{
			const int& v = *it;
			sprintf(buf, " %d", v);
			s += buf;
		}

		s += " >";
		return s;
	}


	template <class T>
	bool VerifyContainer(const T& cont, const char *testname, ...)
	{
		//if (!cont.validate()) {
		//    EASTLTest_Printf("intrusive_list[%s] container damaged!\n", testname);
		//    return false;
		//}

		typename T::const_iterator it(cont.begin()), itEnd(cont.end());
		va_list val;
		int index = 0;

		va_start(val, testname);
		while(it != itEnd)
		{
			int next = va_arg(val, int);

			if (next == -1 || next != *it)
			{
				const int value = *it;
				const char* const pString = IntListToString8(cont).c_str();
				EASTLTest_Printf("intrusive_list[%s] Mismatch at index %d: expected %d, found %d; contents: %s\n", testname, index, next, value, pString);
				va_end(val);
				return false;
			}

			++it;
			++index;
		}

		if (va_arg(val, int) != -1)
		{
			do {
				++index;
			} while(va_arg(val, int) != -1);

			const int countainerSize = (int)cont.size();
			const char* const pString = IntListToString8(cont).c_str();
			EASTLTest_Printf("intrusive_list[%s] Too many elements: expected %d, found %d; contents: %s\n", testname, index, countainerSize, pString);
			va_end(val);
			return false;
		}

		va_end(val);

		// We silence this by default for a quieter test run.
		// EASTLTest_Printf("intrusive_list[%s] pass\n", testname);
		return true;
	}


	class ListInit
	{
	public:
		ListInit(intrusive_sdlist<IntNode>& container, IntNode* pNodeArray)
			: mpContainer(&container), mpNodeArray(pNodeArray)
		{
			mpContainer->clear();
		}

		ListInit& operator+=(int x)
		{
			mpNodeArray->mX = x;
			mpContainer->push_back(*mpNodeArray++);
			return *this;
		}

		ListInit& operator,(int x)
		{
			mpNodeArray->mX = x;
			mpContainer->push_back(*mpNodeArray++);
			return *this;
		}

	protected:
		intrusive_sdlist<IntNode>* mpContainer;
		IntNode*                   mpNodeArray;
	};

} // namespace




// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::intrusive_sdlist<TestSDListLocal::IntNode>;



int TestIntrusiveSDList()
{
	using namespace TestSDListLocal;

	int nErrorCount = 0;

	IntNode nodes[20];

	IntrusiveSDList l;

	// Enforce that the intrusive_list copy ctor is visible. If it is not, then
	// the class is not a POD type as it is supposed to.
	delete new IntrusiveSDList(l);

	// Enforce that offsetof() can be used with an intrusive_list in a struct;
	// it requires a POD type. Some compilers will flag warnings or even errors
	// when this is violated.
	struct Test { IntrusiveSDList m; };

	#ifndef __GNUC__ // GCC warns on this, though strictly specaking it is allowed to.
		(void)offsetof(Test, m);
	#endif

	VERIFY(VerifyContainer(l, "ctor()", -1));

	// push_back
	ListInit(l, nodes) += 0, 1, 2, 3, 4, 5, 6, 7, 8, 9;
	VERIFY(VerifyContainer(l, "push_back()", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1));

	// iterator++
	{
		IntrusiveSDList::iterator it1(l.begin());
		IntrusiveSDList::iterator it2(l.begin());

		++it1;
		++it2;

		if (it1 != it2++ || ++it1 != it2) {
			VERIFY(!"[iterator::increment] fail\n");
		}
	}

	// clear()/empty()
	VERIFY(!l.empty());

	l.clear();
	VERIFY(VerifyContainer(l, "clear()", -1));
	VERIFY(l.empty());

	l.erase(l.begin(), l.end()); // Erase an already empty container.
	VERIFY(l.empty());

	IntrusiveSDList l2;

	// splice
	//ListInit(l, nodes) += 0, 1, 2, 3, 4, 5, 6, 7, 8, 9;
	//
	//l.splice(++l.begin(), l, --l.end());
	//VERIFY(VerifyContainer(l, "splice(single)", 0, 9, 1, 2, 3, 4, 5, 6, 7, 8, -1));
	//
	//ListInit(l2, nodes+10) += 10, 11, 12, 13, 14, 15, 16, 17, 18, 19;
	//
	//l.splice(++++l.begin(), l2);
	//VERIFY(VerifyContainer(l2, "splice(whole)", -1));
	//VERIFY(VerifyContainer(l, "splice(whole)", 0, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 1, 2, 3, 4, 5, 6, 7, 8, -1));

	//l.splice(l.begin(), l, ++++l.begin(), ----l.end());
	//VERIFY(VerifyContainer(l, "splice(range)", 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 1, 2, 3, 4, 5, 6, 0, 9, 7, 8, -1));

	//l.clear();
	//l.swap(l2);
	//VERIFY(VerifyContainer(l, "swap(empty)", -1));
	//VERIFY(VerifyContainer(l2, "swap(empty)", -1));

	//l2.push_back(nodes[0]);
	//l.splice(l.begin(), l2);
	//VERIFY(VerifyContainer(l, "splice(single)", 0, -1));
	//VERIFY(VerifyContainer(l2, "splice(single)", -1));

	// splice(single) -- evil case (splice at or right after current position)
	//ListInit(l, nodes) += 0, 1, 2, 3, 4;
	//l.splice(++++l.begin(), *++++l.begin());
	//VERIFY(VerifyContainer(l, "splice(single)", 0, 1, 2, 3, 4, -1));
	//l.splice(++++++l.begin(), *++++l.begin());
	//VERIFY(VerifyContainer(l, "splice(single)", 0, 1, 2, 3, 4, -1));

	// splice(range) -- evil case (splice right after current position)
	//ListInit(l, nodes) += 0, 1, 2, 3, 4;
	//l.splice(++++l.begin(), l, ++l.begin(), ++++l.begin());
	//VERIFY(VerifyContainer(l, "splice(range)", 0, 1, 2, 3, 4, -1));

	// push_front()
	l.clear();
	l2.clear();
	for(int i=4; i>=0; --i) {
		l.push_front(nodes[i]);
		l2.push_front(nodes[i+5]);
	}

	VERIFY(VerifyContainer(l, "push_front()", 0, 1, 2, 3, 4, -1));
	VERIFY(VerifyContainer(l2, "push_front()", 5, 6, 7, 8, 9, -1));

	// swap()
	l.swap(l2);
	VERIFY(VerifyContainer(l, "swap()", 5, 6, 7, 8, 9, -1));
	VERIFY(VerifyContainer(l2, "swap()", 0, 1, 2, 3, 4, -1));

	// erase()
	ListInit(l2, nodes) += 0, 1, 2, 3, 4;
	ListInit(l, nodes+5) += 5, 6, 7, 8, 9;
	l.erase(++++l.begin());
	VERIFY(VerifyContainer(l, "erase(single)", 5, 6, 8, 9, -1));

	l.erase(l.begin(), l.end());
	VERIFY(VerifyContainer(l, "erase(all)", -1));

	ListInit(l, nodes) += 0, 1, 2;
	VERIFY(l2.size() == 3);

	l2.pop_front();
	VERIFY(VerifyContainer(l2, "pop_front()", 1, 2, -1));

	l2.pop_back();
	VERIFY(VerifyContainer(l2, "pop_back()", 1, -1));

	// remove
	IntNode i1(1), i2(2), i3(3);
	l.clear();

	l.push_front(i1);
	IntrusiveSDList::remove(i1);
	VERIFY(VerifyContainer(l, "remove()", -1));

	l.push_front(i1);
	l.push_front(i2);
	IntrusiveSDList::remove(i1);
	VERIFY(VerifyContainer(l, "remove()", 2, -1));

	l.push_front(i1);
	IntrusiveSDList::remove(i2);
	VERIFY(VerifyContainer(l, "remove()", 1, -1));

	l.push_back(i2);
	l.push_back(i3);
	IntrusiveSDList::remove(i2);
	VERIFY(VerifyContainer(l, "remove()", 1, 3, -1));


	// const_iterator / begin
	const intrusive_sdlist<IntNode>           cilist;
	intrusive_sdlist<IntNode>::const_iterator cit;
	for(cit = cilist.begin(); cit != cilist.end(); ++cit)
		VERIFY(cit == cilist.end()); // This is guaranteed to be false.



	return nErrorCount;
}










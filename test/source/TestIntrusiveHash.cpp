/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/internal/intrusive_hashtable.h>
#include <EASTL/intrusive_hash_set.h>
#include <EASTL/intrusive_hash_map.h>
#include <EABase/eabase.h>



using namespace eastl;


namespace
{
	struct SetWidget : public intrusive_hash_node
	{
		SetWidget(int x = 0)
			: mX(x) { }
		int mX;
	};

	inline bool operator==(const SetWidget& a, const SetWidget& b)
		{ return a.mX == b.mX; }

	struct SWHash
	{
		size_t operator()(const SetWidget& sw) const
		{
			return (size_t)sw.mX;
		}
	};

	struct SetWidgetComparable // Exists for the sole purpose of testing the find_as function.
	{
		SetWidgetComparable(int x = 0)
			: mX(x) { }
		int mX;
	};

	struct SWCHash
	{
		size_t operator()(const SetWidgetComparable& swc) const
		{
			return (size_t)swc.mX;
		}
	};

	bool operator==(const SetWidget& a, const SetWidgetComparable& b)
		{ return a.mX == b.mX; }



	struct MapWidget : public intrusive_hash_node_key<int>
	{
		MapWidget(int x = 0)
			: mX(x) { }
		int mX;
	};

	inline bool operator==(const MapWidget& a, const MapWidget& b)
		{ return a.mX == b.mX; }

	//struct MapWidgetComparable // Exists for the sole purpose of testing the find_as function.
	//{
	//    MapWidgetComparable(int x = 0)
	//        : mX(x) { }
	//    int mX;
	//};
	//
	//bool operator==(const SetWidget& a, const MapWidgetComparable& b)
	//    { return a.mX == b.mX; }




	// IHWidget
	//
	// Implements the intrusive node data directly instead of inheriting from intrusive_hash_node.
	//
	struct IHWidget
	{
		IHWidget(int x = 0)
			: mX(x) { }

		int         mX;
		IHWidget*   mpNext;
		typedef int key_type;
		int         mKey;

	};

	inline bool operator==(const IHWidget& a, const IHWidget& b)
		{ return a.mX == b.mX; }

	struct IHWHash
	{
		size_t operator()(const IHWidget& ihw) const
		{
			return (size_t)ihw.mX;
		}
	};

} // namespace




// Template instantations.
// These tell the compiler to compile all the functions for the given class.
//template class intrusive_hash_set<SetWidget>;
//template class intrusive_hash_map<MapWidget>;


template class eastl::intrusive_hashtable<SetWidget, SetWidget, SWHash, eastl::equal_to<SetWidget>, 37, true, true>;
template class eastl::intrusive_hashtable<int, MapWidget, eastl::hash<int>, eastl::equal_to<int>, 37, false, true>;

template class eastl::intrusive_hash_set<SetWidget, 37, SWHash>;
template class eastl::intrusive_hash_multiset<SetWidget, 37, SWHash>;

template class eastl::intrusive_hash_map<int, MapWidget, 37>;
template class eastl::intrusive_hash_multimap<int, MapWidget, 37>;

template class eastl::intrusive_hash_set<IHWidget, 37, IHWHash>;
template class eastl::intrusive_hash_multiset<IHWidget, 37, IHWHash>;

template class eastl::intrusive_hash_map<int, IHWidget, 37, IHWHash>;
template class eastl::intrusive_hash_multimap<int, IHWidget, 37, IHWHash>;





int TestIntrusiveHash()
{
	int nErrorCount = 0;

	{
		SetWidget sw1, sw2;
		VERIFY(sw1 == sw2);

		MapWidget mw1, mw2;
		VERIFY(mw1 == mw2);

		IHWidget iw1, iw2;
		VERIFY(iw1 == iw2);

		IHWHash ih1;
		VERIFY(ih1.operator()(iw1) == ih1.operator()(iw2));
	}

	{
		// Test intrusive_hash_set

		const size_t kBucketCount = 37;
		typedef intrusive_hash_set<SetWidget, kBucketCount, SWHash> IHM_SW;

		const size_t kArraySize = 100;
		SetWidget swArray[kArraySize];

		int nExpectedKeySum = 0; // We use this as a checksum in order to do validity checks below.

		for(size_t i = 0; i < kArraySize; i++)
		{
			swArray[i].mX    = (int)i;
			nExpectedKeySum += (int)i;
		}


		// const key_equal& key_eq() const;
		// key_equal&       key_eq();
		IHM_SW       ih;
		const IHM_SW ihc;

		const IHM_SW::key_equal& ke = ihc.key_eq();
		ih.key_eq() = ke;


		// intrusive_hashtable(const Hash&, const Equal&);
		// void swap(this_type& x);
		// size_type size() const;
		// bool empty() const;
		// size_type bucket_count() const;
		// size_type bucket_size(size_type n) const;
		// float load_factor() const;
		// void clear();
		// bool validate() const;

		IHM_SW ihmSW1;
		IHM_SW ihmSW2;

		VERIFY(ihmSW1.size() == 0);
		VERIFY(ihmSW1.empty());
		VERIFY(ihmSW1.validate());
		VERIFY(ihmSW2.validate());

		ihmSW1.swap(ihmSW2);

		VERIFY(ihmSW1.validate());
		VERIFY(ihmSW2.validate());
		VERIFY(ihmSW2.bucket_count() == kBucketCount);
		VERIFY(ihmSW2.bucket_size(0) == 0);
		VERIFY(ihmSW2.bucket_size(kBucketCount - 1) == 0);
		VERIFY(ihmSW1.load_factor() == 0.f);
		VERIFY(ihmSW2.load_factor() == 0.f);

		ihmSW1.clear();
		VERIFY(ihmSW1.validate());
		VERIFY(ihmSW1.begin() == ihmSW1.end());


		// void insert(InputIterator first, InputIterator last);
		// insert_return_type insert(value_type& value);
		// void swap(this_type& x);
		// void clear();

		ihmSW1.clear();
		ihmSW1.insert(swArray, swArray + (kArraySize - 10));
		for(int i = 0; i < 10; i++) // insert the remaining elements via the other insert function.
		{
			pair<IHM_SW::iterator, bool> result = ihmSW1.insert(swArray[(kArraySize - 10) + i]);
			VERIFY(result.second == true);
		}

		VERIFY(ihmSW1.size() == kArraySize);
		VERIFY(ihmSW1.validate());

		for(size_t i = 0; i < kArraySize; i++)
		{
			// Try to re-insert the elements. All insertions should fail.
			pair<IHM_SW::iterator, bool> result = ihmSW1.insert(swArray[i]);
			VERIFY(result.second == false);
		}

		VERIFY(ihmSW1.size() == kArraySize);
		VERIFY(!ihmSW1.empty());
		VERIFY(ihmSW1.validate());

		ihmSW2.clear();
		ihmSW1.swap(ihmSW2);


		// size_type size() const;
		// bool empty() const;
		// size_type count(const key_type& k) const;
		// size_type bucket_size(size_type n) const;
		// float load_factor() const;
		// size_type bucket(const key_type& k) const

		VERIFY(ihmSW1.validate());
		VERIFY(ihmSW2.validate());
		VERIFY(ihmSW1.size() == 0);
		VERIFY(ihmSW1.empty());
		VERIFY(ihmSW2.size() == kArraySize);
		VERIFY(!ihmSW2.empty());
		VERIFY(ihmSW1.load_factor() == 0.f);
		VERIFY(ihmSW2.load_factor() > 2.f);
		VERIFY(ihmSW1.count(0) == 0);
		VERIFY(ihmSW1.count(999999) == 0);
		VERIFY(ihmSW2.count(0) == 1);
		VERIFY(ihmSW2.count(999999) == 0);
		VERIFY(ihmSW2.bucket_size(0) == 3);     // We just happen to know this should be so based on the distribution.
		VERIFY(ihmSW2.bucket(13)    == (13    % kBucketCount)); // We know this is so because our hash function simply returns n.
		VERIFY(ihmSW2.bucket(10000) == (10000 % kBucketCount)); // We know this is so because our hash function simply returns n.
   

		// iterator begin();
		// const_iterator begin() const;

		ihmSW1.swap(ihmSW2);
		int nSum = 0; 

		for(IHM_SW::iterator it = ihmSW1.begin(); it != ihmSW1.end(); ++it)
		{
			const SetWidget& sw = *it; // Recall that set iterators are const_iterators.

			nSum += sw.mX;

			const int iresult = ihmSW1.validate_iterator(it);
			VERIFY(iresult == (isf_valid | isf_current | isf_can_dereference));

			IHM_SW::iterator itf = ihmSW1.find(sw.mX);
			VERIFY(itf == it);
		}

		VERIFY(nSum == nExpectedKeySum);


		// iterator end();
		// const_iterator end() const;

		const IHM_SW& ihmSW1Const = ihmSW1;

		for(IHM_SW::const_iterator itc = ihmSW1Const.begin(); itc != ihmSW1Const.end(); ++itc)
		{
			const SetWidget& sw = *itc;

			IHM_SW::const_iterator itf = ihmSW1.find(sw.mX);
			VERIFY(itf == itc);
		}


		// local_iterator begin(size_type n)
		// local_iterator end(size_type)

		for(IHM_SW::local_iterator itl = ihmSW1.begin(5); itl != ihmSW1.end(5); ++itl)
		{
			const SetWidget& sw = *itl; // Recall that set iterators are const_iterators.

			VERIFY((sw.mX % kBucketCount) == 5);
		}


		// const_local_iterator begin(size_type n) const
		// const_local_iterator end(size_type) const

		for(IHM_SW::const_local_iterator itlc = ihmSW1Const.begin(5); itlc != ihmSW1Const.end(5); ++itlc)
		{
			const SetWidget& sw = *itlc;

			VERIFY((sw.mX % kBucketCount) == 5);
		}


		// iterator       find(const key_type& k);
		// const_iterator find(const key_type& k) const;

		IHM_SW::iterator itf = ihmSW1.find(SetWidget(99999));
		VERIFY(itf == ihmSW1.end());

		IHM_SW::const_iterator itfc = ihmSW1Const.find(SetWidget(99999));
		VERIFY(itfc == ihmSW1Const.end());


		// iterator       find_as(const U& u);
		// const_iterator find_as(const U& u) const;

		//itf = ihmSW1.find_as(SetWidget(7)); // Can't work unless there was a default eastl::hash function for SetWidget.
		//VERIFY(itf->mX == 7);

		//itfc = ihmSW1Const.find_as(SetWidget(7));
		//VERIFY(itfc->mX == 7);


		// iterator       find_as(const U& u, UHash uhash, BinaryPredicate predicate);
		// const_iterator find_as(const U& u, UHash uhash, BinaryPredicate predicate) const;

		itf = ihmSW1.find_as(SetWidgetComparable(7), SWCHash(), eastl::equal_to<>());
		VERIFY(itf->mX == 7);

		itfc = ihmSW1Const.find_as(SetWidgetComparable(7), SWCHash(), eastl::equal_to<>());
		VERIFY(itfc->mX == 7);


		// iterator  erase(iterator);
		// iterator  erase(iterator, iterator);
		// size_type erase(const key_type&);

		eastl_size_t n = ihmSW1.erase(SetWidget(99999));
		VERIFY(n == 0);

		n = ihmSW1.erase(SetWidget(17));
		VERIFY(n == 1);

		itf = ihmSW1.find(SetWidget(18));
		VERIFY(itf != ihmSW1.end());
		VERIFY(ihmSW1.validate_iterator(itf) == (isf_valid | isf_current | isf_can_dereference));

		itf = ihmSW1.erase(itf);
		VERIFY(itf != ihmSW1.end());
		VERIFY(ihmSW1.validate_iterator(itf) == (isf_valid | isf_current | isf_can_dereference));

		itf = ihmSW1.find(SetWidget(18));
		VERIFY(itf == ihmSW1.end());

		itf = ihmSW1.find(SetWidget(19));
		VERIFY(itf != ihmSW1.end());

		IHM_SW::iterator itf2(itf);
		eastl::advance(itf2, 7);
		VERIFY(itf2 != ihmSW1.end());
		VERIFY(ihmSW1.validate_iterator(itf2) == (isf_valid | isf_current | isf_can_dereference));

		itf = ihmSW1.erase(itf, itf2);
		VERIFY(itf != ihmSW1.end());
		VERIFY(ihmSW1.validate_iterator(itf) == (isf_valid | isf_current | isf_can_dereference));

		itf = ihmSW1.find(SetWidget(19));
		VERIFY(itf == ihmSW1.end());


		// eastl::pair<iterator, iterator>             equal_range(const key_type& k);
		// eastl::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

		eastl::pair<IHM_SW::iterator, IHM_SW::iterator> p = ihmSW1.equal_range(SetWidget(1));
		VERIFY(p.first != ihmSW1.end());
		VERIFY(p.second != ihmSW1.end());

		eastl::pair<IHM_SW::const_iterator, IHM_SW::const_iterator> pc = ihmSW1Const.equal_range(SetWidget(1));
		VERIFY(pc.first != ihmSW1Const.end());
		VERIFY(pc.second != ihmSW1Const.end());


		// void clear();
		// bool validate() const;
		// int validate_iterator(const_iterator i) const;

		IHM_SW::iterator itTest;
		int iresult = ihmSW1.validate_iterator(itTest);
		VERIFY(iresult == isf_none);

		itTest = ihmSW1.begin();
		iresult = ihmSW1.validate_iterator(itTest);
		VERIFY(iresult == (isf_valid | isf_current | isf_can_dereference));

		itTest = ihmSW1.end();
		iresult = ihmSW1.validate_iterator(itTest);
		VERIFY(iresult == (isf_valid | isf_current));

		ihmSW1.clear();
		ihmSW2.clear();
		VERIFY(ihmSW1.validate());
		VERIFY(ihmSW2.validate());

		itTest = ihmSW1.begin();
		iresult = ihmSW1.validate_iterator(itTest);
		VERIFY(iresult == (isf_valid | isf_current));
	}


	{
		// Test intrusive_hash_map

		const size_t kBucketCount = 37;
		typedef intrusive_hash_map<int, MapWidget, kBucketCount> IHM_MW;

		const size_t kArraySize = 100;
		MapWidget mwArray[kArraySize];

		int nExpectedKeySum = 0; // We use this as a checksum in order to do validity checks below.

		for(size_t i = 0; i < kArraySize; i++)
		{
			mwArray[i].mKey  = (int)i;
			mwArray[i].mX    = (int)i;
			nExpectedKeySum += (int)i;
		}


		// intrusive_hashtable(const Hash&, const Equal&);
		// void swap(this_type& x);
		// size_type size() const;
		// bool empty() const;
		// size_type bucket_count() const;
		// size_type bucket_size(size_type n) const;
		// float load_factor() const;
		// void clear();
		// bool validate() const;

		IHM_MW ihmMW1;
		IHM_MW ihmMW2;

		VERIFY(ihmMW1.size() == 0);
		VERIFY(ihmMW1.empty());
		VERIFY(ihmMW1.validate());
		VERIFY(ihmMW2.validate());

		ihmMW1.swap(ihmMW2);

		VERIFY(ihmMW1.validate());
		VERIFY(ihmMW2.validate());
		VERIFY(ihmMW2.bucket_count() == kBucketCount);
		VERIFY(ihmMW2.bucket_size(0) == 0);
		VERIFY(ihmMW2.bucket_size(kBucketCount - 1) == 0);
		VERIFY(ihmMW1.load_factor() == 0.f);
		VERIFY(ihmMW2.load_factor() == 0.f);

		ihmMW1.clear();
		VERIFY(ihmMW1.validate());
		VERIFY(ihmMW1.begin() == ihmMW1.end());


		// void insert(InputIterator first, InputIterator last);
		// insert_return_type insert(value_type& value);
		// void swap(this_type& x);
		// void clear();

		ihmMW1.clear();
		ihmMW1.insert(mwArray, mwArray + (kArraySize - 10));
		for(int i = 0; i < 10; i++) // insert the remaining elements via the other insert function.
		{
			pair<IHM_MW::iterator, bool> result = ihmMW1.insert(mwArray[(kArraySize - 10) + i]);
			VERIFY(result.second == true);
		}

		VERIFY(ihmMW1.size() == kArraySize);
		VERIFY(ihmMW1.validate());

		for(size_t i = 0; i < kArraySize; i++)
		{
			// Try to re-insert the elements. All insertions should fail.
			pair<IHM_MW::iterator, bool> result = ihmMW1.insert(mwArray[i]);
			VERIFY(result.second == false);
		}

		VERIFY(ihmMW1.size() == kArraySize);
		VERIFY(!ihmMW1.empty());
		VERIFY(ihmMW1.validate());

		ihmMW2.clear();
		ihmMW1.swap(ihmMW2);


		// size_type size() const;
		// bool empty() const;
		// size_type count(const key_type& k) const;
		// size_type bucket_size(size_type n) const;
		// float load_factor() const;
		// size_type bucket(const key_type& k) const

		VERIFY(ihmMW1.validate());
		VERIFY(ihmMW2.validate());
		VERIFY(ihmMW1.size() == 0);
		VERIFY(ihmMW1.empty());
		VERIFY(ihmMW2.size() == kArraySize);
		VERIFY(!ihmMW2.empty());
		VERIFY(ihmMW1.load_factor() == 0.f);
		VERIFY(ihmMW2.load_factor() > 2.f);
		VERIFY(ihmMW1.count(0) == 0);
		VERIFY(ihmMW1.count(999999) == 0);
		VERIFY(ihmMW2.count(0) == 1);
		VERIFY(ihmMW2.count(999999) == 0);
		VERIFY(ihmMW2.bucket_size(0) == 3);     // We just happen to know this should be so based on the distribution.
		VERIFY(ihmMW2.bucket(13)    == (13    % kBucketCount)); // We know this is so because our hash function simply returns n.
		VERIFY(ihmMW2.bucket(10000) == (10000 % kBucketCount)); // We know this is so because our hash function simply returns n.
   

		// iterator begin();
		// const_iterator begin() const;

		ihmMW1.swap(ihmMW2);
		int nSum = 0; 

		for(IHM_MW::iterator it = ihmMW1.begin(); it != ihmMW1.end(); ++it)
		{
			IHM_MW::value_type& v = *it;

			VERIFY(v.mKey == v.mX); // We intentionally made this so above.
			nSum += v.mKey;

			const int iresult = ihmMW1.validate_iterator(it);
			VERIFY(iresult == (isf_valid | isf_current | isf_can_dereference));

			IHM_MW::iterator itf = ihmMW1.find(v.mKey);
			VERIFY(itf == it);
		}

		VERIFY(nSum == nExpectedKeySum);


		// iterator end();
		// const_iterator end() const;

		const IHM_MW& ihmMW1Const = ihmMW1;

		for(IHM_MW::const_iterator itc = ihmMW1Const.begin(); itc != ihmMW1Const.end(); ++itc)
		{
			const IHM_MW::value_type& v = *itc;

			VERIFY(v.mKey == v.mX); // We intentionally made this so above.

			IHM_MW::const_iterator itf = ihmMW1Const.find(v.mKey);
			VERIFY(itf == itc);
		}


		// local_iterator begin(size_type n)
		// local_iterator end(size_type)

		for(IHM_MW::local_iterator itl = ihmMW1.begin(5); itl != ihmMW1.end(5); ++itl)
		{
			IHM_MW::value_type& v = *itl;

			VERIFY(v.mKey == v.mX); // We intentionally made this so above.
		}


		// const_local_iterator begin(size_type n) const
		// const_local_iterator end(size_type) const

		for(IHM_MW::const_local_iterator itlc = ihmMW1Const.begin(5); itlc != ihmMW1Const.end(5); ++itlc)
		{
			const IHM_MW::value_type& v = *itlc;

			VERIFY(v.mKey == v.mX); // We intentionally made this so above.
		}


		// iterator       find(const key_type& k);
		// const_iterator find(const key_type& k) const;

		IHM_MW::iterator itf = ihmMW1.find(99999);
		VERIFY(itf == ihmMW1.end());

		IHM_MW::const_iterator itfc = ihmMW1Const.find(99999);
		VERIFY(itfc == ihmMW1Const.end());


		// iterator       find_as(const U& u);
		// const_iterator find_as(const U& u) const;

		itf = ihmMW1.find_as(7.f);
		VERIFY(itf->mKey == 7);

		itfc = ihmMW1Const.find_as(7.f);
		VERIFY(itfc->mKey == 7);
				
		itf = ihmMW1.find_as(8);
		VERIFY(itf->mKey == 8);

		itfc = ihmMW1Const.find_as(8);
		VERIFY(itfc->mKey == 8);


		// iterator       find_as(const U& u, UHash uhash, BinaryPredicate predicate);
		// const_iterator find_as(const U& u, UHash uhash, BinaryPredicate predicate) const;

		itf = ihmMW1.find_as(7.f, eastl::hash<float>(), eastl::equal_to<>());
		VERIFY(itf->mKey == 7);

		itfc = ihmMW1Const.find_as(7.f, eastl::hash<float>(), eastl::equal_to<>());
		VERIFY(itfc->mKey == 7);


		// iterator  erase(iterator);
		// iterator  erase(iterator, iterator);
		// size_type erase(const key_type&);

		eastl_size_t n = ihmMW1.erase(99999);
		VERIFY(n == 0);

		n = ihmMW1.erase(17);
		VERIFY(n == 1);

		itf = ihmMW1.find(18);
		VERIFY(itf != ihmMW1.end());
		VERIFY(ihmMW1.validate_iterator(itf) == (isf_valid | isf_current | isf_can_dereference));

		itf = ihmMW1.erase(itf);
		VERIFY(itf != ihmMW1.end());
		VERIFY(ihmMW1.validate_iterator(itf) == (isf_valid | isf_current | isf_can_dereference));

		itf = ihmMW1.find(18);
		VERIFY(itf == ihmMW1.end());

		itf = ihmMW1.find(19);
		VERIFY(itf != ihmMW1.end());

		IHM_MW::iterator itf2(itf);
		eastl::advance(itf2, 7);
		VERIFY(itf2 != ihmMW1.end());
		VERIFY(ihmMW1.validate_iterator(itf2) == (isf_valid | isf_current | isf_can_dereference));

		itf = ihmMW1.erase(itf, itf2);
		VERIFY(itf != ihmMW1.end());
		VERIFY(ihmMW1.validate_iterator(itf) == (isf_valid | isf_current | isf_can_dereference));

		itf = ihmMW1.find(19);
		VERIFY(itf == ihmMW1.end());


		// eastl::pair<iterator, iterator>             equal_range(const key_type& k);
		// eastl::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

		eastl::pair<IHM_MW::iterator, IHM_MW::iterator> p = ihmMW1.equal_range(1);
		VERIFY(p.first != ihmMW1.end());
		VERIFY(p.second != ihmMW1.end());

		eastl::pair<IHM_MW::const_iterator, IHM_MW::const_iterator> pc = ihmMW1Const.equal_range(1);
		VERIFY(pc.first != ihmMW1Const.end());
		VERIFY(pc.second != ihmMW1Const.end());


		// void clear();
		// bool validate() const;
		// int validate_iterator(const_iterator i) const;

		IHM_MW::iterator itTest;
		int iresult = ihmMW1.validate_iterator(itTest);
		VERIFY(iresult == isf_none);

		itTest = ihmMW1.begin();
		iresult = ihmMW1.validate_iterator(itTest);
		VERIFY(iresult == (isf_valid | isf_current | isf_can_dereference));

		itTest = ihmMW1.end();
		iresult = ihmMW1.validate_iterator(itTest);
		VERIFY(iresult == (isf_valid | isf_current));

		ihmMW1.clear();
		ihmMW2.clear();
		VERIFY(ihmMW1.validate());
		VERIFY(ihmMW2.validate());

		itTest = ihmMW1.begin();
		iresult = ihmMW1.validate_iterator(itTest);
		VERIFY(iresult == (isf_valid | isf_current));
	}


	{
		// Test case of single bucket.
		eastl::intrusive_hash_set<SetWidget, 1, SWHash> hs;
		SetWidget node1, node2, node3;
		 
		node1.mX = 1;
		node2.mX = 2;
		node3.mX = 3;
		 
		hs.insert(node1);
		hs.insert(node2);
		hs.insert(node3);

		const eastl_size_t removeCount = hs.erase(node3);
		VERIFY(removeCount == 1); 
	}


	{
		// Test intrusive_hashtable_iterator(value_type* pNode, value_type** pBucket = NULL)
		eastl::intrusive_hash_set<SetWidget, 37, SWHash> hs;
		SetWidget node1, node2, node3;
		 
		node1.mX = 1;
		node2.mX = 2;
		node3.mX = 3;

		hs.insert(node1);
		hs.insert(node2);
		hs.insert(node3);

		VERIFY(hs.validate());

		hs.remove(node1);
		hs.remove(node2);
		hs.remove(node3);
		
		VERIFY(hs.validate());

		hs.insert(node1);
		hs.insert(node2);
		hs.insert(node3);

		VERIFY(hs.validate());
	}

	return nErrorCount;
}













/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

template<typename StringViewT>
int TEST_STRING_NAME()
{
	int nErrorCount = 0;
	{
		// EA_CONSTEXPR basic_string_view() 
		{
			StringViewT sw;
			VERIFY(sw.empty());
			VERIFY(sw.data() == nullptr);
			VERIFY(sw.size() == 0);
			VERIFY(sw.size() == sw.length());
		}

		// User-reported regression:  constructing string_view from a nullptr, NULL, 0
		{
			{
				StringViewT sw(nullptr);
				VERIFY(sw.empty());
				VERIFY(sw.data() == nullptr);
				VERIFY(sw.size() == 0);
				VERIFY(sw.size() == sw.length());
			}
			{
				StringViewT sw(0);
				VERIFY(sw.empty());
				VERIFY(sw.data() == nullptr);
				VERIFY(sw.size() == 0);
				VERIFY(sw.size() == sw.length());
			}
			{
				StringViewT sw(NULL);
				VERIFY(sw.empty());
				VERIFY(sw.data() == nullptr);
				VERIFY(sw.size() == 0);
				VERIFY(sw.size() == sw.length());
			}
		}

		// EA_CONSTEXPR basic_string_view(const basic_string_view& other) = default;
		{
			auto* pLiteral = LITERAL("Hello, World");
			StringViewT sw1(pLiteral);
			StringViewT sw2(sw1);
			VERIFY(sw1.size() == sw2.size());
			VERIFY(eastl::Compare(sw1.data(), sw2.data(), sw1.size()) == 0);
		}

		// EA_CONSTEXPR basic_string_view(const T* s, size_type count)
		{
			{
				StringViewT sw(LITERAL("Hello, World"), 12);
				VERIFY(!sw.empty());
				VERIFY(sw.data() != nullptr);
				VERIFY(sw.size() == 12);
				VERIFY(sw.size() == sw.length());
			}

			{
				StringViewT sw(LITERAL("Hello, World"), 5);
				VERIFY(!sw.empty());
				VERIFY(sw.data() != nullptr);
				VERIFY(sw.size() == 5);
				VERIFY(sw.size() == sw.length());
				VERIFY(eastl::Compare(sw.data(), LITERAL("Hello"), sw.size()) == 0);
			}
		}

		// EA_CONSTEXPR basic_string_view(const T* s)
		{
			auto* pLiteral = LITERAL("Vancouver, Canada");
			StringViewT sw(pLiteral);
			VERIFY(!sw.empty());
			VERIFY(sw.data() != nullptr);
			VERIFY(sw.size() == 17);
			VERIFY(sw.size() == sw.length());
			VERIFY(eastl::Compare(sw.data(), pLiteral, sw.size()) == 0);
		}

		// basic_string_view& operator=(const basic_string_view& view) = default;
		{
			auto* pLiteral = LITERAL("Hello, World");
			StringViewT sw1(pLiteral);
			StringViewT sw2;
			VERIFY(!sw1.empty());
			VERIFY(sw2.empty());

			sw2 = sw1;

			VERIFY(!sw1.empty());
			VERIFY(!sw2.empty());
			VERIFY(sw1.size() == sw2.size());
			VERIFY(eastl::Compare(sw1.data(), pLiteral, sw1.size()) == 0);
			VERIFY(eastl::Compare(sw2.data(), pLiteral, sw2.size()) == 0);
		}

		{
			// EA_CONSTEXPR const_iterator begin() const EA_NOEXCEPT
			// EA_CONSTEXPR const_iterator cbegin() const EA_NOEXCEPT
			StringViewT sw(LITERAL("abcdefg"));
			{
				auto i = sw.begin();
				auto ci = sw.cbegin();

				VERIFY(*i++ == LITERAL('a'));
				VERIFY(*i++ == LITERAL('b'));

				VERIFY(*ci++ == LITERAL('a'));
				VERIFY(*ci++ == LITERAL('b'));
			}

			// EA_CONSTEXPR const_iterator end() const EA_NOEXCEPT
			// EA_CONSTEXPR const_iterator cend() const EA_NOEXCEPT
			{
				auto i = sw.end();
				auto ci = sw.cend();

				VERIFY(*i-- == LITERAL('\0'));
				VERIFY(*i-- == LITERAL('g'));

				VERIFY(*ci-- == LITERAL('\0'));
				VERIFY(*ci-- == LITERAL('g'));
			}

			// EA_CONSTEXPR const_reverse_iterator rbegin() const EA_NOEXCEPT
			// EA_CONSTEXPR const_reverse_iterator crbegin() const EA_NOEXCEPT
			{
				auto i = sw.rbegin();
				auto ci = sw.crbegin();

				VERIFY(*i++ == LITERAL('g'));
				VERIFY(*i++ == LITERAL('f'));

				VERIFY(*ci++ == LITERAL('g'));
				VERIFY(*ci++ == LITERAL('f'));
			}

			// EA_CONSTEXPR const_reverse_iterator rend() const EA_NOEXCEPT
			// EA_CONSTEXPR const_reverse_iterator crend() const EA_NOEXCEPT
			{
				auto i = sw.rend();
				i--;

				auto ci = sw.crend();
				ci--;

				VERIFY(*i-- == LITERAL('a'));
				VERIFY(*i-- == LITERAL('b'));

				VERIFY(*ci-- == LITERAL('a'));
				VERIFY(*ci-- == LITERAL('b'));
			}
		}

		// EA_CONSTEXPR const_pointer data() const
		{
			auto* pLiteral = LITERAL("Vancouver, Canada");
			StringViewT sw(pLiteral);
			VERIFY(sw.data() != nullptr);
			VERIFY(eastl::Compare(sw.data(), pLiteral, sw.size()) == 0);
			VERIFY(eastl::Compare(sw.data() + 11, LITERAL("Canada"), 6) == 0);
		}

		// EA_CONSTEXPR const_reference front() const
		// EA_CONSTEXPR const_reference back() const
		{
			{
				StringViewT sw(LITERAL("Vancouver, Canada"));
				VERIFY(sw.front() == LITERAL('V'));
				VERIFY(sw.back() == LITERAL('a'));

			}
			{
				StringViewT sw(LITERAL("Canada"));
				VERIFY(sw.front() == LITERAL('C'));
				VERIFY(sw.back() == LITERAL('a'));
			}
		}

		// EA_CONSTEXPR const_reference operator[](size_type pos) const
		{
			StringViewT sw(LITERAL("Vancouver"));
			VERIFY(sw[0] == LITERAL('V'));
			VERIFY(sw[1] == LITERAL('a'));
			VERIFY(sw[2] == LITERAL('n'));
			VERIFY(sw[3] == LITERAL('c'));
			VERIFY(sw[4] == LITERAL('o'));
			VERIFY(sw[5] == LITERAL('u'));
			VERIFY(sw[6] == LITERAL('v'));
			VERIFY(sw[7] == LITERAL('e'));
			VERIFY(sw[8] == LITERAL('r'));
		}

		// EA_CONSTEXPR size_type size() const EA_NOEXCEPT 
		// EA_CONSTEXPR size_type length() const EA_NOEXCEPT
		// EA_CONSTEXPR size_type max_size() const EA_NOEXCEPT
		// EA_CONSTEXPR bool empty() const EA_NOEXCEPT 
		{
			StringViewT sw(LITERAL("http://en.cppreference.com/w/cpp/header/string_view"));
			VERIFY(sw.size() == 51); 
			VERIFY(sw.length() == 51); 
			VERIFY(sw.max_size() == eastl::numeric_limits<typename StringViewT::size_type>::max()); 
			VERIFY(!sw.empty()); 
		}

		// EA_CONSTEXPR void swap(basic_string_view& v)
		{
			auto* pV = LITERAL("Vancouver");
			auto* pC = LITERAL("Canada");
			StringViewT sw1(pV);
			StringViewT sw2(pC);
			sw1.swap(sw2);
			VERIFY(eastl::Compare(sw1.data(), pC, sw1.size()) == 0);
			VERIFY(eastl::Compare(sw2.data(), pV, sw2.size()) == 0);
		}

		// EA_CONSTEXPR void remove_prefix(size_type n)
		// EA_CONSTEXPR void remove_suffix(size_type n)
		{
			StringViewT sw(LITERAL("Vancouver"));
			sw.remove_prefix(3);
			VERIFY(eastl::Compare(sw.data(), LITERAL("couver"), sw.size()) == 0);
			VERIFY(sw.size() == 6);

			sw.remove_prefix(3);
			VERIFY(eastl::Compare(sw.data(), LITERAL("ver"), sw.size()) == 0);
			VERIFY(sw.size() == 3);

			sw.remove_suffix(1);
			VERIFY(eastl::Compare(sw.data(), LITERAL("ve"), sw.size()) == 0);
			VERIFY(sw.size() == 2);
			
			sw.remove_suffix(1);
			VERIFY(eastl::Compare(sw.data(), LITERAL("v"), sw.size()) == 0);
			VERIFY(sw.size() == 1);

			sw.remove_suffix(1);
			VERIFY(eastl::Compare(sw.data(), LITERAL(""), sw.size()) == 0);
			VERIFY(sw.size() == 0);
		}

		// size_type copy(T* s, size_type n, size_type pos = 0) const;
		{
			typename StringViewT::value_type buf[256];
			StringViewT sw(LITERAL("**Hello, World"));
			auto cnt = sw.copy(buf, 5, 2);
			VERIFY(eastl::Compare(buf, LITERAL("Hello"), 5) == 0);
			VERIFY(cnt == 5);
		}

		// EA_CONSTEXPR basic_string_view substr(size_type pos = 0, size_type n = npos) const;
		{
			StringViewT sw(LITERAL("**Hello, World"));
			auto sw2 = sw.substr(2, 5);
			VERIFY(eastl::Compare(sw2.data(), LITERAL("Hello"), sw2.size()) == 0);
		}

		// EA_CONSTEXPR int compare(basic_string_view s) const EA_NOEXCEPT;
		{
			{
				VERIFY(StringViewT(LITERAL("A")).compare(StringViewT(LITERAL("A"))) == 0);
				VERIFY(StringViewT(LITERAL("a")).compare(StringViewT(LITERAL("a"))) == 0);
				VERIFY(StringViewT(LITERAL("A")).compare(StringViewT(LITERAL("a"))) != 0);
				VERIFY(StringViewT(LITERAL("A")).compare(StringViewT(LITERAL("a"))) < 0);
				VERIFY(StringViewT(LITERAL("A")).compare(StringViewT(LITERAL("A"))) <= 0);
				VERIFY(StringViewT(LITERAL("a")).compare(StringViewT(LITERAL("A"))) > 0);
				VERIFY(StringViewT(LITERAL("A")).compare(StringViewT(LITERAL("A"))) >= 0);
			}

			{
				VERIFY(StringViewT(LITERAL("Aa")).compare(StringViewT(LITERAL("A"))) > 0);
				VERIFY(StringViewT(LITERAL("A")).compare(StringViewT(LITERAL("Aa"))) < 0);
			}

			{
				StringViewT sw1(LITERAL("Hello, World"));
				StringViewT sw2(LITERAL("Hello, WWorld"));
				StringViewT sw3(LITERAL("Hello, Wzorld"));
				VERIFY(sw1.compare(sw1) == 0);
				VERIFY(sw1.compare(sw2) > 0);
				VERIFY(sw1.compare(sw3) < 0);
			}
		}

		// EA_CONSTEXPR int compare(size_type pos1, size_type n1, basic_string_view s) const;
		{
			StringViewT sw1(LITERAL("*** Hello ***"));
			StringViewT sw2(LITERAL("Hello"));
			VERIFY(sw1.compare(4, 5, sw2) == 0);
		}

		// EA_CONSTEXPR int compare(size_type pos1, size_type n1, basic_string_view s, size_type pos2, size_type n2) const;
		{
			StringViewT sw(LITERAL("Vancouver"));
			VERIFY(sw.compare(0, 3, StringViewT(LITERAL("Van")), 0, 3) == 0);
			VERIFY(sw.compare(6, 3, StringViewT(LITERAL("ver")), 0, 3) == 0);
			VERIFY(sw.compare(0, 3, StringViewT(LITERAL("Tan")), 0, 3) != 0);
		}

		// EA_CONSTEXPR int compare(const T* s) const;
		{
			StringViewT sw(LITERAL("Hello"));
			VERIFY(sw.compare(LITERAL("Vancouver")) != 0);
			VERIFY(sw.compare(LITERAL("Vancouver!")) != 0);
			VERIFY(sw.compare(LITERAL("Hello")) == 0);
		}

		// EA_CONSTEXPR int compare(size_type pos1, size_type n1, const T* s) const;
		{
			StringViewT sw(LITERAL("*** Hello"));
			VERIFY(sw.compare(4, 5, LITERAL("Hello")) == 0);
			VERIFY(sw.compare(4, 5, LITERAL("Hello 555")) != 0);
			VERIFY(sw.compare(4, 5, LITERAL("hello")) != 0);
		}

		// EA_CONSTEXPR int compare(size_type pos1, size_type n1, const T* s, size_type n2) const;
		{
			StringViewT sw(LITERAL("*** Hello ***"));
			VERIFY(sw.compare(4, 5, LITERAL("Hello"), 5) == 0);
			VERIFY(sw.compare(0, 1, LITERAL("*"), 1) == 0);
			VERIFY(sw.compare(0, 2, LITERAL("**"), 1) != 0);
			VERIFY(sw.compare(0, 2, LITERAL("**"), 2) == 0);
			VERIFY(sw.compare(0, 2, LITERAL("^^"), 2) != 0);
		}


		// EA_CONSTEXPR size_type find(basic_string_view s, size_type pos = 0) const EA_NOEXCEPT;
		{
			StringViewT sw(LITERAL("*** Hello ***"));
			VERIFY(sw.find(StringViewT(LITERAL("Hello"))) != StringViewT::npos);
			VERIFY(sw.find(StringViewT(LITERAL("ell"))) != StringViewT::npos);
			VERIFY(sw.find(StringViewT(LITERAL("FailToFindMe"))) == StringViewT::npos);
		}

		// EA_CONSTEXPR size_type find(T c, size_type pos = 0) const EA_NOEXCEPT;
		{
			StringViewT sw(LITERAL("*** Hello ***"));
			VERIFY(sw.find(LITERAL("H")) == 4);
			VERIFY(sw.find(LITERAL("e")) == 5);
			VERIFY(sw.find(LITERAL("l")) == 6);
			VERIFY(sw.find(LITERAL("o")) == 8);
			VERIFY(sw.find(LITERAL("&")) == StringViewT::npos);
			VERIFY(sw.find(LITERAL("@")) == StringViewT::npos);
		}

		// EA_CONSTEXPR size_type find(const T* s, size_type pos, size_type n) const;
		{
			StringViewT sw(LITERAL("Hello, Vancouver"));
			VERIFY(sw.find(LITERAL("Hello"), 0, 3) != StringViewT::npos);
			VERIFY(sw.find(LITERAL("Hello"), 3, 3) == StringViewT::npos);
			VERIFY(sw.find(LITERAL("Vancouv"), 7, 7) != StringViewT::npos);
		}

		// EA_CONSTEXPR size_type find(const T* s, size_type pos = 0) const;
		{
			StringViewT sw(LITERAL("Hello, Vancouver"));
			VERIFY(sw.find(LITERAL("Hello"), 0) != StringViewT::npos);
			VERIFY(sw.find(LITERAL("Hello"), 3) == StringViewT::npos);
			VERIFY(sw.find(LITERAL("Vancouv"), 7) != StringViewT::npos);
		}


		// EA_CONSTEXPR size_type rfind(basic_string_view s, size_type pos = npos) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type rfind(T c, size_type pos = npos) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type rfind(const T* s, size_type pos, size_type n) const;
		// EA_CONSTEXPR size_type rfind(const T* s, size_type pos = npos) const;
		{
			StringViewT str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

			VERIFY(str.rfind(StringViewT(LITERAL("d")))    != StringViewT::npos);
			VERIFY(str.rfind(StringViewT(LITERAL("tuv")))  != StringViewT::npos);
			VERIFY(str.rfind(StringViewT(LITERAL("123r"))) == StringViewT::npos);

			VERIFY(str.rfind(LITERAL("d"))    != StringViewT::npos);
			VERIFY(str.rfind(LITERAL("tuv"))  != StringViewT::npos);
			VERIFY(str.rfind(LITERAL("123r")) == StringViewT::npos);

			VERIFY(str.rfind(LITERAL("d"), str.length())        != StringViewT::npos);
			VERIFY(str.rfind(LITERAL("tuv"), str.length() - 2)  != StringViewT::npos);
			VERIFY(str.rfind(LITERAL("123r"), str.length() - 2) == StringViewT::npos);

			VERIFY(str.rfind(LITERAL('d'), str.length() - 0) != StringViewT::npos);
			VERIFY(str.rfind(LITERAL('t'), str.length() - 2) != StringViewT::npos);
			VERIFY(str.rfind(LITERAL('1'), str.length() - 2) == StringViewT::npos);
		}

		// EA_CONSTEXPR size_type find_first_of(basic_string_view s, size_type pos = 0) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type find_first_of(T c, size_type pos = 0) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type find_first_of(const T* s, size_type pos, size_type n) const;
		// EA_CONSTEXPR size_type find_first_of(const T* s, size_type pos = 0) const;
		{
			StringViewT str(LITERAL("aaaaabbbbbcccdddddeeeeefffggh"));

			VERIFY(str.find_first_of(StringViewT(LITERAL("aaa"))) == 0);
			VERIFY(str.find_first_of(LITERAL("aab")) == 0);
			VERIFY(str.find_first_of(LITERAL("baab")) == 0);
			VERIFY(str.find_first_of(LITERAL("ceg")) == 10);
			VERIFY(str.find_first_of(LITERAL("eeef"), 1, 2) == 18);
			VERIFY(str.find_first_of(LITERAL("eeef"), 1, 4) == 18);
			VERIFY(str.find_first_of(LITERAL('g')) == 26);
			VERIFY(str.find_first_of(LITERAL('$')) == StringViewT::npos);
			VERIFY(str.find_first_of(StringViewT(LITERAL(" a"), 1)) == StringViewT::npos);
		}

		// EA_CONSTEXPR size_type find_last_of(basic_string_view s, size_type pos = npos) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type find_last_of(T c, size_type pos = npos) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type find_last_of(const T* s, size_type pos, size_type n) const;
		// EA_CONSTEXPR size_type find_last_of(const T* s, size_type pos = npos) const;
		{
			StringViewT str(LITERAL("aaaaabbbbbcccdddddeeeeefffggh"));

			VERIFY(str.find_last_of(StringViewT(LITERAL("aaa"))) == 4);
			VERIFY(str.find_last_of(LITERAL("aab")) == 9);
			VERIFY(str.find_last_of(LITERAL("baab")) == 9);
			VERIFY(str.find_last_of(LITERAL("ceg")) == 27);
			// VERIFY(str.find_last_of(LITERAL("eeef"), 1, 2) == StringViewT::npos);  // todo: FIX ME
			// VERIFY(str.find_last_of(LITERAL("eeef"), 1, 4) == StringViewT::npos);  // todo: FIX ME
			VERIFY(str.find_last_of(LITERAL('g')) == 27);
			VERIFY(str.find_last_of(LITERAL('$')) == StringViewT::npos);
		}

		// EA_CONSTEXPR size_type find_first_not_of(basic_string_view s, size_type pos = 0) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type find_first_not_of(T c, size_type pos = 0) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type find_first_not_of(const T* s, size_type pos, size_type n) const;
		// EA_CONSTEXPR size_type find_first_not_of(const T* s, size_type pos = 0) const;
		{
			StringViewT str(LITERAL("aaaaabbbbbcccdddddeeeeefffggh"));

			VERIFY(str.find_first_not_of(StringViewT(LITERAL("abcdfg"))) == 18);
			VERIFY(str.find_first_not_of(LITERAL("abcdfg")) == 18);
			// VERIFY(str.find_first_not_of(LITERAL("abcdfg"), 2, 2) == 0);   // todo:  FIX ME 
			// VERIFY(str.find_first_not_of(LITERAL("abcdfg"), 0, 2) == 10);  // todo:  FIX ME 
			VERIFY(str.find_first_not_of(LITERAL('a')) == 5);  
		}


		// EA_CONSTEXPR size_type find_last_not_of(basic_string_view s, size_type pos = npos) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type find_last_not_of(T c, size_type pos = npos) const EA_NOEXCEPT;
		// EA_CONSTEXPR size_type find_last_not_of(const T* s, size_type pos, size_type n) const;
		// EA_CONSTEXPR size_type find_last_not_of(const T* s, size_type pos = npos) const;
		{
			StringViewT str(LITERAL("aaaaabbbbbcccdddddeeeeefffggh"));

			VERIFY(str.find_last_not_of(StringViewT(LITERAL("a"))) == 28);
			VERIFY(str.find_last_not_of(StringViewT(LITERAL("abcdfg"))) == 28);
			VERIFY(str.find_last_not_of(StringViewT(LITERAL("abcdfgh"))) == 22);
			VERIFY(str.find_last_not_of(LITERAL("abcdfgh")) == 22);
			// VERIFY(str.find_last_not_of(LITERAL("abcdfg"), 2, 2) == 0);   // todo:  FIX ME 
			// VERIFY(str.find_last_not_of(LITERAL("abcdfg"), 0, 2) == 10);  // todo:  FIX ME 
			VERIFY(str.find_last_not_of(LITERAL('a')) == 28);  
		}

		// template <class CharT, class Traits>
		// constexpr bool operator==(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs);
		// template <class CharT, class Traits>
		// constexpr bool operator!=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs);
		// template <class CharT, class Traits>
		// constexpr bool operator<(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs);
		// template <class CharT, class Traits>
		// constexpr bool operator<=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs);
		// template <class CharT, class Traits>
		// constexpr bool operator>(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs);
		// template <class CharT, class Traits>
		// constexpr bool operator>=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs);
		{
			StringViewT sw1(LITERAL("AAAAABBBBBCCCDDDDDEEEEEFFFGGH"));
			StringViewT sw2(LITERAL("aaaaabbbbbcccdddddeeeeefffggh"));
			StringViewT sw3(LITERAL("0123456789!@#$%^&*()"));

			VERIFY(sw1 == StringViewT(LITERAL("AAAAABBBBBCCCDDDDDEEEEEFFFGGH")));
			VERIFY(sw1 != StringViewT(LITERAL("abcdefghijklmnopqrstuvwxyz")));
			VERIFY(sw1 < sw2);
			VERIFY(sw1 <= sw2);
			VERIFY(sw2 > sw1);
			VERIFY(sw2 >= sw1);
		}

		// template<> struct hash<std::string_view>;
		// template<> struct hash<std::wstring_view>;
		// template<> struct hash<std::u16string_view>;
		// template<> struct hash<std::u32string_view>;
		{
			StringViewT sw1(LITERAL("Hello, World"));
			StringViewT sw2(LITERAL("Hello, World"), 5);
			StringViewT sw3(LITERAL("Hello"));
			auto s = LITERAL("Hello");

			VERIFY(eastl::hash<StringViewT>{}(sw1) != eastl::hash<StringViewT>{}(sw2));
			VERIFY(eastl::hash<StringViewT>{}(sw2) == eastl::hash<StringViewT>{}(sw3));
			VERIFY(eastl::hash<StringViewT>{}(sw3) == eastl::hash<decltype(s)>{}(s));
		}
	}

	{
		StringViewT sw1(LITERAL("AAAAABBBBBCCCDDDDDEEEEEFFFGGH"));

		VERIFY( sw1.starts_with(LITERAL('A')));
		VERIFY(!sw1.starts_with(LITERAL('X')));
		VERIFY( sw1.starts_with(LITERAL("AAAA")));
		VERIFY( sw1.starts_with(StringViewT(LITERAL("AAAA"))));
		VERIFY(!sw1.starts_with(LITERAL("AAAB")));

		VERIFY( sw1.ends_with(LITERAL('H')));
		VERIFY(!sw1.ends_with(LITERAL('X')));
		VERIFY( sw1.ends_with(LITERAL("FGGH")));
		VERIFY( sw1.ends_with(StringViewT(LITERAL("FGGH"))));
		VERIFY(!sw1.ends_with(LITERAL("FGGH$")));
	}

	return nErrorCount;
}

// Required to prevent manual undef of macros when 'TestString.inl' preprocessed at the top of the unit test cpp file.
#undef TEST_STRING_NAME 
#undef LITERAL


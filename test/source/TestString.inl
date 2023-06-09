/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// todo:
// Test Encoding
// Test StringHash
// Test exceptions

#if EASTL_OPENSOURCE
	#define EASTL_SNPRINTF_TESTS_ENABLED 0
#else
	#define EASTL_SNPRINTF_TESTS_ENABLED 1
#endif


template<typename StringType>
int TEST_STRING_NAME()
{
	int nErrorCount = 0;

	struct Failocator
	{
		Failocator() = default;
		Failocator(const char*) {}

		void* allocate(size_t) { EA_FAIL(); return nullptr; }
		void deallocate(void*, size_t) { EA_FAIL(); }
	};

	#if defined(EA_PLATFORM_ANDROID)
		EA_DISABLE_CLANG_WARNING(-Wunknown-warning-option)  // warning: disable unknown warning suppression pragmas
		EA_DISABLE_CLANG_WARNING(-Wunknown-pragmas)         // warning: disable unknown warning suppression pragmas
		EA_DISABLE_CLANG_WARNING(-Winherited-variadic-ctor) // warning: inheriting constructor does not inherit ellipsis
	#endif

	struct SSOStringType : public StringType
	{
		using StringType::StringType;
		using StringType::IsSSO;
	};

	// Use custom string type that always fails to allocate memory to highlight when SSO is not functioning correctly.
	struct SSOFailocatorString : public eastl::basic_string<typename StringType::value_type, Failocator>
	{
		using eastl::basic_string<typename StringType::value_type, Failocator>::basic_string;
		using eastl::basic_string<typename StringType::value_type, Failocator>::IsSSO;
	};

	#if defined(EA_PLATFORM_ANDROID)
		EA_RESTORE_CLANG_WARNING()
		EA_RESTORE_CLANG_WARNING()
		EA_RESTORE_CLANG_WARNING()
	#endif

	// SSO (short string optimization) tests
	{
		{
			SSOFailocatorString str;
			VERIFY(str.validate());
			VERIFY(str.empty());
			VERIFY(str.IsSSO());
		}

		EA_CONSTEXPR_IF(EA_PLATFORM_WORD_SIZE == 8 && EASTL_SIZE_T_32BIT == 0)
		{
			// test SSO size on 64 bit platforms
			EA_CONSTEXPR_IF(sizeof(typename StringType::value_type) == 1)
			{
				// we can fit 23 characters on 64bit system with 1 byte chars
				const auto* pLiteral = LITERAL("aaaaaaaaaaaaaaaaaaaaaaa");
				SSOFailocatorString str(pLiteral);

				VERIFY(EA::StdC::Strlen(pLiteral) == 23);
				VERIFY(str == pLiteral);
				VERIFY(str.validate());
				VERIFY(str.IsSSO());
			}

			EA_CONSTEXPR_IF(sizeof(typename StringType::value_type) == 2)
			{
				// we can fit 11 characters on 64 bit system with 2 byte chars
				const auto* pLiteral = LITERAL("aaaaaaaaaaa");
				SSOFailocatorString str(pLiteral);

				VERIFY(EA::StdC::Strlen(pLiteral) == 11);
				VERIFY(str == pLiteral);
				VERIFY(str.validate());
				VERIFY(str.IsSSO());
			}

			EA_CONSTEXPR_IF(sizeof(typename StringType::value_type) == 4)
			{
				// we can fit 5 characters on 64 bit system with 4 byte chars
				const auto* pLiteral = LITERAL("aaaaa");
				SSOFailocatorString str(pLiteral);

				VERIFY(EA::StdC::Strlen(pLiteral) == 5);
				VERIFY(str == pLiteral);
				VERIFY(str.validate());
				VERIFY(str.IsSSO());
			}
		}

		EA_CONSTEXPR_IF(EA_PLATFORM_WORD_SIZE == 4)
		{
			// test SSO size on 32 bit platforms
			EA_CONSTEXPR_IF(sizeof(typename StringType::value_type) == 1)
			{
				// we can fit 11 characters on 32bit system with 1 byte chars
				const auto* pLiteral = LITERAL("aaaaaaaaaaa");
				SSOFailocatorString str(pLiteral);

				VERIFY(EA::StdC::Strlen(pLiteral) == 11);
				VERIFY(str == pLiteral);
				VERIFY(str.validate());
				VERIFY(str.IsSSO());
			}

			EA_CONSTEXPR_IF(sizeof(typename StringType::value_type) == 2)
			{
				// we can fit 5 characters on 32 bit system with 2 byte chars
				const auto* pLiteral = LITERAL("aaaaa");
				SSOFailocatorString str(pLiteral);

				VERIFY(EA::StdC::Strlen(pLiteral) == 5);
				VERIFY(str == pLiteral);
				VERIFY(str.validate());
				VERIFY(str.IsSSO());
			}

			EA_CONSTEXPR_IF(sizeof(typename StringType::value_type) == 4)
			{
				// we can fit 2 characters on 32 bit system with 4 byte chars
				const auto* pLiteral = LITERAL("aa");
				SSOFailocatorString str(pLiteral);

				VERIFY(EA::StdC::Strlen(pLiteral) == 2);
				VERIFY(str == pLiteral);
				VERIFY(str.validate());
				VERIFY(str.IsSSO());
			}
		}
	}

	// basic_string();
	{
		StringType str;
		VERIFY(str.empty());
		VERIFY(str.length() == 0);
		VERIFY(str.validate());
	}

	// explicit basic_string(const allocator_type& allocator);
	{
		typename StringType::allocator_type alloc;
		StringType str(alloc);
		VERIFY(str.validate());
	}

	// basic_string(const value_type* p, size_type n, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
	{
		{
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"), 26);
			VERIFY(str[5] == LITERAL('f'));
			VERIFY(!str.empty());
			VERIFY(str.length() == 26);
			VERIFY(str.validate());
		}

		{
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			VERIFY(str[5] == LITERAL('f'));
			VERIFY(!str.empty());
			VERIFY(str.length() == 26);
			VERIFY(str.validate());
		}
	}

	// basic_string(const this_type& x, size_type position, size_type n = npos);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		StringType str2(str1, 3, 3);
		VERIFY(str2 == LITERAL("def"));
		VERIFY(str2.size() == 3);
		VERIFY(str2.length() == 3);
		VERIFY(str2.capacity() >= 3);  // SSO buffer size

		StringType str3(str1, 25, 3);
		VERIFY(str3 == LITERAL("z"));
		VERIFY(str3.size() == 1);
		VERIFY(str3.length() == 1);
		VERIFY(str3.capacity() >= 1); // SSO buffer size

		VERIFY(str1.validate());
		VERIFY(str2.validate());
		VERIFY(str3.validate());
	}

	// EASTL_STRING_EXPLICIT basic_string(const value_type* p, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
	{
		auto* pLiteral = LITERAL("abcdefghijklmnopqrstuvwxyz");
		StringType str(pLiteral);
		VERIFY(str == pLiteral);
	}

	// basic_string(size_type n, value_type c, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
	{
		StringType str(32, LITERAL('a'));
		VERIFY(!str.empty());
		VERIFY(str.size() == 32);
		VERIFY(str.length() == 32);
		VERIFY(str == LITERAL("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));

		VERIFY(str.validate());
	}

	// basic_string(const this_type& x);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		StringType str2(str1);

		VERIFY(str1 == str2);
		VERIFY(str1.size() == str2.size());
		VERIFY(str1.empty() == str2.empty());
		VERIFY(str1.length() == str2.length());
		VERIFY(EA::StdC::Memcmp(str1.data(), str2.data(), str1.size()) == 0);

		VERIFY(str1.validate());
		VERIFY(str2.validate());
	}

	// basic_string(const value_type* pBegin, const value_type* pEnd, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		auto* pStart = str1.data() + 5;
		auto* pEnd = str1.data() + 20;

		StringType str(pStart, pEnd);
		VERIFY(str == LITERAL("fghijklmnopqrst"));
		VERIFY(!str.empty());
		VERIFY(str.size() == 15);
	}

	// basic_string(CtorDoNotInitialize, size_type n, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
	{
		StringType str(typename StringType::CtorDoNotInitialize(), 42);
		VERIFY(str.size() == 0);
		VERIFY(str.length() == 0);
		VERIFY(str.capacity() == 42);
	}

	// basic_string(CtorSprintf, const value_type* pFormat, ...);
	{
	#if EASTL_SNPRINTF_TESTS_ENABLED
		{
			StringType str(typename StringType::CtorSprintf(), LITERAL("Hello, %d"), 42);
			VERIFY(str == LITERAL("Hello, 42"));
			VERIFY(str.validate());
		}

		{
			StringType str(typename StringType::CtorSprintf(), LITERAL("Hello, %d %d %d %d %d %d %d %d %d"), 42, 42, 42, 42, 42, 42, 42, 42, 42);
			VERIFY(str == LITERAL("Hello, 42 42 42 42 42 42 42 42 42"));
			VERIFY(str.validate());
		}

		{
			constexpr eastl_ssize_t signedValue = -42;
			StringType str(typename StringType::CtorSprintf(), LITERAL("Hello, %" EASTL_PRIdSIZE), signedValue);
			VERIFY(str == LITERAL("Hello, -42"));
			VERIFY(str.validate());
		}

		{
			constexpr eastl_ssize_t signedValue = -42;
			StringType str(typename StringType::CtorSprintf(), LITERAL("Hello, %" EASTL_PRIiSIZE), signedValue);
			VERIFY(str == LITERAL("Hello, -42"));
			VERIFY(str.validate());
		}

		{
			constexpr eastl_size_t unsignedValue = 42u;
			StringType str(typename StringType::CtorSprintf(), LITERAL("Hello, 0%" EASTL_PRIoSIZE), unsignedValue);
			VERIFY(str == LITERAL("Hello, 052"));
			VERIFY(str.validate());
		}

		{
			constexpr eastl_size_t unsignedValue = 42u;
			StringType str(typename StringType::CtorSprintf(), LITERAL("Hello, %" EASTL_PRIuSIZE), unsignedValue);
			VERIFY(str == LITERAL("Hello, 42"));
			VERIFY(str.validate());
		}

		{
			constexpr eastl_size_t unsignedValue = 42u;
			StringType str(typename StringType::CtorSprintf(), LITERAL("Hello, 0x%" EASTL_PRIxSIZE), unsignedValue);
			VERIFY(str == LITERAL("Hello, 0x2a"));
			VERIFY(str.validate());
		}

		{
			constexpr eastl_size_t unsignedValue = 42u;
			StringType str(typename StringType::CtorSprintf(), LITERAL("Hello, 0x%" EASTL_PRIXSIZE), unsignedValue);
			VERIFY(str == LITERAL("Hello, 0x2A"));
			VERIFY(str.validate());
		}
	#endif
	}

	// basic_string(std::initializer_list<value_type> init, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
	{
	#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		StringType str({'a','b','c','d','e','f'});
		VERIFY(str == LITERAL("abcdef"));
		VERIFY(!str.empty());
		VERIFY(str.length() == 6);
		VERIFY(str.size() == 6);
		VERIFY(str.validate());
	#endif
	}

	// basic_string(this_type&& x);
	// basic_string(this_type&& x, const allocator_type& allocator);
	{  // test heap string
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		StringType str2(eastl::move(str1));

		VERIFY(str1 != LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(str2 == LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str1.empty());
		VERIFY(!str2.empty());

		VERIFY(str1.length() == 0);
		VERIFY(str2.length() == 26);

		VERIFY(str1.size() == 0);
		VERIFY(str2.size() == 26);

		VERIFY(str1.validate());
		VERIFY(str2.validate());
	}
	{  // test sso string
		StringType str1(LITERAL("a"));
		StringType str2(eastl::move(str1));

		VERIFY(str1 != LITERAL("a"));
		VERIFY(str2 == LITERAL("a"));

		VERIFY(str1.empty());
		VERIFY(!str2.empty());

		VERIFY(str1.length() == 0);
		VERIFY(str2.length() == 1);

		VERIFY(str1.size() == 0);
		VERIFY(str2.size() == 1);

		VERIFY(str1.validate());
		VERIFY(str2.validate());
	}

	// basic_string(const view_type& sv, const allocator_type& allocator);
	// basic_string(const view_type& sv, size_type position, size_type n, const allocator_type& allocator);
	{
		{ // test string_view
			typename StringType::view_type sv(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			StringType str(sv);

			VERIFY(str == LITERAL("abcdefghijklmnopqrstuvwxyz"));
			VERIFY(!str.empty());
			VERIFY(str.length() == 26);
			VERIFY(str.size() == 26);
			VERIFY(str.validate());
		}

		{  // test string_view substring
			typename StringType::view_type sv(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			StringType str(sv, 2, 22);

			VERIFY(str == LITERAL("cdefghijklmnopqrstuvwx"));
			VERIFY(!str.empty());
			VERIFY(str.length() == 22);
			VERIFY(str.size() == 22);
			VERIFY(str.validate());
		}
	}

	// template <typename OtherCharType>
	// basic_string(CtorConvert, const OtherCharType* p, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
	{
		{
		#if defined(EA_CHAR8)
			StringType str(typename StringType::CtorConvert(), EA_CHAR8("123456789"));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR16)
			StringType str(typename StringType::CtorConvert(), EA_CHAR16("123456789"));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR32)
			StringType str(typename StringType::CtorConvert(), EA_CHAR32("123456789"));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_WCHAR)
		    StringType str(typename StringType::CtorConvert(), EA_WCHAR("123456789"));
		    VERIFY(str == LITERAL("123456789"));
		    VERIFY(str.validate());
		#endif
		}
	}

	// template <typename OtherCharType>
	// basic_string(CtorConvert, const OtherCharType* p, size_type n, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
	{
		{
		#if defined(EA_CHAR8)
			StringType str(typename StringType::CtorConvert(), EA_CHAR8("123456789"), 4);
			VERIFY(str == LITERAL("1234"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR16)
			StringType str(typename StringType::CtorConvert(), EA_CHAR16("123456789"), 4);
			VERIFY(str == LITERAL("1234"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR32)
			StringType str(typename StringType::CtorConvert(), EA_CHAR32("123456789"), 4);
			VERIFY(str == LITERAL("1234"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_WCHAR)
		    StringType str(typename StringType::CtorConvert(), EA_WCHAR("123456789"), 4);
		    VERIFY(str == LITERAL("1234"));
		    VERIFY(str.validate());
		#endif
		}
	}

	// template <typename OtherStringType>
	// basic_string(CtorConvert, const OtherStringType& x);
	{
		{
		#if defined(EA_CHAR8)
			StringType str(typename StringType::CtorConvert(), eastl::basic_string<char8_t, typename StringType::allocator_type>(EA_CHAR8("123456789")));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR16)
			StringType str(typename StringType::CtorConvert(), eastl::basic_string<char16_t, typename StringType::allocator_type>(EA_CHAR16("123456789")));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR32)
			StringType str(typename StringType::CtorConvert(), eastl::basic_string<char32_t, typename StringType::allocator_type>(EA_CHAR32("123456789")));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_WCHAR)
		    StringType str(typename StringType::CtorConvert(), eastl::basic_string<wchar_t, typename StringType::allocator_type>(EA_WCHAR("123456789")));
		    VERIFY(str == LITERAL("123456789"));
		    VERIFY(str.validate());
		#endif
		}
	}

	// const allocator_type& get_allocator() const EA_NOEXCEPT;
	// allocator_type&       get_allocator() EA_NOEXCEPT;
	// void                  set_allocator(const allocator_type& allocator);
	{
	}

	// this_type& operator=(const this_type& x);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		StringType str1_copy(LITERAL(""));

		VERIFY(str1_copy.empty());

		str1_copy = str1;

		VERIFY(str1 == str1_copy);
		VERIFY(!str1_copy.empty());
		VERIFY(str1.validate());
		VERIFY(str1_copy.validate());
	}

	// this_type& operator=(const value_type* p);
	{
		StringType str;
		str = LITERAL("abcdefghijklmnopqrstuvwxyz");

		VERIFY(str[5] == LITERAL('f'));
		VERIFY(str == LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(!str.empty());
		VERIFY(str.length() == 26);
		VERIFY(str.validate());
	}

	// this_type& operator=(value_type c);
	{
		StringType str;
		str = LITERAL('a');

		VERIFY(str == LITERAL("a"));
		VERIFY(!str.empty());
		VERIFY(str.length() == 1);
		VERIFY(str.size() == 1);
		VERIFY(str.validate());
	}

	// this_type& operator=(std::initializer_list<value_type> ilist);
	{
	#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		StringType str = {'a','b','c','d','e','f'};

		VERIFY(str == LITERAL("abcdef"));
		VERIFY(!str.empty());
		VERIFY(str.length() == 6);
		VERIFY(str.size() == 6);
		VERIFY(str.validate());
	#endif
	}

	// this_type& operator=(this_type&& x);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		StringType str2 = eastl::move(str1);

		VERIFY(str1 != LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(str2 == LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str1.empty());
		VERIFY(!str2.empty());

		VERIFY(str1.length() == 0);
		VERIFY(str2.length() == 26);

		VERIFY(str1.size() == 0);
		VERIFY(str2.size() == 26);

		VERIFY(str1.validate());
		VERIFY(str2.validate());
	}
	{
		StringType str1(LITERAL("a"));
		StringType str2 = eastl::move(str1);

		VERIFY(str1 != LITERAL("a"));
		VERIFY(str2 == LITERAL("a"));

		VERIFY(str1.empty());
		VERIFY(!str2.empty());

		VERIFY(str1.length() == 0);
		VERIFY(str2.length() == 1);

		VERIFY(str1.size() == 0);
		VERIFY(str2.size() == 1);

		VERIFY(str1.validate());
		VERIFY(str2.validate());
	}

	//     this_type& operator=(value_type* p);
	//
	//     template <typename OtherCharType>
	//     this_type& operator=(const OtherCharType* p);
	//
	//     template <typename OtherStringType>
	//     this_type& operator=(const OtherStringType& x);
	{
	#if EASTL_OPERATOR_EQUALS_OTHER_ENABLED
		{
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			str = LITERAL("123456789");
			VERIFY(str == LITERAL("123456789");
			VERIFY(str.validate());
		}
		{
			{
			#if defined(EA_CHAR8)
				StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
				str = EA_CHAR8("123456789");
				VERIFY(str == LITERAL("123456789"));
				VERIFY(str.validate());
			#endif
			}
			{
			#if defined(EA_CHAR16)
				StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
				str = EA_CHAR16("123456789");
				VERIFY(str == LITERAL("123456789"));
				VERIFY(str.validate());
			#endif
			}
			{
			#if defined(EA_CHAR32)
				StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
				str = EA_CHAR32("123456789");
				VERIFY(str == LITERAL("123456789"));
				VERIFY(str.validate());
			#endif
			}
			{
			#if defined(EA_WCHAR)
				StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
				str = EA_WCHAR("123456789");
				VERIFY(str == LITERAL("123456789"));
				VERIFY(str.validate());
			#endif
			}
		}
		{
			{
			#if defined(EA_CHAR8)
				StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
				str = eastl::basic_string<char8_t>(EA_CHAR8("123456789"));
				VERIFY(str == LITERAL("123456789"));
				VERIFY(str.validate());
			#endif
			}
			{
			#if defined(EA_CHAR16)
				StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
				str = eastl::basic_string<char16_t>(EA_CHAR16("123456789"));
				VERIFY(str == LITERAL("123456789"));
				VERIFY(str.validate());
			#endif
			}
			{
			#if defined(EA_CHAR32)
				StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
				str = eastl::basic_string<char32_t>(EA_CHAR32("123456789"));
				VERIFY(str == LITERAL("123456789"));
				VERIFY(str.validate());
			#endif
			}
			{
			#if defined(EA_WCHAR)
				StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
				str = eastl::basic_string<wchar_t>(EA_WCHAR("123456789"));
				VERIFY(str == LITERAL("123456789"));
				VERIFY(str.validate());
			#endif
			}
		}
    #endif
	}

	// void swap(this_type& x);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		StringType str2;

		str1.swap(str2);

		VERIFY(str1 != LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(str2 == LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str1.empty());
		VERIFY(!str2.empty());

		VERIFY(str1.length() == 0);
		VERIFY(str2.length() == 26);
		VERIFY(str1.size() == 0);
		VERIFY(str2.size() == 26);

		VERIFY(str1.validate());
		VERIFY(str2.validate());
	}

	// this_type& assign(const this_type& x);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		StringType str2;

		str2.assign(str1);

		VERIFY(str1 == LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(str2 == LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(!str1.empty());
		VERIFY(!str2.empty());

		VERIFY(str1.length() == 26);
		VERIFY(str2.length() == 26);
		VERIFY(str1.size() == 26);
		VERIFY(str2.size() == 26);

		VERIFY(str1.validate());
		VERIFY(str2.validate());
	}

	// this_type& assign(const this_type& x, size_type position, size_type n);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		StringType str2(LITERAL("123456789"));

		str1.assign(str2, 3, 3);

		VERIFY(str1 == LITERAL("456"));
		VERIFY(str1.validate());
		VERIFY(str2.validate());
	}

	// this_type& assign(const value_type* p, size_type n);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		str.assign(LITERAL("123456789"), 5);

		VERIFY(str == LITERAL("12345"));
		VERIFY(str.validate());
	}

	// this_type& assign(const value_type* p);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		str.assign(LITERAL("123"));

		VERIFY(str == LITERAL("123"));
		VERIFY(str.validate());
	}

	// this_type& assign(size_type n, value_type c);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		str.assign(32, LITERAL('c'));

		VERIFY(str == LITERAL("cccccccccccccccccccccccccccccccc"));
		VERIFY(str.validate());
	}

	// this_type& assign(const value_type* pBegin, const value_type* pEnd);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		auto* pLiteral = LITERAL("0123456789");
		auto* pBegin = pLiteral + 4;
		auto* pEnd = pLiteral + 7;

		str.assign(pBegin, pEnd);

		VERIFY(str == LITERAL("456"));
		VERIFY(str.validate());
	}

	// this_type& assign(this_type&& x);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		StringType str2;

		str1.assign(eastl::move(str2));

		VERIFY(str1 != LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(str2 == LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str1.empty());
		VERIFY(!str2.empty());

		VERIFY(str1.length() == 0);
		VERIFY(str2.length() == 26);
		VERIFY(str1.size() == 0);
		VERIFY(str2.size() == 26);

		VERIFY(str1.validate());
		VERIFY(str2.validate());
	}

	// this_type& assign(std::initializer_list<value_type>);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		str.assign({'1','2','3'});

		VERIFY(str == LITERAL("123"));
		VERIFY(str.validate());
	}

	// template <typename OtherCharType>
	// this_type& assign_convert(const OtherCharType* p);
	{
		{
		#if defined(EA_CHAR8)
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			str.assign_convert(EA_CHAR8("123456789"));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR16)
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			str.assign_convert(EA_CHAR16("123456789"));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR32)
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			str.assign_convert(EA_CHAR32("123456789"));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_WCHAR)
		    StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		    str.assign_convert(EA_WCHAR("123456789"));
		    VERIFY(str == LITERAL("123456789"));
		    VERIFY(str.validate());
		#endif
		}
	}

	// template <typename OtherCharType>
	// this_type& assign_convert(const OtherCharType* p, size_type n);
	{
		{
		#if defined(EA_CHAR8)
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			str.assign_convert(EA_CHAR8("123456789"), 3);
			VERIFY(str == LITERAL("123"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR16)
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			str.assign_convert(EA_CHAR16("123456789"), 3);
			VERIFY(str == LITERAL("123"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR32)
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			str.assign_convert(EA_CHAR32("123456789"), 3);
			VERIFY(str == LITERAL("123"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_WCHAR)
		    StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		    str.assign_convert(EA_WCHAR("123456789"), 3);
		    VERIFY(str == LITERAL("123"));
		    VERIFY(str.validate());
		#endif
		}
	}

	// template <typename OtherStringType>
	// this_type& assign_convert(const OtherStringType& x);
	{
		{
		#if defined(EA_CHAR8)
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			eastl::basic_string<char8_t> str2(EA_CHAR8("123456789"));

			str.assign_convert(str2);
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR16)
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			eastl::basic_string<char16_t> str2(EA_CHAR16("123456789"));

			str.assign_convert(str2);
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR32)
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			eastl::basic_string<char32_t> str2(EA_CHAR32("123456789"));

			str.assign_convert(str2);
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_WCHAR)
		    StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		    eastl::basic_string<wchar_t> str2(EA_WCHAR("123456789"));

		    str.assign_convert(str2);
		    VERIFY(str == LITERAL("123456789"));
		    VERIFY(str.validate());
		#endif
		}
	}

	// iterator       begin() EA_NOEXCEPT;
	// const_iterator begin() const EA_NOEXCEPT;
	// const_iterator cbegin() const EA_NOEXCEPT;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		auto iBegin = str.begin();

		VERIFY(*iBegin++ == LITERAL('a'));
		VERIFY(*iBegin++ == LITERAL('b'));
		VERIFY(*iBegin++ == LITERAL('c'));
		VERIFY(*iBegin++ == LITERAL('d'));
		VERIFY(*iBegin++ == LITERAL('e'));
		VERIFY(*iBegin++ == LITERAL('f'));
		VERIFY(*(str.begin() + 25) == LITERAL('z'));

	}

	// iterator       end() EA_NOEXCEPT;
	// const_iterator end() const EA_NOEXCEPT;
	// const_iterator cend() const EA_NOEXCEPT;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		auto iEnd = str.end()-1;

		VERIFY(*iEnd-- == LITERAL('z'));
		VERIFY(*iEnd-- == LITERAL('y'));
		VERIFY(*iEnd-- == LITERAL('x'));
		VERIFY(*iEnd-- == LITERAL('w'));
		VERIFY(*iEnd-- == LITERAL('v'));
		VERIFY(*iEnd-- == LITERAL('u'));
		VERIFY(*(str.end() - 26) == LITERAL('a'));
	}

	// reverse_iterator       rbegin() EA_NOEXCEPT;
	// const_reverse_iterator rbegin() const EA_NOEXCEPT;
	// const_reverse_iterator crbegin() const EA_NOEXCEPT;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		auto iRBegin = str.rbegin();

		VERIFY(*iRBegin++ == LITERAL('z'));
		VERIFY(*iRBegin++ == LITERAL('y'));
		VERIFY(*iRBegin++ == LITERAL('x'));
		VERIFY(*iRBegin++ == LITERAL('w'));
		VERIFY(*iRBegin++ == LITERAL('v'));
		VERIFY(*iRBegin++ == LITERAL('u'));
		VERIFY(*(str.rbegin() + 25) == LITERAL('a'));
	}

	// reverse_iterator       rend() EA_NOEXCEPT;
	// const_reverse_iterator rend() const EA_NOEXCEPT;
	// const_reverse_iterator crend() const EA_NOEXCEPT;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		auto iREnd = str.rend() - 1;

		VERIFY(*iREnd-- == LITERAL('a'));
		VERIFY(*iREnd-- == LITERAL('b'));
		VERIFY(*iREnd-- == LITERAL('c'));
		VERIFY(*iREnd-- == LITERAL('d'));
		VERIFY(*iREnd-- == LITERAL('e'));
		VERIFY(*iREnd-- == LITERAL('f'));
		VERIFY(*(str.rend() - 26) == LITERAL('z'));
	}

	// bool empty() const EA_NOEXCEPT;
	// size_type size() const EA_NOEXCEPT;
	// size_type length() const EA_NOEXCEPT;
	// size_type capacity() const EA_NOEXCEPT;
	// void resize(size_type n, value_type c);
	// void resize(size_type n);
	// void set_capacity(size_type n = npos);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(!str.empty());
		VERIFY(str.size() == 26);
		VERIFY(str.length() == 26);
		VERIFY(str.capacity() >= 26);

		str.assign(LITERAL(""));
		VERIFY(str.empty());
		VERIFY(str.size() == 0);
		VERIFY(str.length() == 0);
		VERIFY(str.capacity() >= 26);  // should not free existing capacity

		str.resize(0);
		VERIFY(str.empty());
		VERIFY(str.size() == 0);
		VERIFY(str.length() == 0);
		VERIFY(str.capacity() >= 26);  // should not free existing capacity

		str.set_capacity(0);
		// VERIFY(str.capacity() == 0); // frees existing capacity, but has a minimun of SSO capacity

		str.resize(32, LITERAL('c'));
		VERIFY(!str.empty());
		VERIFY(str.size() == 32);
		VERIFY(str.length() == 32);
		VERIFY(str.capacity() >= 32);
		VERIFY(str == LITERAL("cccccccccccccccccccccccccccccccc"));
	}

	// void shrink_to_fit
	{
		SSOStringType str(LITERAL("a"));
		str.reserve(100);
		VERIFY(str.capacity() == 100);
		str.shrink_to_fit();
		// string should shrink to SSO
		VERIFY(str.IsSSO());

		str = LITERAL("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); // 32 characters
		str.reserve(100);
		VERIFY(str.capacity() == 100);
		str.shrink_to_fit();
		// string should shrink but still be heap
		VERIFY(str.capacity() == 32);
		VERIFY(!str.IsSSO());
	}

	// void set_capacity(n)
	{
		const auto *pLiteral32 = LITERAL("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		const auto *pLiteral31 = LITERAL("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		const auto *pLiteral1 = LITERAL("a");
		const auto *pLiteral2 = LITERAL("aa");

		SSOStringType str = pLiteral32;
		// set_capacity(0) - deallocate and reset to SSO;
		{
			// heap -> sso
			VERIFY(!str.IsSSO());
			str.set_capacity(0);
			VERIFY(str.IsSSO());
			VERIFY(str == LITERAL(""));
		}
		{
			// sso -> sso
			str = pLiteral1;
			VERIFY(str.IsSSO());
			str.set_capacity(0);
			VERIFY(str.IsSSO());
			VERIFY(str == LITERAL(""));
		}

		// set_capacity(npos) - set capacity equal to current size - should realloc
		{
			// heap -> heap
			str = pLiteral32;
			str.reserve(100);
			VERIFY(!str.IsSSO());
			VERIFY(str.capacity() == 100);
			str.set_capacity(StringType::npos);
			VERIFY(!str.IsSSO());
			VERIFY(str.capacity() == 32);
			VERIFY(str == pLiteral32);
		}
		{
			// heap -> sso
			str = pLiteral1;
			str.reserve(100);
			VERIFY(!str.IsSSO());
			VERIFY(str.capacity() == 100);
			str.set_capacity(StringType::npos);
			VERIFY(str.IsSSO());
			VERIFY(str == pLiteral1);
		}
		{
			// sso -> sso
			str = pLiteral1;
			VERIFY(str.IsSSO());
			str.set_capacity(StringType::npos);
			VERIFY(str.IsSSO());
			VERIFY(str == pLiteral1);
		}

		// set_capacity(n > capacity) - set capacity greater than out current capacity
		{
			// heap -> heap
			str = pLiteral32;
			VERIFY(!str.IsSSO());
			auto nSavedCap = str.capacity();
			str.set_capacity(nSavedCap + 1);
			VERIFY(!str.IsSSO());
			VERIFY(str == pLiteral32);
			VERIFY(str.capacity() > nSavedCap);
		}
		{
			// sso -> heap
			str.set_capacity(0); // reset to sso
			str = pLiteral1;
			VERIFY(str.IsSSO());
			auto nSavedCap = str.capacity();
			str.set_capacity(nSavedCap + 1);
			VERIFY(!str.IsSSO());
			VERIFY(str == pLiteral1);
			VERIFY(str.capacity() > nSavedCap);
		}
		{
			// sso -> sso
			str.set_capacity(0); // reset to sso
			str = pLiteral1;
			VERIFY(str.IsSSO());
			auto nSavedCap = str.capacity();
			str.set_capacity(str.size() + 1);
			VERIFY(str.IsSSO());
			VERIFY(str == pLiteral1);
			VERIFY(str.capacity() == nSavedCap);
		}

		// set_capacity(n < size) - set capacity less than current size, str should truncate
		{
			// sso -> sso
			str = pLiteral2;
			VERIFY(str.IsSSO());
			str.set_capacity(1);
			VERIFY(str.IsSSO());
			VERIFY(str == pLiteral1);
		}
		{
			// heap -> sso
			str = pLiteral32;
			VERIFY(!str.IsSSO());
			str.set_capacity(1);
			VERIFY(str.IsSSO());
			VERIFY(str == pLiteral1);
		}
		{
			// heap -> heap
			str = pLiteral32;
			VERIFY(!str.IsSSO());
			str.set_capacity(31);
			VERIFY(!str.IsSSO());
			VERIFY(str == pLiteral31);
		}
	}

	// void reserve(size_type = 0);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(!str.empty());
		VERIFY(str.size() == 26);
		VERIFY(str.length() == 26);
		VERIFY(str.capacity() >= 26);

		// verifies that we allocate memory
		str.reserve(64);
		VERIFY(!str.empty());
		VERIFY(str.size() == 26);
		VERIFY(str.length() == 26);
		VERIFY(str.capacity() >= 64);

		// verifies that we do not free memory
		str.reserve(32);
		VERIFY(!str.empty());
		VERIFY(str.size() == 26);
		VERIFY(str.length() == 26);
		VERIFY(str.capacity() >= 64);
	}

	// void force_size(size_type n);
	{
		// force_size does not write terminating null, meant to set size when using external
		// string writing mnethods like strcpy or sprintf
		StringType str(LITERAL("aaa"));
		VERIFY(str.size() == 3);
		str.force_size(0);
		VERIFY(str.size() == 0);
		str.reserve(4); // 32 bit platform with char32_t can only hold 2 characters
		str.force_size(4);
		VERIFY(str.size() == 4);
		str[4] = '0';
		str = LITERAL("aaa");
		VERIFY(str.size() == 3);
	}

	// const value_type* data() const EA_NOEXCEPT;
	// const value_type* c_str() const EA_NOEXCEPT;
	{
		const StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		const typename StringType::value_type* pData = str.data();
		const typename StringType::value_type* pCStr = str.c_str();

		VERIFY(pData != nullptr);
		VERIFY(pCStr != nullptr);
		VERIFY(pData == pCStr);
		VERIFY(EA::StdC::Memcmp(pData, pCStr, str.size()) == 0);
	}

	// value_type* data() EA_NOEXCEPT;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		typename StringType::value_type* pData = str.data();

		VERIFY(pData != nullptr);
		VERIFY(EA::StdC::Memcmp(pData, LITERAL("abcdefghijklmnopqrstuvwxyz"), str.size()) == 0);
	}

	// reference       operator[](size_type n);
	// const_reference operator[](size_type n) const;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str[0]  == LITERAL('a'));
		VERIFY(str[14] == LITERAL('o'));
		VERIFY(str[25] == LITERAL('z'));
	}

	// reference       at(size_type n);
	// const_reference at(size_type n) const;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str.at(0)  == LITERAL('a'));
		VERIFY(str.at(14) == LITERAL('o'));
		VERIFY(str.at(25) == LITERAL('z'));
	}

	// reference       front();
	// const_reference front() const;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(str.front()  == LITERAL('a'));
	}

	// reference       back();
	// const_reference back() const;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		VERIFY(str.back()  == LITERAL('z'));
	}

	// this_type& operator+=(const this_type& x);
	// this_type& operator+=(const value_type* p);
	// this_type& operator+=(value_type c);
	{
		StringType str1(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		StringType str2(LITERAL("123"));
		str1 += str2;
		str1 += LITERAL("456");
		str1 += LITERAL('7');

		VERIFY(str1 == LITERAL("abcdefghijklmnopqrstuvwxyz1234567"));
	}

	// this_type& append(const this_type& x);
	// this_type& append(const this_type& x, size_type position, size_type n);
	// this_type& append(const value_type* p, size_type n);
	// this_type& append(const value_type* p);
	// this_type& append(size_type n, value_type c);
	// this_type& append(const value_type* pBegin, const value_type* pEnd);
	{
		const StringType src(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		StringType str;
		str.append(StringType(LITERAL("abcd")));        // "abcd"
		str.append(src, 4, 4); 		   					// "abcdefgh"
		str.append(src.data() + 8, 4); 					// "abcdefghijkl"
		str.append(LITERAL("mnop"));   					// "abcdefghijklmnop"
		str.append(1, LITERAL('q'));   					// "abcdefghijklmnopq"
		str.append(src.data() + 17, src.data() + 26);   // "abcdefghijklmnopqrstuvwxyz"

		VERIFY(str == src);
	}

	// this_type& append_sprintf_va_list(const value_type* pFormat, va_list arguments);
	// this_type& append_sprintf(const value_type* pFormat, ...);
	{
	#if EASTL_SNPRINTF_TESTS_ENABLED
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		str.append_sprintf(LITERAL("Hello, %d"), 42);

		VERIFY(str == LITERAL("abcdefghijklmnopqrstuvwxyzHello, 42"));
		VERIFY(str.validate());
	#endif
	}

	// template <typename OtherCharType>
	// this_type& append_convert(const OtherCharType* p);
	{
		{
		#if defined(EA_CHAR8)
			StringType str;
			str.append_convert(EA_CHAR8("123456789"));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR16)
			StringType str;
			str.append_convert(EA_CHAR16("123456789"));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR32)
			StringType str;
			str.append_convert(EA_CHAR32("123456789"));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_WCHAR)
		    StringType str;
		    str.append_convert(EA_WCHAR("123456789"));
		    VERIFY(str == LITERAL("123456789"));
		    VERIFY(str.validate());
		#endif
		}
	}

	// template <typename OtherCharType>
	// this_type& append_convert(const OtherCharType* p, size_type n);
	{
		{
		#if defined(EA_CHAR8)
			StringType str;
			str.append_convert(EA_CHAR8("123456789"), 5);
			VERIFY(str == LITERAL("12345"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR16)
			StringType str;
			str.append_convert(EA_CHAR16("123456789"), 5);
			VERIFY(str == LITERAL("12345"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR32)
			StringType str;
			str.append_convert(EA_CHAR32("123456789"), 5);
			VERIFY(str == LITERAL("12345"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_WCHAR)
		    StringType str;
		    str.append_convert(EA_WCHAR("123456789"), 5);
		    VERIFY(str == LITERAL("12345"));
		    VERIFY(str.validate());
		#endif
		}
	}

	// template <typename OtherStringType>
	// this_type& append_convert(const OtherStringType& x);
	{
		{
		#if defined(EA_CHAR8)
			StringType str; 
			str.append_convert(eastl::u8string(EA_CHAR8("123456789")));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR16)
			StringType str; 
			str.append_convert(eastl::string16(EA_CHAR16("123456789")));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_CHAR32)
			StringType str;
			str.append_convert(eastl::string32(EA_CHAR32("123456789")));
			VERIFY(str == LITERAL("123456789"));
			VERIFY(str.validate());
		#endif
		}
		{
		#if defined(EA_WCHAR)
		    StringType str;
		    str.append_convert(eastl::wstring(EA_WCHAR("123456789")));
		    VERIFY(str == LITERAL("123456789"));
		    VERIFY(str.validate());
		#endif
		}
	}

	// void push_back(value_type c);
	{
		StringType str;
		const StringType src(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		eastl::for_each(eastl::begin(src), eastl::end(src), [&str](const typename StringType::value_type& c)
		                { str.push_back(c); });

		VERIFY(str == src);
		VERIFY(str.validate());
	}

	// void pop_back();
	{
		StringType str(LITERAL("123456789"));
		VERIFY(str == LITERAL("123456789"));

		str.pop_back(); VERIFY(str == LITERAL("12345678"));
		str.pop_back(); VERIFY(str == LITERAL("1234567"));
		str.pop_back(); VERIFY(str == LITERAL("123456"));
		str.pop_back(); VERIFY(str == LITERAL("12345"));
		str.pop_back(); VERIFY(str == LITERAL("1234"));
		str.pop_back(); VERIFY(str == LITERAL("123"));
		str.pop_back(); VERIFY(str == LITERAL("12"));
		str.pop_back(); VERIFY(str == LITERAL("1"));
		str.pop_back(); VERIFY(str == LITERAL(""));

		VERIFY(str.validate());
	}

	// this_type& insert(size_type position, const this_type& x);
	// this_type& insert(size_type position, const this_type& x, size_type beg, size_type n);
	// this_type& insert(size_type position, const value_type* p, size_type n);
	// this_type& insert(size_type position, const value_type* p);
	// this_type& insert(size_type position, size_type n, value_type c);
	// iterator insert(const_iterator p, value_type c);
	// iterator insert(const_iterator p, size_type n, value_type c);
	// iterator insert(const_iterator p, const value_type* pBegin, const value_type* pEnd);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		str.insert((typename StringType::size_type)0, (typename StringType::size_type)1, LITERAL('1'));   // todo: elminiate the cast to disambiguate
		VERIFY(str == LITERAL("1abcdefghijklmnopqrstuvwxyz"));

		str.insert(2, LITERAL("234"));
		VERIFY(str == LITERAL("1a234bcdefghijklmnopqrstuvwxyz"));

		str.insert(15, StringType(LITERAL("567")));
		VERIFY(str == LITERAL("1a234bcdefghijk567lmnopqrstuvwxyz"));

		str.insert(30, StringType(LITERAL(" is an example of a substring")), 1, 14);
		VERIFY(str == LITERAL("1a234bcdefghijk567lmnopqrstuvwis an example xyz"));

		{
			StringType strSSO;
			auto nSSOCap = strSSO.capacity();
			StringType strCheck;
			strCheck.append(nSSOCap, LITERAL('a'));

			strSSO.append(nSSOCap - 1, LITERAL('a'));

			strSSO.insert(strSSO.size() - 1, LITERAL("a"));
			VERIFY(strSSO.validate());
			VERIFY(strSSO == strCheck);
		}

		{
			StringType strSSO;
			auto nSSOCap = strSSO.capacity();

			// 32 bit platform with char32_t can only hold 2 characters in SSO
			if (nSSOCap - 2 > 0)
			{
				StringType strCheck;
				strCheck.append(nSSOCap, LITERAL('a'));

				strSSO.append(nSSOCap - 2, LITERAL('a'));

				strSSO.insert(strSSO.size() - 1, LITERAL("aa"));
				VERIFY(strSSO.validate());
				VERIFY(strSSO == strCheck);
			}
		}
	}

	// iterator insert(const_iterator p, std::initializer_list<value_type>);
	{
	#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		StringType str;
		str.insert(str.begin(),     {'a','b','c'});
		str.insert(str.end(),       {'d','e','f'});
		str.insert(str.begin() + 3, {'1','2','3'});

		VERIFY(str == LITERAL("abc123def"));
		VERIFY(str.validate());
	#endif
	}

	// insert(const_iterator p, value_type c)
	{
		StringType str = LITERAL("aaa");
		auto it = str.insert(str.end(), 'b');
		VERIFY(*it == LITERAL('b'));
		VERIFY(str == LITERAL("aaab"));
		it = str.insert(str.begin(), 'c');
		VERIFY(*it == LITERAL('c'));
		VERIFY(str == LITERAL("caaab"));
		it = str.insert(str.begin() + 2, 'd');
		VERIFY(*it == LITERAL('d'));
		VERIFY(str == LITERAL("cadaab"));
	}

	// this_type&       erase(size_type position = 0, size_type n = npos);
	// iterator         erase(const_iterator p);
	// iterator         erase(const_iterator pBegin, const_iterator pEnd);
	// reverse_iterator erase(reverse_iterator position);
	// reverse_iterator erase(reverse_iterator first, reverse_iterator last);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		str.erase(0,5);
		VERIFY(str == LITERAL("fghijklmnopqrstuvwxyz"));

		str.erase(5,10);
		VERIFY(str == LITERAL("fghijuvwxyz"));

		str.erase(str.find(LITERAL('v')));
		VERIFY(str == LITERAL("fghiju"));

		str.erase(str.find(LITERAL('g')), str.find(LITERAL('i')));
		VERIFY(str == LITERAL("fju"));

		typename StringType::const_iterator it = str.begin() + 1; // 'j'
		str.erase(it);
		VERIFY(str == LITERAL("fu"));

	}

	// void clear() EA_NOEXCEPT;
	{
		StringType str(LITERAL("123456789"));
		VERIFY(str == LITERAL("123456789"));

		str.clear();
		VERIFY(str == LITERAL(""));
		VERIFY(str.empty());
		VERIFY(str.validate());
	}


	// pointer detach() EA_NOEXCEPT;
	{
		{
			// Heap
			auto* pLiteral = LITERAL("abcdefghijklmnopqrstuvwxyz");
			StringType str(pLiteral);
			const auto sz = str.size() + 1;  // +1 for null-terminator

			auto* pDetach = str.detach();

			VERIFY(pDetach != nullptr);
			VERIFY(EA::StdC::Strcmp(pDetach, pLiteral) == 0);
			VERIFY(pDetach != pLiteral);
			VERIFY(str.empty());
			VERIFY(str.size() == 0);

			str.get_allocator().deallocate(pDetach, sz);
		}

		{
			// SSO
			auto* pLiteral = LITERAL("a");
			StringType str(pLiteral);
			const auto sz = str.size() + 1;  // +1 for null-terminator

			auto* pDetach = str.detach();

			VERIFY(pDetach != nullptr);
			VERIFY(EA::StdC::Strcmp(pDetach, pLiteral) == 0);
			VERIFY(pDetach != pLiteral);
			VERIFY(str.empty());
			VERIFY(str.size() == 0);

			str.get_allocator().deallocate(pDetach, sz);
		}

		{
			// SSO, empty string
			auto* pLiteral = LITERAL("");
			StringType str(pLiteral);
			const auto sz = str.size() + 1;  // +1 for null-terminator

			auto* pDetach = str.detach();

			VERIFY(pDetach != nullptr);
			VERIFY(EA::StdC::Strcmp(pDetach, pLiteral) == 0);
			VERIFY(pDetach != pLiteral);
			VERIFY(str.empty());
			VERIFY(str.size() == 0);

			str.get_allocator().deallocate(pDetach, sz);
		}

		{
			// SSO, empty string via default ctor
			StringType str;
			const auto sz = str.size() + 1;  // +1 for null-terminator

			auto* pDetach = str.detach();

			VERIFY(pDetach != nullptr);
			VERIFY(pDetach[0] == 0);
			VERIFY(str.empty());
			VERIFY(str.size() == 0);

			str.get_allocator().deallocate(pDetach, sz);
		}
	}

	// this_type&  replace(size_type position, size_type n, const this_type& x);
	// this_type&  replace(size_type pos1, size_type n1, const this_type& x, size_type pos2, size_type n2);
	// this_type&  replace(size_type position, size_type n1, const value_type* p, size_type n2);
	// this_type&  replace(size_type position, size_type n1, const value_type* p);
	// this_type&  replace(size_type position, size_type n1, size_type n2, value_type c);
	// this_type&  replace(const_iterator first, const_iterator last, const this_type& x);
	// this_type&  replace(const_iterator first, const_iterator last, const value_type* p, size_type n);
	// this_type&  replace(const_iterator first, const_iterator last, const value_type* p);
	// this_type&  replace(const_iterator first, const_iterator last, size_type n, value_type c);
	// this_type&  replace(const_iterator first, const_iterator last, const value_type* pBegin, const value_type* pEnd);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		str.replace(5, 10, StringType(LITERAL("123")));
		VERIFY(str == LITERAL("abcde123pqrstuvwxyz"));

		str.replace(13, 1, StringType(LITERAL("0123456789")), 4, 6 );
		VERIFY(str == LITERAL("abcde123pqrst456789vwxyz"));

		str.replace(24, 1, LITERAL("0123456789"));
		VERIFY(str == LITERAL("abcde123pqrst456789vwxyz0123456789"));

		str.replace(16, 4, 4, LITERAL('@'));
		VERIFY(str == LITERAL("abcde123pqrst456@@@@wxyz0123456789"));
	}

	// size_type copy(value_type* p, size_type n, size_type position = 0) const;
	{
		typename StringType::value_type buf[64];

		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		str.copy(buf, 10, 10);

		VERIFY(EA::StdC::Memcmp(buf, LITERAL("klmnopqrst"), 10) == 0);
	}

	// size_type find(const this_type& x, size_type position = 0) const EA_NOEXCEPT;
	// size_type find(const value_type* p, size_type position = 0) const;
	// size_type find(const value_type* p, size_type position, size_type n) const;
	// size_type find(value_type c, size_type position = 0) const EA_NOEXCEPT;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str.find(StringType(LITERAL("d")))    != StringType::npos);
		VERIFY(str.find(StringType(LITERAL("tuv")))  != StringType::npos);
		VERIFY(str.find(StringType(LITERAL("123r"))) == StringType::npos);

		VERIFY(str.find(LITERAL("d"))    != StringType::npos);
		VERIFY(str.find(LITERAL("tuv"))  != StringType::npos);
		VERIFY(str.find(LITERAL("123r")) == StringType::npos);

		VERIFY(str.find(LITERAL("d"), 0)    != StringType::npos);
		VERIFY(str.find(LITERAL("tuv"), 2)  != StringType::npos);
		VERIFY(str.find(LITERAL("123r"), 2) == StringType::npos);

		VERIFY(str.find(LITERAL('d'), 0) != StringType::npos);
		VERIFY(str.find(LITERAL('t'), 2) != StringType::npos);
		VERIFY(str.find(LITERAL('1'), 2) == StringType::npos);
	}

	// size_type rfind(const this_type& x, size_type position = npos) const EA_NOEXCEPT;
	// size_type rfind(const value_type* p, size_type position = npos) const;
	// size_type rfind(const value_type* p, size_type position, size_type n) const;
	// size_type rfind(value_type c, size_type position = npos) const EA_NOEXCEPT;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str.rfind(StringType(LITERAL("d")))    != StringType::npos);
		VERIFY(str.rfind(StringType(LITERAL("tuv")))  != StringType::npos);
		VERIFY(str.rfind(StringType(LITERAL("123r"))) == StringType::npos);

		VERIFY(str.rfind(LITERAL("d"))    != StringType::npos);
		VERIFY(str.rfind(LITERAL("tuv"))  != StringType::npos);
		VERIFY(str.rfind(LITERAL("123r")) == StringType::npos);

		VERIFY(str.rfind(LITERAL("d"), 20)    != StringType::npos);
		VERIFY(str.rfind(LITERAL("tuv"), 20)  != StringType::npos);
		VERIFY(str.rfind(LITERAL("123r"), 20) == StringType::npos);

		VERIFY(str.rfind(LITERAL('d'), 20) != StringType::npos);
		VERIFY(str.rfind(LITERAL('t'), 20) != StringType::npos);
		VERIFY(str.rfind(LITERAL('1'), 20) == StringType::npos);
	}

	// size_type find_first_of(const this_type& x, size_type position = 0) const EA_NOEXCEPT;
	// size_type find_first_of(const value_type* p, size_type position = 0) const;
	// size_type find_first_of(const value_type* p, size_type position, size_type n) const;
	// size_type find_first_of(value_type c, size_type position = 0) const EA_NOEXCEPT;
	{
		StringType str(LITERAL("aaaaabbbbbcccdddddeeeeefffggh"));

		VERIFY(str.find_first_of(StringType(LITERAL("aaa"))) == 0);
		VERIFY(str.find_first_of(LITERAL("aab")) == 0);
		VERIFY(str.find_first_of(LITERAL("baab")) == 0);
		VERIFY(str.find_first_of(LITERAL("ceg")) == 10);
		VERIFY(str.find_first_of(LITERAL("eeef"), 1, 2) == 18);
		VERIFY(str.find_first_of(LITERAL("eeef"), 1, 4) == 18);
		VERIFY(str.find_first_of(LITERAL('g')) == 26);
		VERIFY(str.find_first_of(LITERAL('$')) == StringType::npos);
	}

	// size_type find_last_of(const this_type& x, size_type position = npos) const EA_NOEXCEPT;
	// size_type find_last_of(const value_type* p, size_type position = npos) const;
	// size_type find_last_of(const value_type* p, size_type position, size_type n) const;
	// size_type find_last_of(value_type c, size_type position = npos) const EA_NOEXCEPT;
	{
		StringType str(LITERAL("aaaaabbbbbcccdddddeeeeefffggh"));

		VERIFY(str.find_last_of(StringType(LITERAL("aaa"))) == 4);
		VERIFY(str.find_last_of(LITERAL("aab")) == 9);
		VERIFY(str.find_last_of(LITERAL("baab")) == 9);
		VERIFY(str.find_last_of(LITERAL("ceg")) == 27);
		// VERIFY(str.find_last_of(LITERAL("eeef"), 1, 2) == StringType::npos);  // todo: FIX ME
		// VERIFY(str.find_last_of(LITERAL("eeef"), 1, 4) == StringType::npos);  // todo: FIX ME
		VERIFY(str.find_last_of(LITERAL('g')) == 27);
		VERIFY(str.find_last_of(LITERAL('$')) == StringType::npos);
	}

	// size_type find_first_not_of(const this_type& x, size_type position = 0) const EA_NOEXCEPT;
	// size_type find_first_not_of(const value_type* p, size_type position = 0) const;
	// size_type find_first_not_of(const value_type* p, size_type position, size_type n) const;
	// size_type find_first_not_of(value_type c, size_type position = 0) const EA_NOEXCEPT;
	{
		StringType str(LITERAL("aaaaabbbbbcccdddddeeeeefffggh"));

		VERIFY(str.find_first_not_of(StringType(LITERAL("abcdfg"))) == 18);
		VERIFY(str.find_first_not_of(LITERAL("abcdfg")) == 18);
		// VERIFY(str.find_first_not_of(LITERAL("abcdfg"), 2, 2) == 0);   // todo:  FIX ME
		// VERIFY(str.find_first_not_of(LITERAL("abcdfg"), 0, 2) == 10);  // todo:  FIX ME
		VERIFY(str.find_first_not_of(LITERAL('a')) == 5);
	}

	// size_type find_last_not_of(const this_type& x,  size_type position = npos) const EA_NOEXCEPT;
	// size_type find_last_not_of(const value_type* p, size_type position = npos) const;
	// size_type find_last_not_of(const value_type* p, size_type position, size_type n) const;
	// size_type find_last_not_of(value_type c, size_type position = npos) const EA_NOEXCEPT;
	{
		StringType str(LITERAL("aaaaabbbbbcccdddddeeeeefffggh"));

		VERIFY(str.find_last_not_of(StringType(LITERAL("a"))) == 28);
		VERIFY(str.find_last_not_of(StringType(LITERAL("abcdfg"))) == 28);
		VERIFY(str.find_last_not_of(StringType(LITERAL("abcdfgh"))) == 22);
		VERIFY(str.find_last_not_of(LITERAL("abcdfgh")) == 22);
		// VERIFY(str.find_last_not_of(LITERAL("abcdfg"), 2, 2) == 0);   // todo:  FIX ME
		// VERIFY(str.find_last_not_of(LITERAL("abcdfg"), 0, 2) == 10);  // todo:  FIX ME
		VERIFY(str.find_last_not_of(LITERAL('a')) == 28);
	}

	// this_type substr(size_type position = 0, size_type n = npos) const;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		auto substring = str.substr(0, 6);
		VERIFY(substring == LITERAL("abcdef"));

		substring = str.substr(0, 0);
		VERIFY(substring == LITERAL(""));

		substring = str.substr(16, 0);
		VERIFY(substring == LITERAL(""));

		substring = str.substr(16, 42);
		VERIFY(substring == LITERAL("qrstuvwxyz"));
	}

	// int        compare(const this_type& x) const EA_NOEXCEPT;
	// int        compare(size_type pos1, size_type n1, const this_type& x) const;
	// int        compare(size_type pos1, size_type n1, const this_type& x, size_type pos2, size_type n2) const;
	// int        compare(const value_type* p) const;
	// int        compare(size_type pos1, size_type n1, const value_type* p) const;
	// int        compare(size_type pos1, size_type n1, const value_type* p, size_type n2) const;
	// static int compare(const value_type* pBegin1, const value_type* pEnd1, const value_type* pBegin2, const value_type* pEnd2);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str.compare(StringType(LITERAL("abcdefghijklmnopqrstuvwxyz"))) == 0);
		VERIFY(str.compare(StringType(LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ"))) != 0);
		VERIFY(str.compare(StringType(LITERAL("abcdefghijklmnopqrstuvwxyz123"))) != 0);
		VERIFY(str.compare(LITERAL("abcdefghijklmnopqrstuvwxyz")) == 0);
		VERIFY(str.compare(LITERAL("abcdefghijklmnopqrstuvwxyz123")) != 0);
		VERIFY(str.compare(LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ123")) != 0);
	}

	// int        comparei(const this_type& x) const EA_NOEXCEPT;
	// int        comparei(const value_type* p) const;
	// static int comparei(const value_type* pBegin1, const value_type* pEnd1, const value_type* pBegin2, const value_type* pEnd2);
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		VERIFY(str.comparei(StringType(LITERAL("abcdefghijklmnopqrstuvwxyz"))) == 0);
		VERIFY(str.comparei(StringType(LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ"))) == 0);
		VERIFY(str.comparei(StringType(LITERAL("abcdefghijklmnopqrstuvwxyz123"))) != 0);
		VERIFY(str.comparei(LITERAL("abcdefghijklmnopqrstuvwxyz")) == 0);
		VERIFY(str.comparei(LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ")) == 0);
		VERIFY(str.comparei(LITERAL("abcdefghijklmnopqrstuvwxyz123")) != 0);
	}

	// void make_lower();
	{
		{
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			str.make_lower();
			VERIFY(str == LITERAL("abcdefghijklmnopqrstuvwxyz"));
		}
		{
			StringType str(LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
			str.make_lower();
			VERIFY(str == LITERAL("abcdefghijklmnopqrstuvwxyz"));
		}
		{
			StringType str(LITERAL("123456789~!@#$%^&*()_+"));
			str.make_lower();
			VERIFY(str == LITERAL("123456789~!@#$%^&*()_+"));
		}
	}

	// void make_upper();
	{
		{
			StringType str(LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
			str.make_upper();
			VERIFY(str == LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
		}
		{
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			str.make_upper();
			VERIFY(str == LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
		}
		{
			StringType str(LITERAL("123456789~!@#$%^&*()_+"));
			str.make_upper();
			VERIFY(str == LITERAL("123456789~!@#$%^&*()_+"));
		}
	}

	// void ltrim();
	// void rtrim();
	// void trim();
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		{
			StringType rstr(LITERAL("abcdefghijklmnopqrstuvwxyz			\t		\t\t\t		"));
			rstr.ltrim();
			VERIFY(str != rstr);
		}
		{
			StringType lstr(LITERAL("	\t						abcdefghijklmnopqrstuvwxyz"));
			lstr.ltrim();
			VERIFY(str == lstr);
		}
		{
			StringType rstr(LITERAL("abcdefghijklmnopqrstuvwxyz		\t\t\t					"));
			rstr.rtrim();
			VERIFY(str == rstr);
		}
		{
			StringType lstr(LITERAL("	\t						abcdefghijklmnopqrstuvwxyz"));
			lstr.rtrim();
			VERIFY(str != lstr);
		}
		{
			StringType lrstr(LITERAL("   \t                abcdefghijklmnopqrstuvwxyz		\t					"));
			lrstr.trim();
			VERIFY(str == lrstr);
		}
		{
			auto* pLiteral = LITERAL("abcdefghijklmn          opqrstuvwxyz");
			StringType mstr(pLiteral);
			mstr.trim();
			VERIFY(mstr == pLiteral);
		}
	}

	// void ltrim("a");
	// void rtrim("b");
	// void trim("?");
	{
		StringType expected(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		{
			const auto source = LITERAL("abcdefghijklmnopqrstuvwxyz                                         ");

			StringType rstr(source);
			rstr.ltrim(LITERAL(" "));
			VERIFY(rstr == source);  

			rstr.rtrim(LITERAL(" "));
			VERIFY(expected == rstr);
		}

		{
			const auto source = LITERAL("abcdefghijklmnopqrstuvwxyz			\t		\t\t\t		");

			StringType rstr(source);
			rstr.ltrim(LITERAL(" \t"));
			VERIFY(rstr == source);

			rstr.rtrim(LITERAL(" \t"));
			VERIFY(expected == rstr);
		}

		{
			const auto source = LITERAL(" \t		\t\t\t		abcdefghijklmnopqrstuvwxyz");

			StringType rstr(source);
			rstr.rtrim(LITERAL(" \t"));
			VERIFY(rstr == source);

			rstr.ltrim(LITERAL(" \t"));
			VERIFY(expected == rstr);
		}

		{
			const auto source = LITERAL("$$$%$$$$$$%$$$$$$$$$%$$$$$$$$abcdefghijklmnopqrstuvwxyz*********@*****************@******");
			StringType rstr(source);
			rstr.trim(LITERAL("^("));
			VERIFY(rstr == source);
		}

		{
			const auto source = LITERAL("$$$%$$$$$$%$$$$$$$$$%$$$$$$$$abcdefghijklmnopqrstuvwxyz*********@*****************@******");
			StringType rstr(source);
			rstr.rtrim(LITERAL("@*"));

			VERIFY(expected != rstr);
			VERIFY(rstr == LITERAL("$$$%$$$$$$%$$$$$$$$$%$$$$$$$$abcdefghijklmnopqrstuvwxyz"));

			rstr.ltrim(LITERAL("$%"));
			VERIFY(expected == rstr);
		}

		{
			const auto source = LITERAL("abcdefghijklmnopqrstuvwxyz**********************************");
			StringType rstr(source);
			rstr.ltrim(LITERAL("*"));
			VERIFY(expected != source);
		}

		{
			const auto source = LITERAL("           ?      abcdefghijklmnopqrstuvwxyz**********************************");
			StringType rstr(source);
			rstr.trim(LITERAL("*? "));
			VERIFY(expected != source);
		}
	}

	// this_type left(size_type n) const;
	// this_type right(size_type n) const;
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));

		auto lstr = str.left(6);
		VERIFY(lstr == LITERAL("abcdef"));

		auto rstr = str.right(8);
		VERIFY(rstr == LITERAL("stuvwxyz"));
	}

	// this_type& sprintf_va_list(const value_type* pFormat, va_list arguments);
	// this_type& sprintf(const value_type* pFormat, ...);
	{
	#if EASTL_SNPRINTF_TESTS_ENABLED
		StringType str(LITERAL(""));

		str.sprintf(LITERAL("Hello, %d"), 42);
		VERIFY(str == LITERAL("Hello, 42"));
	#endif
	}

	// void force_size(size_type n);
	{
		StringType str(LITERAL(""));
		str.reserve(10);

		auto p = const_cast<typename StringType::value_type*>(str.data());
		p[0] = 'a';
		p[1] = 'a';
		p[2] = 'a';
		p[3] = '\0';

		str.force_size(3);

		VERIFY(str.size() == 3);
		VERIFY(str.validate());
		VERIFY(!str.empty());
	}

	// test basic_string implicit conversion to basic_string_view
	// 		eastl::string implicitly converts to eastl::string_view.
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		[&](basic_string_view<typename StringType::value_type> sv)  // simulate api that requires eastl::string_view.
		{
			VERIFY(sv.compare(LITERAL("abcdefghijklmnopqrstuvwxyz")) == 0);
		}(str);
	}

	// test constructing a eastl::basic_string from an eastl::basic_string_view
	{
		using StringViewType = basic_string_view<typename StringType::value_type>;
		StringViewType sv = LITERAL("abcdefghijklmnopqrstuvwxyz");

		{
			StringType str(sv);
			VERIFY(str == LITERAL("abcdefghijklmnopqrstuvwxyz"));
		}

		{
			StringType str(sv, typename StringType::allocator_type("test"));
			VERIFY(str == LITERAL("abcdefghijklmnopqrstuvwxyz"));
		}

		{
			StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
			VERIFY(sv == str);
		}
    }

	// test assigning from an eastl::basic_string_view
	{
		using StringViewType = basic_string_view<typename StringType::value_type>;
		StringViewType sv = LITERAL("abcdefghijklmnopqrstuvwxyz");

		{
			StringType str;
			str = sv;  // force call to 'operator='
			VERIFY(str == LITERAL("abcdefghijklmnopqrstuvwxyz"));
		}
	}

	// test eastl::erase
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		auto numErased = eastl::erase(str, LITERAL('a'));
	    VERIFY(numErased == 1);
		numErased = eastl::erase(str, LITERAL('f'));
	    VERIFY(numErased == 1);
		numErased = eastl::erase(str, LITERAL('l'));
	    VERIFY(numErased == 1);
		numErased = eastl::erase(str, LITERAL('w'));
	    VERIFY(numErased == 1);
		numErased = eastl::erase(str, LITERAL('y'));
	    VERIFY(numErased == 1);
		VERIFY(str == LITERAL("bcdeghijkmnopqrstuvxz"));
	}

	// test eastl::erase_if
	{
		StringType str(LITERAL("abcdefghijklmnopqrstuvwxyz"));
		auto numErased = eastl::erase_if(str, [](auto c) { return c == LITERAL('a') || c == LITERAL('v'); });
		VERIFY(str == LITERAL("bcdefghijklmnopqrstuwxyz"));
	    VERIFY(numErased == 2);
	}

	// template<> struct hash<eastl::string>;
	// template<> struct hash<eastl::wstring>;
	// template<> struct hash<eastl::u16string>;
	// template<> struct hash<eastl::u32string>;
	{
		// NOTE(rparolin): This is required because the string tests inject custom allocators to assist in debugging.
		// These custom string types require their own hashing specializations which we emulate by constructing a custom
		// hashing functor that defers to the eastl::basic_string<CharT> hash implementation; effectively ignoring the
		// custom allocator.
	    auto LocalHash = [](auto s) -> size_t {
		    using UserStringType = decltype(s);
		    using TargetType = eastl::basic_string<typename UserStringType::value_type>;

		    TargetType t(s.data());
		    return eastl::hash<TargetType>{}(t);
	    };

	    StringType sw1(LITERAL("Hello, World"));
		StringType sw2(LITERAL("Hello, World"), 5);
		StringType sw3(LITERAL("Hello"));

		VERIFY(LocalHash(sw1) != LocalHash(sw2));
		VERIFY(LocalHash(sw2) == LocalHash(sw3));
	}

	// test <=> operator
	#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	{
		StringType sw1(LITERAL("Test String "));
		StringType sw2(LITERAL("Test String 1"));
		StringType sw3(LITERAL("Test String 2"));
		StringType sw4(LITERAL("abcdef"));

		VERIFY((sw1 <=> sw2) != 0);
		VERIFY((sw1 <=> sw3) != 0);
		VERIFY((sw2 <=> sw3) != 0);
		VERIFY((sw1 <=> sw2) < 0);
		VERIFY((sw1 <=> sw3) < 0);
		VERIFY((sw2 <=> sw2) == 0);
		VERIFY((sw2 <=> sw3) < 0);
		VERIFY((sw2 <=> sw4) < 0);
		VERIFY((sw4 <=> sw2) > 0);
		VERIFY((sw4 <=> sw3) > 0);
		VERIFY((sw3 <=> sw2) > 0);
	}
	#endif

	return nErrorCount;
}

// Required to prevent manual undef of macros when 'TestString.inl' preprocessed at the top of the unit test cpp file.
#undef TEST_STRING_NAME
#undef LITERAL


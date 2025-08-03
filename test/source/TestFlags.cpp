#include "EASTLTest.h"

#include <EASTL/bonus/flags.h>


// we will be directly comparing the mask, bit-by-bit. thus we
// will need to turn on bits manually, based off a flag value
#define BIT(flag) (1 << static_cast<std::underlying_type_t<decltype(flag)>>(flag))

// we also need to take a pre-shifted flag to its integral type
#define MASKBIT(flag) static_cast<std::underlying_type_t<decltype(flag)>>(flag)



//
//  begin testing
//

enum class Wizards : uint64_t { Brilliant, Imbecile, Evil, Blase };
enum class Knights : uint32_t { Happy, Drunk, Depressed };
enum class Maidens : uint16_t { Angry = 1, Charismatic = 2, Cheerful = 4, Distressed = 8, LactoseIntolerant = 16 };

EASTL_DECLARE_BITFLAGS_ENUM_CLASS(DragonFlags, Dragons)
{
	Earth, Fire, Wind, Water, Heart
};

EASTL_DECLARE_BITFLAGS(WizardFlags, Wizards);

EASTL_DECLARE_BITFLAGS(KnightFlags, Knights);

EASTL_DECLARE_MASKFLAGS(MaidenFlags, Maidens);

EASTL_DECLARE_MASKFLAGS_ENUM_CLASS_SIZED(uint8_t, SquireFlags, Squires)
{
	Bored = 1, Churlish = 2, Keen = 4
};


static void TestFlags_test_static_typetraits()
{
	static_assert(std::is_same_v<DragonFlags::mask_type, uint32_t>,
		"We'd expect the underlying-type of Dragons (int) to mean the "
		"mask_type of the resulting basic_flags structure is uint32_t "
		"(on LP64/LLP64 data-models)");

	static_assert(std::is_same_v<WizardFlags::mask_type, uint64_t>,
		"We'd expect the underlying-type of Wizards (uint64_t) to also be the "
		"mask_type of the resulting basic_flags structure");

	static_assert(std::is_same_v<KnightFlags::mask_type, uint32_t>,
		"We'd expect the underlying-type of Knights (uint32_t) to also be the "
		"mask_type of the resulting basic_flags structure");

	static_assert(std::is_same_v<MaidenFlags::mask_type, uint16_t>,
		"We'd expect the underlying-type of Maidens (uint16_t) to also be the "
		"mask_type of the resulting basic_flags structure");

	static_assert(std::is_same_v<SquireFlags::mask_type, uint8_t>,
		"We'd expect the underlying-type of Squires (uint8_t) to also be the "
		"mask_type of the resulting basic_flags structure");

	// marshaller::to_mask should be resulting in mask_type
	static_assert(std::is_same_v<SquireFlags::mask_type, decltype(SquireFlags::marshaller_type::to_mask(Squires{}))>,
		"basic_flags<>::marshaller_type::to_mask should result in type basic_flags<>::mask_type.");
}


static int TestFlags_bitflags()
{
	int nErrorCount = 0;


	// default construction
	{
		DragonFlags dragons;

		static_assert(sizeof(dragons) == sizeof(Dragons),
			"basic_flags is not precisely the size of its constituent enum");

		// test implicit conversion to bool (false-case)
		VERIFY(!dragons);
		
		auto mask = eastl::mask_of(dragons);
		VERIFY(mask == 0);
	}

	// direct construction
	{
		DragonFlags dragons{Dragons::Fire, Dragons::Water};

		// test implicit conversion to bool (true-case)
		VERIFY(dragons);

		auto mask = eastl::mask_of(dragons);
		VERIFY(~mask ^ BIT(Dragons::Earth)); //  no
		VERIFY( mask & BIT(Dragons::Fire));  // yes
		VERIFY(~mask ^ BIT(Dragons::Wind));  //  no
		VERIFY( mask & BIT(Dragons::Water)); // yes
		VERIFY(~mask ^ BIT(Dragons::Heart)); //  no
	}

	// operator or-assignment
	{
		DragonFlags dragons;
		
		// singular value
		dragons |= Dragons::Fire;
		{
			VERIFY(dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY(~mask ^ BIT(Dragons::Earth)); //  no
			VERIFY( mask & BIT(Dragons::Fire));  // yes
			VERIFY(~mask ^ BIT(Dragons::Wind));  //  no
			VERIFY(~mask ^ BIT(Dragons::Water)); //  no
			VERIFY(~mask ^ BIT(Dragons::Heart)); //  no
		}

		// non-destructive sequential operations
		dragons |= Dragons::Wind;
		{
			VERIFY(dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY(~mask ^ BIT(Dragons::Earth)); //  no
			VERIFY( mask & BIT(Dragons::Fire));  // yes
			VERIFY( mask & BIT(Dragons::Wind));  // yes
			VERIFY(~mask ^ BIT(Dragons::Water)); //  no
			VERIFY(~mask ^ BIT(Dragons::Heart)); //  no
		}

		// same value (Fire) again doesn't change anything
		dragons |= Dragons::Fire;
		{
			VERIFY(dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY(~mask ^ BIT(Dragons::Earth)); //  no
			VERIFY( mask & BIT(Dragons::Fire));  // yes
			VERIFY( mask & BIT(Dragons::Wind));  // yes
			VERIFY(~mask ^ BIT(Dragons::Water)); //  no
			VERIFY(~mask ^ BIT(Dragons::Heart)); //  no
		}

		// multi-value
		dragons |= (Dragons::Heart | Dragons::Water);
		{
			VERIFY(dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY(~mask ^ BIT(Dragons::Earth)); //  no
			VERIFY( mask & BIT(Dragons::Fire));  // yes
			VERIFY( mask & BIT(Dragons::Wind));  // yes
			VERIFY( mask & BIT(Dragons::Water)); // yes
			VERIFY( mask & BIT(Dragons::Heart)); // yes
		}
	}

	// operator and-assignment
	{
		DragonFlags dragons{Dragons::Earth, Dragons::Fire, Dragons::Heart};

		// multi-value
		dragons &= (Dragons::Earth | Dragons::Heart);
		{
			VERIFY(dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY( mask & BIT(Dragons::Earth)); // yes
			VERIFY(~mask ^ BIT(Dragons::Fire));  //  no
			VERIFY(~mask ^ BIT(Dragons::Wind));  //  no
			VERIFY(~mask ^ BIT(Dragons::Water)); //  no
			VERIFY( mask & BIT(Dragons::Heart)); // yes
		}

		// repeated operation performs no change
		dragons &= (Dragons::Earth | Dragons::Heart);
		{
			VERIFY(dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY( mask & BIT(Dragons::Earth)); // yes
			VERIFY(~mask ^ BIT(Dragons::Fire));  //  no
			VERIFY(~mask ^ BIT(Dragons::Wind));  //  no
			VERIFY(~mask ^ BIT(Dragons::Water)); //  no
			VERIFY( mask & BIT(Dragons::Heart)); // yes
		}

		// singular value
		dragons &= Dragons::Earth;
		{
			VERIFY(dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY( mask & BIT(Dragons::Earth)); // yes
			VERIFY(~mask ^ BIT(Dragons::Fire));  //  no
			VERIFY(~mask ^ BIT(Dragons::Wind));  //  no
			VERIFY(~mask ^ BIT(Dragons::Water)); //  no
			VERIFY(~mask ^ BIT(Dragons::Heart)); //  no
		}

		// non-seen value doesn't get flipped
		dragons &= Dragons::Heart;
		{
			VERIFY(!dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY(~mask ^ BIT(Dragons::Earth)); //  no
			VERIFY(~mask ^ BIT(Dragons::Fire));  //  no
			VERIFY(~mask ^ BIT(Dragons::Wind));  //  no
			VERIFY(~mask ^ BIT(Dragons::Water)); //  no
			VERIFY(~mask ^ BIT(Dragons::Heart)); //  no
		}
	}

	// operator xor-assignment
	{
		DragonFlags dragons{Dragons::Earth, Dragons::Fire, Dragons::Heart};

		// multi-value
		dragons ^= (Dragons::Earth | Dragons::Water);
		{
			VERIFY(dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY(~mask ^ BIT(Dragons::Earth)); //  no
			VERIFY( mask & BIT(Dragons::Fire));  // yes
			VERIFY(~mask ^ BIT(Dragons::Wind));  //  no
			VERIFY( mask & BIT(Dragons::Water)); // yes
			VERIFY( mask & BIT(Dragons::Heart)); // yes
		}

		// repeated value flips flags
		dragons ^= (Dragons::Earth | Dragons::Water);
		{
			VERIFY(dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY( mask & BIT(Dragons::Earth)); // yes
			VERIFY( mask & BIT(Dragons::Fire));  // yes
			VERIFY(~mask ^ BIT(Dragons::Wind));  //  no
			VERIFY(~mask ^ BIT(Dragons::Water)); //  no
			VERIFY( mask & BIT(Dragons::Heart)); // yes
		}

		// xor us to zero
		dragons ^= DragonFlags{dragons};
		{
			VERIFY(!dragons);

			auto mask = eastl::mask_of(dragons);
			VERIFY(mask == 0);
		}
	}

	// nonmember and-operator
	{
		KnightFlags knights{Knights::Happy, Knights::Drunk};

		// rhs is a flag
		auto happiness = knights & Knights::Happy;
		auto drunkeness = knights & Knights::Drunk;
		auto depression = knights & Knights::Depressed;
		static_assert(std::is_same_v<decltype(happiness), decltype(knights)>,
			"operator 'and' should result in a flags structure");
		
		// rhs is basic_flags
		auto confused = knights & (Knights::Happy | Knights::Depressed);
		static_assert(std::is_same_v<decltype(confused), decltype(knights)>,
			"operator 'and' should result in a flags structure");

		VERIFY(happiness);
		VERIFY(drunkeness);
		VERIFY(!depression);

		// verify and-operator was non-destructive
		{
			auto mask = eastl::mask_of(knights);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY( mask & BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}

		// verify each of our resultant flags
		{
			auto mask = eastl::mask_of(happiness);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}
		{
			auto mask = eastl::mask_of(drunkeness);
			VERIFY(~mask ^ BIT(Knights::Happy));
			VERIFY( mask & BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}
		{
			auto mask = eastl::mask_of(depression);
			VERIFY(~mask ^ BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}
		{
			auto mask = eastl::mask_of(confused);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}
	}

	// nonmember or-operator
	{
		KnightFlags knights{Knights::Happy};

		// rhs is a flag
		auto happiness = (knights | Knights::Happy);
		auto drunkeness = (knights | Knights::Drunk);
		auto depression = (knights | Knights::Depressed);
		static_assert(std::is_same_v<decltype(happiness), decltype(knights)>,
			"operator 'or' should result in a flags structure");

		// rhs is basic_flags
		auto confused = knights | (Knights::Happy | Knights::Depressed);
		static_assert(std::is_same_v<decltype(confused), decltype(knights)>,
			"operator 'or' should result in a flags structure");

		VERIFY(happiness);
		VERIFY(drunkeness);
		VERIFY(depression);
		VERIFY(confused);

		// verify and-operator was non-destructive
		{
			auto mask = eastl::mask_of(knights);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}

		// verify each of our resultant flags
		{
			auto mask = eastl::mask_of(happiness);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}
		{
			auto mask = eastl::mask_of(drunkeness);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY( mask & BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}
		{
			auto mask = eastl::mask_of(depression);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY( mask & BIT(Knights::Depressed));
		}
		{
			auto mask = eastl::mask_of(confused);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY( mask & BIT(Knights::Depressed));
		}
	}

	// nonmember xor-operator
	{
		KnightFlags knights{Knights::Happy};

		// rhs is a flag
		auto happiness = (knights ^ Knights::Happy);
		auto drunkeness = (knights ^ Knights::Drunk);
		auto depression = (knights ^ Knights::Depressed);
		static_assert(std::is_same_v<decltype(happiness), decltype(knights)>,
			"operator 'xor' should result in a flags structure");

		// rhs is basic_flags
		auto confused = knights ^ (Knights::Happy | Knights::Depressed);
		static_assert(std::is_same_v<decltype(confused), decltype(knights)>,
			"operator 'xor' should result in a flags structure");

		VERIFY(!happiness);
		VERIFY(drunkeness);
		VERIFY(depression);
		VERIFY(confused);

		// verify and-operator was non-destructive
		{
			auto mask = eastl::mask_of(knights);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}

		// verify each of our resultant flags
		{
			auto mask = eastl::mask_of(happiness);
			VERIFY(~mask ^ BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}
		{
			auto mask = eastl::mask_of(drunkeness);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY( mask & BIT(Knights::Drunk));
			VERIFY(~mask ^ BIT(Knights::Depressed));
		}
		{
			auto mask = eastl::mask_of(depression);
			VERIFY( mask & BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY( mask & BIT(Knights::Depressed));
		}
		{
			auto mask = eastl::mask_of(confused);
			VERIFY(~mask ^ BIT(Knights::Happy));
			VERIFY(~mask ^ BIT(Knights::Drunk));
			VERIFY( mask & BIT(Knights::Depressed));
		}
	}


	return nErrorCount;
}



















static int TestFlags_maskflags()
{
	int nErrorCount = 0;

	// default construction
	{
		MaidenFlags maidens;

		static_assert(sizeof(maidens) == sizeof(Maidens),
			"basic_flags is not precisely the size of its constituent enum");

		// test implicit conversion to bool (false-case)
		VERIFY(!maidens);
		
		auto mask = eastl::mask_of(maidens);
		VERIFY(mask == 0);
	}

	// direct construction
	{
		MaidenFlags maidens{Maidens::Charismatic, Maidens::Cheerful};

		// test implicit conversion to bool (true-case)
		VERIFY(maidens);

		auto mask = eastl::mask_of(maidens);
		VERIFY(~mask ^ MASKBIT(Maidens::Angry));
		VERIFY( mask & MASKBIT(Maidens::Charismatic));
		VERIFY( mask & MASKBIT(Maidens::Cheerful));
		VERIFY(~mask ^ MASKBIT(Maidens::Distressed));
		VERIFY(~mask ^ MASKBIT(Maidens::LactoseIntolerant));
	}

	
	// operator or-assignment
	{
		MaidenFlags maidens;
		
		// singular value
		maidens |= Maidens::Charismatic;
		{
			VERIFY(maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY(~mask ^ MASKBIT(Maidens::Angry));
			VERIFY( mask & MASKBIT(Maidens::Charismatic));
			VERIFY(~mask ^ MASKBIT(Maidens::Cheerful));
			VERIFY(~mask ^ MASKBIT(Maidens::Distressed));
			VERIFY(~mask ^ MASKBIT(Maidens::LactoseIntolerant));
		}

		// non-destructive sequential operations
		maidens |= Maidens::Distressed;
		{
			VERIFY(maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY(~mask ^ MASKBIT(Maidens::Angry));
			VERIFY( mask & MASKBIT(Maidens::Charismatic));
			VERIFY(~mask ^ MASKBIT(Maidens::Cheerful));
			VERIFY( mask & MASKBIT(Maidens::Distressed));
			VERIFY(~mask ^ MASKBIT(Maidens::LactoseIntolerant));
		}

		// same value (Charismatic) again doesn't change anything
		maidens |= Maidens::Charismatic;
		{
			VERIFY(maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY(~mask ^ MASKBIT(Maidens::Angry));
			VERIFY( mask & MASKBIT(Maidens::Charismatic));
			VERIFY(~mask ^ MASKBIT(Maidens::Cheerful));
			VERIFY( mask & MASKBIT(Maidens::Distressed));
			VERIFY(~mask ^ MASKBIT(Maidens::LactoseIntolerant));
		}

		// multi-value
		maidens |= (Maidens::Angry | Maidens::Distressed);
		{
			VERIFY(maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY( mask & MASKBIT(Maidens::Angry));
			VERIFY( mask & MASKBIT(Maidens::Charismatic));
			VERIFY(~mask ^ MASKBIT(Maidens::Cheerful));
			VERIFY( mask & MASKBIT(Maidens::Distressed));
			VERIFY(~mask ^ MASKBIT(Maidens::LactoseIntolerant));
		}
	}

	// operator and-assignment
	{
		MaidenFlags maidens{Maidens::Angry, Maidens::Cheerful, Maidens::LactoseIntolerant};
		
		// multi-value
		maidens &= (Maidens::Angry | Maidens::Cheerful);
		{
			VERIFY(maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY( mask & MASKBIT(Maidens::Angry));
			VERIFY(~mask ^ MASKBIT(Maidens::Charismatic));
			VERIFY( mask & MASKBIT(Maidens::Cheerful));
			VERIFY(~mask ^ MASKBIT(Maidens::Distressed));
			VERIFY(~mask ^ MASKBIT(Maidens::LactoseIntolerant));
		}

		// repeated operation performs no change
		maidens &= (Maidens::Angry | Maidens::Cheerful);
		{
			VERIFY(maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY( mask & MASKBIT(Maidens::Angry));
			VERIFY(~mask ^ MASKBIT(Maidens::Charismatic));
			VERIFY( mask & MASKBIT(Maidens::Cheerful));
			VERIFY(~mask ^ MASKBIT(Maidens::Distressed));
			VERIFY(~mask ^ MASKBIT(Maidens::LactoseIntolerant));
		}

		// singular value
		maidens &= Maidens::Cheerful;
		{
			VERIFY(maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY(~mask ^ MASKBIT(Maidens::Angry));
			VERIFY(~mask ^ MASKBIT(Maidens::Charismatic));
			VERIFY( mask & MASKBIT(Maidens::Cheerful));
			VERIFY(~mask ^ MASKBIT(Maidens::Distressed));
			VERIFY(~mask ^ MASKBIT(Maidens::LactoseIntolerant));
		}

		// non-seen value doesn't get flipped
		maidens &= Maidens::Distressed;
		{
			VERIFY(!maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY(~mask ^ MASKBIT(Maidens::Angry));
			VERIFY(~mask ^ MASKBIT(Maidens::Charismatic));
			VERIFY(~mask ^ MASKBIT(Maidens::Cheerful));
			VERIFY(~mask ^ MASKBIT(Maidens::Distressed));
			VERIFY(~mask ^ MASKBIT(Maidens::LactoseIntolerant));
		}
	}

	// operator xor-assignment
	{
		MaidenFlags maidens{Maidens::Angry, Maidens::Charismatic, Maidens::LactoseIntolerant};

		// multi-value
		maidens ^= (Maidens::Charismatic | Maidens::Distressed);
		{
			VERIFY(maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY( mask & MASKBIT(Maidens::Angry));
			VERIFY(~mask ^ MASKBIT(Maidens::Charismatic));
			VERIFY(~mask ^ MASKBIT(Maidens::Cheerful));
			VERIFY( mask & MASKBIT(Maidens::Distressed));
			VERIFY( mask & MASKBIT(Maidens::LactoseIntolerant));
		}

		// repeated value flips flags
		maidens ^= (Maidens::Charismatic | Maidens::Distressed);
		{
			VERIFY(maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY( mask & MASKBIT(Maidens::Angry));
			VERIFY( mask & MASKBIT(Maidens::Charismatic));
			VERIFY(~mask ^ MASKBIT(Maidens::Cheerful));
			VERIFY(~mask ^ MASKBIT(Maidens::Distressed));
			VERIFY( mask & MASKBIT(Maidens::LactoseIntolerant));
		}

		// xor us to zero
		maidens ^= MaidenFlags{maidens};
		{
			VERIFY(!maidens);

			auto mask = eastl::mask_of(maidens);
			VERIFY(mask == 0);
		}
	}


	// nonmember or-operator
	{
		SquireFlags const squires{Squires::Bored, Squires::Churlish};

		// rhs is a flag
		auto boredom = squires | Squires::Bored;
		auto beligerence = squires | Squires::Churlish;
		auto excitement = squires | Squires::Keen;
		static_assert(std::is_same_v<decltype(boredom), std::remove_const_t<decltype(squires)>>,
			"operator 'and' should result in a flags structure");
		
		// rhs is basic_flags
		auto anticipation = squires | (Squires::Bored | Squires::Keen);
		static_assert(std::is_same_v<decltype(anticipation), std::remove_const_t<decltype(squires)>>,
			"operator 'and' should result in a flags structure");

		VERIFY(boredom);
		VERIFY(beligerence);
		VERIFY(excitement);
		VERIFY(anticipation);

		// verify and-operator was non-destructive
		{
			auto mask = eastl::mask_of(squires);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}

		// verify each of our resultant flags
		{
			auto mask = eastl::mask_of(boredom);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}
		{
			auto mask = eastl::mask_of(beligerence);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}
		{
			auto mask = eastl::mask_of(excitement);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY( mask & MASKBIT(Squires::Keen));
		}
		{
			auto mask = eastl::mask_of(anticipation);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY( mask & MASKBIT(Squires::Keen));
		}
	}
	
	// nonmember and-operator
	{
		SquireFlags const squires{Squires::Bored, Squires::Churlish};

		// rhs is a flag
		auto boredom = squires & Squires::Bored;
		auto beligerence = squires & Squires::Churlish;
		auto excitement = squires & Squires::Keen;
		static_assert(std::is_same_v<decltype(boredom), std::remove_const_t<decltype(squires)>>,
			"operator 'and' should result in a flags structure");
		
		// rhs is basic_flags
		auto anticipation = squires & (Squires::Bored | Squires::Keen);
		static_assert(std::is_same_v<decltype(anticipation), std::remove_const_t<decltype(squires)>>,
			"operator 'and' should result in a flags structure");

		VERIFY(boredom);
		VERIFY(beligerence);
		VERIFY(!excitement);

		// verify and-operator was non-destructive
		{
			auto mask = eastl::mask_of(squires);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}

		// verify each of our resultant flags
		{
			auto mask = eastl::mask_of(boredom);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY(~mask ^ MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}
		{
			auto mask = eastl::mask_of(beligerence);
			VERIFY(~mask ^ MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}
		{
			auto mask = eastl::mask_of(excitement);
			VERIFY(~mask ^ MASKBIT(Squires::Bored));
			VERIFY(~mask ^ MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}
		{
			auto mask = eastl::mask_of(anticipation);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY(~mask ^ MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}
	}

	// nonmember xor-operator
	{
		SquireFlags const squires{Squires::Bored, Squires::Churlish};

		// rhs is a flag
		auto boredom = squires ^ Squires::Bored;
		auto beligerence = squires ^ Squires::Churlish;
		auto excitement = squires ^ Squires::Keen;
		static_assert(std::is_same_v<decltype(boredom), std::remove_const_t<decltype(squires)>>,
			"operator 'and' should result in a flags structure");
		
		// rhs is basic_flags
		auto anticipation = squires ^ (Squires::Bored | Squires::Keen);
		static_assert(std::is_same_v<decltype(anticipation), std::remove_const_t<decltype(squires)>>,
			"operator 'and' should result in a flags structure");

		VERIFY(boredom);
		VERIFY(beligerence);
		VERIFY(excitement);

		// verify and-operator was non-destructive
		{
			auto mask = eastl::mask_of(squires);
			VERIFY(~mask ^ MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}

		// verify each of our resultant flags
		{
			auto mask = eastl::mask_of(boredom);
			VERIFY(~mask ^ MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}
		{
			auto mask = eastl::mask_of(beligerence);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY(~mask ^ MASKBIT(Squires::Churlish));
			VERIFY(~mask ^ MASKBIT(Squires::Keen));
		}
		{
			auto mask = eastl::mask_of(excitement);
			VERIFY( mask & MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY( mask & MASKBIT(Squires::Keen));
		}
		{
			auto mask = eastl::mask_of(anticipation);
			VERIFY(~mask ^ MASKBIT(Squires::Bored));
			VERIFY( mask & MASKBIT(Squires::Churlish));
			VERIFY( mask & MASKBIT(Squires::Keen));
		}
	}


	return nErrorCount;
}




//
// very quickly test namespace-style enums. if it compiles then
// the behaviour is identical to the very-tested enum classes above
//

namespace Colours
{
	enum Enum
	{
		Red = 1, Green = 2, Blue = 4,
		Cyan = Blue | Green,
		Yellow = Green | Red,
		Magenta = Red | Blue,
		White = Blue | Green | Red
	};
}

// test macro with namespace-style enums
EASTL_DECLARE_MASKFLAGS(ColourFlags, Colours::Enum);

static int TestFlags_namespace_style_enums()
{
	int nErrorCount = 0;


	// set
	{
		ColourFlags flags;
		flags.set(Colours::Red);
		flags.set(Colours::Green);

		VERIFY(eastl::mask_of(flags) == MASKBIT(Colours::Yellow));
	}
	// set(_, false)
	{
		ColourFlags flags{Colours::Red, Colours::Green};

		flags.set(Colours::Green, false);
		VERIFY(eastl::mask_of(flags) == MASKBIT(Colours::Red));

		flags.set(Colours::Green, true);
		VERIFY(eastl::mask_of(flags) == (MASKBIT(Colours::Red) | MASKBIT(Colours::Green)));
	}
	// unset
	{
		ColourFlags flags{Colours::Red, Colours::Green};
		flags.unset(Colours::Green);

		VERIFY(eastl::mask_of(flags) == MASKBIT(Colours::Red));
	}
	// toggle
	{
		ColourFlags flags{Colours::Red, Colours::Green};
		flags.toggle(Colours::Green)
			.toggle(Colours::Blue);

		VERIFY(eastl::mask_of(flags) == MASKBIT(Colours::Magenta));
	}


	// assignment operator
	{
		ColourFlags flags{Colours::Red, Colours::Green};
		flags = Colours::Blue;

		VERIFY(eastl::mask_of(flags) == MASKBIT(Colours::Blue));
	}
	// assignment operator for basic_flags
	{
		ColourFlags flags{Colours::Red, Colours::Green};
		ColourFlags flags2{Colours::Blue};
		flags = flags2;

		VERIFY(eastl::mask_of(flags) == MASKBIT(Colours::Blue));
	}


	// bitwise not
	{
		ColourFlags flags{Colours::Red, Colours::Green};
		auto flags2 = ~flags;

		VERIFY((eastl::mask_of(flags2) & eastl::mask_of(Colours::Red | Colours::Green)) == 0);
	}


	// comparison operators
	{
		// equality flags <-> flags
		{
			ColourFlags flags{Colours::Red, Colours::Green};
			ColourFlags flags2{Colours::Red, Colours::Green};
			VERIFY(flags == flags2);
		}
		// equality flags <-> enum
		{
			ColourFlags flags{Colours::Red};
			VERIFY(flags == Colours::Red);
		}
		// equality enum <-> flags
		{
			ColourFlags flags{Colours::Red};
			VERIFY(Colours::Red == flags);
		}

		// inequality flags <-> flags
		{
			ColourFlags flags{Colours::Red, Colours::Green};
			ColourFlags flags2{Colours::Blue};
			VERIFY(flags != flags2);
		}
		// inequality flags <-> enum
		{
			ColourFlags flags{Colours::Red};
			VERIFY(flags != Colours::Green);
		}
		// inequality enum <-> flags
		{
			ColourFlags flags{Colours::Red};
			VERIFY(Colours::Green != flags);
		}
	}

	return nErrorCount;
}

int TestFlags()
{
	int nErrorCount = 0;

	nErrorCount += TestFlags_bitflags();
	nErrorCount += TestFlags_maskflags();
	nErrorCount	+= TestFlags_namespace_style_enums();

	TestFlags_test_static_typetraits();

	return nErrorCount;
}

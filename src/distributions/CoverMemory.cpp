/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "CoverMemory.h"

// Global to make the many cases more streamlined.

struct CoverParams
{
  unsigned char cards;
  unsigned char tops1;
};

CoverParams coverGlobal;


CoverMemory::CoverMemory()
{
  CoverMemory::reset();
}


void CoverMemory::reset()
{
  specs.clear();
}


CoverSpec& CoverMemory::add()
{
  assert(coverGlobal.cards < specs.size());
  assert(coverGlobal.tops1 < specs[coverGlobal.cards].size());
  auto& covers = specs[coverGlobal.cards][coverGlobal.tops1];

  covers.emplace_back(CoverSpec());
  CoverSpec& spec = covers.back();

  // TODO Could also be a CoverParams?
  spec.oppsLength = coverGlobal.cards;
  spec.oppsTops1 = coverGlobal.tops1;
  spec.index = covers.size() - 1;

  return spec;
}



// ----- Length only -----

void CoverMemory::WestLength(const unsigned char len)
{
  // Exactly len
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(len, COVER_EQUAL);
}


void CoverMemory::EastLength(const unsigned char len)
{
  // Exactly len
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(coverGlobal.cards - len, COVER_EQUAL);
}


void CoverMemory::WestLengthRange(
  const unsigned char len1,
  const unsigned char len2)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(len1, len2, COVER_INSIDE_RANGE);
}


void CoverMemory::EastLengthRange(
  const unsigned char len1,
  const unsigned char len2)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(
    coverGlobal.cards - len2, 
    coverGlobal.cards - len1, 
    COVER_INSIDE_RANGE);
}

// ----------------------------------


void CoverMemory::WestIsNotVoid()
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(1, COVER_GREATER_EQUAL);
}


void CoverMemory::EastIsNotVoid()
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(coverGlobal.cards-1, COVER_LESS_EQUAL);
}


void CoverMemory::WestIsVoid()
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(0, COVER_EQUAL);
}


void CoverMemory::EastIsVoid()
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(coverGlobal.cards, COVER_EQUAL);
}


void CoverMemory::WestHasSingleton()
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(1, COVER_EQUAL);
}


void CoverMemory::WestHasDoubleton()
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(2, COVER_EQUAL);
}


void CoverMemory::WestHasTripleton()
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(3, COVER_EQUAL);
}


void CoverMemory::WestHasExactTops(const unsigned char len)
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_TOPS_ONLY;
  spec.westTop1.set(len, COVER_EQUAL);
}


void CoverMemory::EastHasExactTops(const unsigned char len)
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_TOPS_ONLY;
  spec.westTop1.set(coverGlobal.tops1 - len, COVER_EQUAL);
}


void CoverMemory::WestHasAtLeastTops(const unsigned char len)
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_TOPS_ONLY;
  spec.westTop1.set(len, COVER_GREATER_EQUAL);
}


void CoverMemory::WestHasCardRange(
  const unsigned char lowerIncl,
  const unsigned char upperIncl)
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(lowerIncl, upperIncl, COVER_INSIDE_RANGE);
}


void CoverMemory::WestHasShortHonors(const unsigned char len)
{
  // All tops, <= len.
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_AND_TOPS;
  spec.westLength.set(len, COVER_LESS_EQUAL);
  spec.westTop1.set(coverGlobal.tops1, COVER_EQUAL);
}


void CoverMemory::EastHasShortHonors(const unsigned char len)
{
  // All tops, <= len.
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_AND_TOPS;
  spec.westLength.set(coverGlobal.cards - len, COVER_GREATER_EQUAL);
  spec.westTop1.set(0, COVER_EQUAL);
}


void CoverMemory::WestHasAnHonor()
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_TOPS_ONLY;
  spec.westTop1.set(1, COVER_GREATER_EQUAL);
}


void CoverMemory::WestHasARangeHonor(
  const unsigned char lowerIncl,
  const unsigned char upperIncl)
{
  // West has 1+ honor in a certain length range.
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_AND_TOPS;
  spec.westLength.set(lowerIncl, upperIncl, COVER_INSIDE_RANGE);
  spec.westTop1.set(1, COVER_GREATER_EQUAL);
}


void CoverMemory::EastHasARangeHonor(
  const unsigned char lowerIncl,
  const unsigned char upperIncl)
{
  // East has 1+ honor in a certain length range.
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_AND_TOPS;
  spec.westLength.set(
    coverGlobal.cards - upperIncl, 
    coverGlobal.cards - lowerIncl, 
    COVER_INSIDE_RANGE);
  spec.westTop1.set(coverGlobal.tops1 - 1, COVER_LESS_EQUAL);
}


void CoverMemory::WestHasRangeHonors(
  const unsigned char lowerIncl,
  const unsigned char upperIncl)
{
  // West has all the honors.
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_AND_TOPS;
  spec.westLength.set(lowerIncl, upperIncl, COVER_INSIDE_RANGE);
  spec.westTop1.set(coverGlobal.tops1, COVER_EQUAL);
}


void CoverMemory::EastHasRangeHonors(
  const unsigned char lowerIncl,
  const unsigned char upperIncl)
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_AND_TOPS;
  spec.westLength.set(
    coverGlobal.cards - upperIncl, 
    coverGlobal.cards - lowerIncl, 
    COVER_INSIDE_RANGE);
  spec.westTop1.set(0, COVER_EQUAL);
}


void CoverMemory::AnHonorIsShort(const unsigned char len)
{
  // Together these two mean that an honor (there may be more) is with a
  // player with at most "len" cards.

  // West has it.
  CoverSpec& spec1 = CoverMemory::add();
  spec1.mode = COVER_LENGTHS_AND_TOPS;
  spec1.westLength.set(len, COVER_LESS_EQUAL);
  spec1.westTop1.set(1, COVER_GREATER_EQUAL);

  // East has it.
  CoverSpec& spec2 = CoverMemory::add();
  spec2.mode = COVER_LENGTHS_AND_TOPS;
  spec2.westLength.set(coverGlobal.cards - len, COVER_GREATER_EQUAL);
  spec2.westTop1.set(coverGlobal.tops1 - 1, COVER_LESS_EQUAL);
}


void CoverMemory::HonorsAreShort(const unsigned char len)
{
  // Together these two mean that the honor (or all honors) is with a
  // player with at most "len" cards.
  // TODO Not good -- generates two entries.  Separate them.
  // Maybe there's a way to make a combined one?

  // West has it/them.
  CoverSpec& spec1 = CoverMemory::add();
  spec1.mode = COVER_LENGTHS_AND_TOPS;
  spec1.westLength.set(len, COVER_LESS_EQUAL);
  spec1.westTop1.set(coverGlobal.tops1, COVER_EQUAL);

  // East has it/them.
  CoverSpec& spec2 = CoverMemory::add();
  spec2.mode = COVER_LENGTHS_AND_TOPS;
  spec2.westLength.set(coverGlobal.cards - len, COVER_GREATER_EQUAL);
  spec2.westTop1.set(0, COVER_EQUAL);
}


void CoverMemory::WestHasGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl)
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_AND_TOPS;
  spec.westLength.set(lowerCardsIncl, upperCardsIncl, COVER_INSIDE_RANGE);
  spec.westTop1.set(lowerTopsIncl, upperTopsIncl, COVER_INSIDE_RANGE);
}


void CoverMemory::EastHasGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl)
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_AND_TOPS;
  spec.westLength.set(
    coverGlobal.cards - upperCardsIncl, 
    coverGlobal.cards - lowerCardsIncl, 
    COVER_INSIDE_RANGE);
  spec.westTop1.set(
    coverGlobal.tops1 - upperTopsIncl, 
    coverGlobal.tops1 - lowerTopsIncl, 
    COVER_INSIDE_RANGE);
}


void CoverMemory::WestHasGeneralOr(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl)
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_OR_TOPS;
  spec.westLength.set(lowerCardsIncl, upperCardsIncl, COVER_INSIDE_RANGE);
  spec.westTop1.set(lowerTopsIncl, upperTopsIncl, COVER_INSIDE_RANGE);
}


void CoverMemory::EastHasGeneralOr(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl)
{
  CoverSpec& spec = CoverMemory::add();
  spec.mode = COVER_LENGTHS_OR_TOPS;
  spec.westLength.set(
    coverGlobal.cards - upperCardsIncl, 
    coverGlobal.cards - lowerCardsIncl, 
    COVER_INSIDE_RANGE);
  spec.westTop1.set(
    coverGlobal.tops1 - upperTopsIncl, 
    coverGlobal.tops1 - lowerTopsIncl, 
    COVER_INSIDE_RANGE);
}


void CoverMemory::prepare_2_1()
{
  coverGlobal = {2, 1};
  CoverMemory::WestHasSingleton();       // 1-1
  CoverMemory::WestIsNotVoid();          // 1-1 or 2=0
  CoverMemory::WestHasExactTops(1);    // West has the top
}


void CoverMemory::prepare_2_2()
{
  coverGlobal = {2, 2};
  CoverMemory::WestHasSingleton();       // 1-1
}


void CoverMemory::prepare_3_1()
{
  coverGlobal = {3, 1};
  CoverMemory::WestIsNotVoid();          // 1=2, 2=1 or 3=0
  CoverMemory::EastIsNotVoid();          // 0=3, 1=2 or 2=1

  CoverMemory::WestIsVoid();             // 0=3
  CoverMemory::EastIsVoid();             // 3=0

  CoverMemory::WestHasCardRange(1, 2); // 1=2 or 2=1

  CoverMemory::WestHasExactTops(1);    // West has the top
  CoverMemory::EastHasExactTops(1);    // East has the top

  CoverMemory::HonorsAreShort(1);      // Stiff H onside + offside

  CoverMemory::WestHasShortHonors(2);  // H or Hx onside
}


void CoverMemory::prepare_3_2()
{
  coverGlobal = {3, 2};
  CoverMemory::WestIsNotVoid();          // 1=2, 2=1 or 3=0
  CoverMemory::WestHasCardRange(1, 2); // 1=2 or 2=1

  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside

  CoverMemory::WestHasRangeHonors(2, 3); // HH(x) with West
}


void CoverMemory::prepare_3_3()
{
  coverGlobal = {3, 3};
  CoverMemory::WestHasCardRange(1, 2); // 1=2 or 2=1
}


void CoverMemory::prepare_4_1()
{
  coverGlobal = {4, 1};
  CoverMemory::WestIsNotVoid();          // 1=3, 2=2, 3=1 or 4=0
  CoverMemory::EastIsNotVoid();          // 0=4, 1=3, 2=2 or 3=1

  CoverMemory::WestIsVoid();             // 0=4
  CoverMemory::EastIsVoid();             // 4=0

  CoverMemory::WestHasDoubleton();       // 2=2
  CoverMemory::WestHasCardRange(1, 2); // 1=3 or 2=2.
  CoverMemory::WestHasCardRange(1, 3); // 1=3, 2=2 or 3=1.
  CoverMemory::WestHasCardRange(1, 2); // 1=3 or 2=2.
  CoverMemory::WestHasCardRange(2, 3); // 2=2 or 3=1.

  CoverMemory::WestHasExactTops(1);    // West has the top
  CoverMemory::EastHasExactTops(1);    // East has the top

  CoverMemory::WestHasShortHonors(2);  // H, Hx onside
  CoverMemory::WestHasShortHonors(3);  // Just not East void

  CoverMemory::WestHasRangeHonors(2, 2); // Hx with West
  CoverMemory::EastHasRangeHonors(2, 2); // Hx with East
  CoverMemory::EastHasRangeHonors(1, 3); // H, Hx, Hxx with East

  CoverMemory::HonorsAreShort(1);      // Stiff H onside + offside
}


void CoverMemory::prepare_4_2()
{
  coverGlobal = {4, 2};
  CoverMemory::WestIsVoid();             // 0=4
  CoverMemory::EastIsVoid();             // 4=0
  CoverMemory::WestIsNotVoid();          // West has 1+ cards

  CoverMemory::WestHasDoubleton();       // 2=2
  CoverMemory::WestHasCardRange(2, 3); // 2=2 or 3=1.
  CoverMemory::WestHasCardRange(1, 3); // 1=3, 2=2 or 3=1.

  CoverMemory::WestHasExactTops(0);    // HH offside, any length
  CoverMemory::WestHasExactTops(2);    // HH onside, any length

  CoverMemory::WestHasARangeHonor(1, 3); // H(xx) with West
  CoverMemory::WestHasARangeHonor(2, 3); // Hx(x) with West
  CoverMemory::EastHasARangeHonor(2, 3); // Hx(x) with East

  CoverMemory::WestHasRangeHonors(2, 3); // HH(x) with West
  CoverMemory::WestHasRangeHonors(3, 3); // HHx with West
  CoverMemory::WestHasRangeHonors(3, 4); // HHx(x) with West

  CoverMemory::WestHasGeneralAnd(2, 2, 1, 2); // Hx, HH onside
  CoverMemory::EastHasGeneralAnd(2, 2, 1, 2); // Hx, HH offside
  CoverMemory::EastHasGeneralAnd(3, 3, 2, 2); // HHx with West

  CoverMemory::EastHasGeneralOr(2, 2, 2, 2); // Both H's East or 2=2

  CoverMemory::WestHasShortHonors(2);  // HH stiff onside
  CoverMemory::EastHasShortHonors(2);  // HH stiff offside

  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside
}


void CoverMemory::prepare_4_3()
{
  coverGlobal = {4, 3};
  CoverMemory::EastIsVoid();             // 4=0
  CoverMemory::WestIsNotVoid();          // West has 1+ cards

  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside
  CoverMemory::WestHasGeneralAnd(2, 2, 1, 2);   // HH or Hx with West
  CoverMemory::WestHasGeneralAnd(2, 2, 2, 2);   // Exactly HH with West
  CoverMemory::WestHasGeneralAnd(3, 3, 3, 3);   // Exactly HHH with West

  CoverMemory::WestHasExactTops(3);    // HHH(x) onside

  CoverMemory::WestHasDoubleton();       // 2=2
  CoverMemory::WestHasCardRange(1, 3); // 1=3, 2=2 or 3=1.
}


void CoverMemory::prepare_4_4()
{
  coverGlobal = {4, 4};
  CoverMemory::WestHasDoubleton();       // 2=2
  CoverMemory::WestHasCardRange(1, 3); // 1=3, 2=2 or 3=1.
}


void CoverMemory::prepare_5_1()
{
  coverGlobal = {5, 1};
  CoverMemory::WestIsVoid();             // West is void
  CoverMemory::EastIsVoid();             // East is void
  CoverMemory::EastIsNotVoid();          // East has 1+ cards

  CoverMemory::WestHasExactTops(1);    // West has the top
  CoverMemory::EastHasExactTops(1);    // East has the top

  CoverMemory::WestHasCardRange(1, 4); // 1-4 cards each
  CoverMemory::WestHasCardRange(2, 3); // 2-3 cards each

  CoverMemory::WestHasAnHonor();         // H onside, any length

  CoverMemory::WestHasShortHonors(2);  // H, Hx onside
  CoverMemory::WestHasShortHonors(3);  // H, Hx, Hxx onside

  CoverMemory::WestHasRangeHonors(1, 4); // H, Hx, Hxx, Hxxx onside
  CoverMemory::WestHasRangeHonors(2, 3); // Hx, Hxx onside

  CoverMemory::WestHasRangeHonors(2, 2); // Specifically Hx onside
  CoverMemory::WestHasRangeHonors(2, 3); // Hx, Hxx onside
  CoverMemory::EastHasRangeHonors(1, 3); // H, Hx, Hxx with West
  CoverMemory::EastHasRangeHonors(4, 5); // Hxxx(x) with West
  CoverMemory::EastHasRangeHonors(2, 3); // Hx, Hxx offside

  CoverMemory::EastHasGeneralAnd(1, 1, 0, 0); // x offside

  CoverMemory::HonorsAreShort(1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(2);      // H, Hx onside + offside
}


void CoverMemory::prepare_5_2()
{
  coverGlobal = {5, 2};
  CoverMemory::WestIsVoid();             // West is void
  CoverMemory::EastIsVoid();             // East is void
  CoverMemory::WestIsNotVoid();          // West is not void

  CoverMemory::WestHasCardRange(1, 4); // 1-4 cards each
  CoverMemory::WestHasCardRange(2, 3); // 2-3 cards each
  CoverMemory::WestHasCardRange(0, 3); // West 0 to 3, East 5 to 2

  CoverMemory::WestHasExactTops(2);  // HH any with West
  CoverMemory::EastHasExactTops(2);  // HH any with East

  CoverMemory::WestHasShortHonors(3);  // West HH, HHx
  CoverMemory::WestHasShortHonors(4);  // Just not East void

  CoverMemory::WestHasARangeHonor(1, 3); // West has H stiff/2nd/3rd
  CoverMemory::WestHasARangeHonor(2, 3); // West has H 2nd/3rd
  CoverMemory::EastHasARangeHonor(1, 2); // East has H stiff/2nd
  CoverMemory::EastHasARangeHonor(2, 3); // East has H 2nd/3rd

  CoverMemory::WestHasGeneralAnd(1, 2, 1, 1); // H(x) onside
  CoverMemory::WestHasGeneralAnd(2, 2, 1, 1); // Hx onside
  CoverMemory::WestHasGeneralAnd(2, 3, 2, 2); // HH(x) onside
  CoverMemory::EastHasGeneralAnd(2, 2, 1, 1); // Hx offside
  CoverMemory::EastHasGeneralAnd(1, 1, 0, 0); // x offside

  CoverMemory::WestHasRangeHonors(4, 5); // HHHx(x) onside

  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(2);      // Stiff HH onside + offside

  CoverMemory::WestHasShortHonors(4);  // Just not East void
}


void CoverMemory::prepare_5_3()
{
  coverGlobal = {5, 3};
  CoverMemory::EastIsVoid();             // East is void

  CoverMemory::WestHasCardRange(1, 4); // 1-4 cards each
  CoverMemory::WestHasCardRange(2, 3); // 2-3 cards each
  CoverMemory::WestHasCardRange(2, 5); // 2-5 cards with West

  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside

  CoverMemory::WestHasRangeHonors(4, 4); // HHHx / x
  CoverMemory::WestHasRangeHonors(4, 5); // HHHx(x)

  CoverMemory::WestHasGeneralOr(0, 3, 3, 3); // West all H's or 0-3c
  CoverMemory::EastHasGeneralOr(0, 3, 3, 3); // East all H's or 0-3c
}


void CoverMemory::prepare_5_4()
{
  coverGlobal = {5, 4};
  CoverMemory::EastIsVoid();             // East is void

  CoverMemory::WestHasCardRange(1, 4); // 1-4 cards each
  CoverMemory::WestHasCardRange(2, 3); // 2-3 cards each
}


void CoverMemory::prepare_5_5()
{
  coverGlobal = {5, 5};
  CoverMemory::WestHasCardRange(1, 4); // 1-4 cards each
  CoverMemory::WestHasCardRange(2, 3); // 2-3 cards each
}


void CoverMemory::prepare_6_1()
{
  coverGlobal = {6, 1};
  CoverMemory::WestIsVoid();             // West is void
  CoverMemory::EastIsVoid();             // East is void

  CoverMemory::WestHasExactTops(1);    // West has the top

  CoverMemory::WestHasTripleton();       // 3-3
  CoverMemory::WestHasCardRange(2, 4); // 2-4 cards each
  CoverMemory::WestHasCardRange(1, 5); // 1-5 cards each

  CoverMemory::EastHasExactTops(1);    // H with East, any length

  CoverMemory::WestHasShortHonors(2);  // H or Hx onside
  CoverMemory::WestHasShortHonors(3);  // H, Hx or Hxx onside

  CoverMemory::WestHasRangeHonors(3, 3); // Hxx with West
  CoverMemory::EastHasRangeHonors(1, 5); // H(xxxx) with East

  CoverMemory::HonorsAreShort(1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(2);      // H or Hx onside + offside
}


void CoverMemory::prepare_6_2()
{
  coverGlobal = {6, 2};
  CoverMemory::WestHasTripleton();       // 3-3
  CoverMemory::WestHasCardRange(2, 4); // 2-4 cards each
  CoverMemory::WestHasCardRange(1, 5); // 1-5 cards each

  CoverMemory::WestHasAtLeastTops(1);  // West has 1+ tops

  CoverMemory::WestHasAtLeastTops(2);  // West has both tops

  CoverMemory::WestHasRangeHonors(3, 3); // HHx with West
  CoverMemory::EastHasRangeHonors(3, 3); // HHx with East

  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(2);      // Stiff HH onside + offside

  CoverMemory::WestHasGeneralAnd(2, 5, 1, 1); // Hx(xxx) with West
  CoverMemory::WestHasGeneralAnd(4, 6, 2, 2); // HHxx(xx) with West

  CoverMemory::WestHasGeneralAnd(2, 2, 1, 1); // Hx with West
  CoverMemory::WestHasGeneralAnd(2, 3, 2, 2); // HH(x) with West
}


void CoverMemory::prepare_6_3()
{
  coverGlobal = {6, 3};
  CoverMemory::WestHasTripleton();       // 3-3
  CoverMemory::WestHasCardRange(2, 4); // 2-4 cards each
  CoverMemory::WestHasCardRange(1, 5); // 1-5 cards each

  CoverMemory::WestHasRangeHonors(3, 3); // HHx with West
  CoverMemory::EastHasRangeHonors(3, 3); // HHx with East

  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside

  CoverMemory::WestHasGeneralAnd(1, 2, 1, 2); // H, Hx, HH onside
  CoverMemory::WestHasGeneralAnd(4, 6, 2, 2); // HHxx(xx) onside

  CoverMemory::WestHasGeneralAnd(2, 2, 2, 2); // Stiff HH with West
  CoverMemory::EastHasGeneralAnd(2, 2, 2, 2); // Stiff HH with East

  CoverMemory::WestHasGeneralOr(3, 3, 3, 3); // 3-3 or HHH onside
  CoverMemory::WestHasGeneralAnd(2, 2, 1, 1); // Hx with West
}


void CoverMemory::prepare_6_4()
{
  coverGlobal = {6, 4};
  CoverMemory::WestHasTripleton();       // 3-3
  CoverMemory::WestHasCardRange(2, 4); // 2-4 cards each
  CoverMemory::WestHasCardRange(1, 5); // 1-5 cards each

  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside
}


void CoverMemory::prepare_6_5()
{
  coverGlobal = {6, 5};
  CoverMemory::WestHasTripleton();       // 3-3
  CoverMemory::WestHasCardRange(2, 4); // 2-4 cards each
  CoverMemory::WestHasCardRange(1, 5); // 1-5 cards each
}


void CoverMemory::prepare_6_6()
{
  coverGlobal = {6, 6};
  CoverMemory::WestHasTripleton();       // 3-3
  CoverMemory::WestHasCardRange(2, 4); // 2-4 cards each
  CoverMemory::WestHasCardRange(1, 5); // 1-5 cards each
}


void CoverMemory::prepare_7_1()
{
  coverGlobal = {7, 1};
  CoverMemory::EastIsVoid();             // East is void

  CoverMemory::WestHasExactTops(1);    // West has the top

  CoverMemory::HonorsAreShort(1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(2);      // H at most doubleton
  CoverMemory::HonorsAreShort(3);      // H at most tripleton
}


void CoverMemory::prepare_7_2()
{
  coverGlobal = {7, 2};
  CoverMemory::WestHasCardRange(3, 4); // 3-4 cards each
  CoverMemory::WestHasCardRange(2, 5); // 2-5 cards each
  CoverMemory::WestHasCardRange(1, 6); // 1-6 cards each

  CoverMemory::WestHasExactTops(2);    // HH(xxxxx) with West

  CoverMemory::WestHasRangeHonors(2, 3); // HH(x) with West
  CoverMemory::EastHasRangeHonors(2, 3); // HH(x) with East

  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(2);      // H or Hx onside + offside

  CoverMemory::WestHasGeneralAnd(1, 2, 1, 1); // H(x) with West
  CoverMemory::WestHasShortHonors(2); // HH stiff with West
}


void CoverMemory::prepare_7_3()
{
  coverGlobal = {7, 3};
  CoverMemory::WestHasGeneralAnd(3, 3, 3, 3); // HHH with West
  CoverMemory::EastHasGeneralAnd(3, 3, 3, 3); // HHH with East
}


void CoverMemory::prepare_8_1()
{
  coverGlobal = {8, 1};
  CoverMemory::WestHasExactTops(1);    // West has the top

  CoverMemory::EastIsVoid();             // East is void

  CoverMemory::HonorsAreShort(1);      // Stiff H onside + offside

  CoverMemory::HonorsAreShort(2);      // H at most doubleton
}


void CoverMemory::prepare_8_2()
{
  coverGlobal = {8, 2};
  CoverMemory::AnHonorIsShort(1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(2);      // Stiff HH onside + offside
}


void CoverMemory::prepare_9_1()
{
  coverGlobal = {9, 1};
  CoverMemory::HonorsAreShort(1);      // Stiff H onside + offside
}


void CoverMemory::prepare(const unsigned char maxCards)
{
  specs.resize(maxCards+1);
  for (unsigned char c = 0; c <= maxCards; c++)
    // Just to have enough for now
    specs[c].resize(14);

  CoverMemory::prepare_2_1();
  CoverMemory::prepare_2_2();

  CoverMemory::prepare_3_1();
  CoverMemory::prepare_3_2();
  CoverMemory::prepare_3_3();

  CoverMemory::prepare_4_1();
  CoverMemory::prepare_4_2();
  CoverMemory::prepare_4_3();
  CoverMemory::prepare_4_4();

  CoverMemory::prepare_5_1();
  CoverMemory::prepare_5_2();
  CoverMemory::prepare_5_3();
  CoverMemory::prepare_5_4();
  CoverMemory::prepare_5_5();

  CoverMemory::prepare_6_1();
  CoverMemory::prepare_6_2();
  CoverMemory::prepare_6_3();
  CoverMemory::prepare_6_4();
  CoverMemory::prepare_6_5();
  CoverMemory::prepare_6_6();

  CoverMemory::prepare_7_1();
  CoverMemory::prepare_7_2();
  CoverMemory::prepare_7_3();

  CoverMemory::prepare_8_1();
  CoverMemory::prepare_8_2();

  CoverMemory::prepare_9_1();
}


list<CoverSpec>::const_iterator CoverMemory::begin(
  const unsigned cards,
  const unsigned tops1) const
{
  assert(cards < specs.size());
if (tops1 >= specs[cards].size())
{
cout << "cards " << cards << endl;
cout << "tops1 " << tops1 << ", size " << specs[cards].size() << endl;
  assert(tops1 < specs[cards].size());
}

  return specs[cards][tops1].begin();
}


list<CoverSpec>::const_iterator CoverMemory::end(
  const unsigned cards,
  const unsigned tops1) const
{
  assert(cards < specs.size());
  assert(tops1 < specs[cards].size());

  return specs[cards][tops1].end();
}


string CoverMemory::str(
  const unsigned cards,
  const unsigned tops1) const
{
  string s = "";

  for (auto iter = CoverMemory::begin(cards, tops1); 
      iter != CoverMemory::end(cards, tops1); iter++)
  {
    s += iter->strLength() + + " (oper) " + iter->strTop1() + "\n";
  }
    
  return s; 
}


/*

void Covers::prepareSpecific(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const vector<unsigned char>& cases,
  const unsigned char maxLength,
  const unsigned char maxTops,
  list<Cover>::iterator& iter)
{
  iter = covers.begin();
  CoverSpec spec;

  // This loop does combinations of the form <=, ==, >=.

  for (unsigned char length = 0; length <= maxLength; length++)
  {
    spec.westLength.setValue(length);
    for (unsigned char top = 0; top <= maxTops; top++)
    {
      spec.westTop1.setValue(top);
      for (unsigned mode = 0; mode < COVER_MODE_SIZE; mode++)
      {
        spec.mode = static_cast<CoverMode>(mode);
        for (unsigned lOper = 0; lOper < COVER_OPERATOR_SIZE-1; lOper++)
        {
          if ((length == 0 || length == maxLength) && lOper != COVER_EQUAL)
            continue;

          spec.westLength.setOperator(static_cast<CoverOperator>(lOper));

          for (unsigned tOper = 0; tOper < COVER_OPERATOR_SIZE-1; tOper++)
          {
            if ((top == 0 || top == maxTops) && tOper != COVER_EQUAL)
              continue;

            spec.westTop1.setOperator(static_cast<CoverOperator>(tOper));

            assert(iter != covers.end());
            iter->prepare(lengths, tops, cases, spec);
            iter++;
          }
        }
      }
    }
  }
}


void Covers::prepareMiddles(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const vector<unsigned char>& cases,
  const unsigned char maxLength,
  const unsigned char maxTops,
  list<Cover>::iterator& iter)
{
  // This loop does more global distributions of the form
  // "4-2 or better".

  CoverSpec spec;
  spec.westLength.setOperator(static_cast<CoverOperator>(COVER_INSIDE_RANGE));

  // With 5 or 6 cards, we run from 1 to 2 as the lower end.
  const unsigned char middleCount = (maxLength-1) >> 1;

  for (unsigned char length = 1; length <= middleCount; length++)
  {
    spec.westLength.setValues(length, maxLength-length);
    for (unsigned char top = 0; top <= maxTops; top++)
    {
      spec.westTop1.setValue(top);
      for (unsigned mode = 0; mode < COVER_MODE_SIZE; mode++)
      {
        spec.mode = static_cast<CoverMode>(mode);
        for (unsigned tOper = 0; tOper < COVER_OPERATOR_SIZE-1; tOper++)
        {
          if ((top == 0 || top == maxTops) && tOper != COVER_EQUAL)
            continue;

          spec.westTop1.setOperator(static_cast<CoverOperator>(tOper));

          assert(iter != covers.end());
          iter->prepare(lengths, tops, cases, spec);
          iter++;
        }
      }
    }
  }
}


void Covers::prepare(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const vector<unsigned char>& cases,
  const unsigned char maxLength,
  const unsigned char maxTops)
{
  assert(lengths.size() == tops.size());
  assert(maxLength >= 2);
  assert(maxTops >= 1);

  // We consider two kinds of distribution information:
  // (1) <=, ==, >= a specific number of West cards.
  // (2) Both West and East in a certain middle range.
  // 
  // For (1) there are generally 3 operators for each of lengths and tops;
  // 4 ways to combine them; and a number of possible lengths and tops.
  // For the maximum and minimum value of lengths and tops there is
  // only one operator, ==.
  //
  // For (2) there are (maxLength-1) >> 1 interesting splits.
  // For example, for 7 cards there are any 4-3; up to 5-2; up to 6-1,
  // and (7-1) >> 1 is 3.  For 8 cards there are also 3, as 4-4 is
  // already covered by the "exactly 4 West cards" split above.

  const unsigned middleCount = (maxLength-1) >> 1;

  unsigned count1, count2;
  if (maxTops == 1)
  {
    count1 = COVER_MODE_SIZE *
      ((maxLength-1) * 2 * (COVER_OPERATOR_SIZE-1) * 1 + 
      2 * 2 * 1 * 1);
    
    count2 = COVER_MODE_SIZE * middleCount * 2 * 1 * 1;
  }
  else
  {
    count1 = COVER_MODE_SIZE *
      ((maxLength-1) * (maxTops-1) * 
        (COVER_OPERATOR_SIZE-1) * (COVER_OPERATOR_SIZE-1) +
      2 * (maxTops-1) * 1 * (COVER_OPERATOR_SIZE-1) +
      (maxLength-1) * 2 * (COVER_OPERATOR_SIZE-1) * 1 +
      2 * 2 * 1 * 1);

    count2 = COVER_MODE_SIZE * middleCount *
      ((maxTops-1) * 1 * (COVER_OPERATOR_SIZE-1) +
      2 * 1 * 1 );
  }

  const unsigned coverCount = count1 + count2;

  covers.resize(coverCount);

  list<Cover>::iterator iter;
  Covers::prepareSpecific(lengths, tops, cases, maxLength, maxTops, iter);
  Covers::prepareMiddles(lengths, tops, cases, maxLength, maxTops, iter);
  assert(iter == covers.end());

  covers.sort([](const Cover& cover1, const Cover& cover2)
  {
    return (cover1.getWeight() >= cover2.getWeight());
  });
}

*/


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


CoverSpec& CoverMemory::addOrExtend(const unsigned specNumber)
{
  assert(coverGlobal.cards < specs.size());
  assert(coverGlobal.tops1 < specs[coverGlobal.cards].size());
  auto& covers = specs[coverGlobal.cards][coverGlobal.tops1];

  if (specNumber == 0)
  {
    // Add
    covers.emplace_back(CoverSpec());
    CoverSpec& spec = covers.back();

    // TODO Could also be a CoverParams?
    spec.oppsLength = coverGlobal.cards;
    spec.oppsTops1 = coverGlobal.tops1;
    spec.index = covers.size() - 1;

    return spec;
  }
  else
  {
    // Extend
    return covers.back();
  }
}



// ----- Length only -----

void CoverMemory::WestLength(
  const unsigned char len,
  const unsigned specNumber,
  const bool invertFlag)
{
  // Exactly len

  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = invertFlag;
  spec.mode[specNumber] = COVER_LENGTHS_ONLY;
  spec.westLength[specNumber].set(len, COVER_EQUAL);
}


void CoverMemory::EastLength(
  const unsigned char len,
  const unsigned specNumber,
  const bool invertFlag)
{
  // Exactly len
  // TODO CoverMemory::WestLength(coverGlobal.cards - len, invertFlag);
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = invertFlag;
  spec.mode[specNumber] = COVER_LENGTHS_ONLY;
  spec.westLength[specNumber].set(coverGlobal.cards - len, COVER_EQUAL);
}


void CoverMemory::WestLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber,
  const bool invertFlag)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = invertFlag;
  spec.mode[specNumber] = COVER_LENGTHS_ONLY;
  spec.westLength[specNumber].set(len1, len2, COVER_INSIDE_RANGE);
}


void CoverMemory::EastLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber,
  const bool invertFlag)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = invertFlag;
  spec.mode[specNumber] = COVER_LENGTHS_ONLY;
  spec.westLength[specNumber].set(
    coverGlobal.cards - len2, 
    coverGlobal.cards - len1, 
    COVER_INSIDE_RANGE);
}


// ----- Top-1 only -----

void CoverMemory::WestTop1(
  const unsigned char len,
  const unsigned specNumber,
  const bool invertFlag)
{
  // Exactly len
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = invertFlag;
  spec.mode[specNumber] = COVER_TOPS_ONLY;
  spec.westTop1[specNumber].set(len, COVER_EQUAL);
}


void CoverMemory::EastTop1(
  const unsigned char len,
  const unsigned specNumber,
  const bool invertFlag)
{
  // Exactly len
  // TODO CoverMemory::WestLength(coverGlobal.cards - len, invertFlag);
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = invertFlag;
  spec.mode[specNumber] = COVER_TOPS_ONLY;
  spec.westTop1[specNumber].set(coverGlobal.tops1 - len, COVER_EQUAL);
}


void CoverMemory::WestTop1Range(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber,
  const bool invertFlag)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = invertFlag;
  spec.mode[specNumber] = COVER_TOPS_ONLY;
  spec.westTop1[specNumber].set(len1, len2, COVER_INSIDE_RANGE);
}


void CoverMemory::EastTop1Range(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber,
  const bool invertFlag)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = invertFlag;
  spec.mode[specNumber] = COVER_TOPS_ONLY;
  spec.westTop1[specNumber].set(
    coverGlobal.tops1 - len2, 
    coverGlobal.tops1 - len1, 
    COVER_INSIDE_RANGE);
}

// ----- Length AND top-1 -----

void CoverMemory::WestGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl,
  const unsigned specNumber)
{
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = false;
  spec.mode[specNumber] = COVER_LENGTHS_AND_TOPS;

  if (lowerCardsIncl == upperCardsIncl)
    spec.westLength[specNumber].set(lowerCardsIncl, COVER_EQUAL);
  else
    spec.westLength[specNumber].set(
      lowerCardsIncl, upperCardsIncl, COVER_INSIDE_RANGE);

  if (lowerTopsIncl == upperTopsIncl)
    spec.westTop1[specNumber].set(lowerTopsIncl, COVER_EQUAL);
  else
    spec.westTop1[specNumber].set(
      lowerTopsIncl, upperTopsIncl, COVER_INSIDE_RANGE);
}


void CoverMemory::EastGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl,
  const unsigned specNumber)
{
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.invertFlag[specNumber] = false;
  spec.mode[specNumber] = COVER_LENGTHS_AND_TOPS;

  if (lowerCardsIncl == upperCardsIncl)
    spec.westLength[specNumber].set(
      coverGlobal.cards - lowerCardsIncl, COVER_EQUAL);
  else
    spec.westLength[specNumber].set(
      coverGlobal.cards - upperCardsIncl, 
      coverGlobal.cards - lowerCardsIncl, 
      COVER_INSIDE_RANGE);

  if (lowerTopsIncl == upperTopsIncl)
    spec.westTop1[specNumber].set(
      coverGlobal.tops1 - lowerTopsIncl, COVER_EQUAL);
  else
    spec.westTop1[specNumber].set(
      coverGlobal.tops1 - upperTopsIncl, 
      coverGlobal.tops1 - lowerTopsIncl, 
      COVER_INSIDE_RANGE);
}


void CoverMemory::SymmGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl)
{
  CoverMemory::WestGeneralAnd(lowerCardsIncl, upperCardsIncl,
    lowerTopsIncl, upperTopsIncl);

  CoverMemory::EastGeneralAnd(lowerCardsIncl, upperCardsIncl,
    lowerTopsIncl, upperTopsIncl, 1);
  
  // TODO Solve more elegantly?
  specs[coverGlobal.cards][coverGlobal.tops1].back().symmFlag = true;
}


void CoverMemory::prepare_2_1()
{
  coverGlobal = {2, 1};
  CoverMemory::WestLength(0, 0, true);  // West is not void
  CoverMemory::WestLength(1);           // 1-1

  CoverMemory::WestTop1(1);             // West has the top
}


void CoverMemory::prepare_2_2()
{
  coverGlobal = {2, 2};
  CoverMemory::WestLength(1);           // 1-1
}


void CoverMemory::prepare_3_1()
{
  coverGlobal = {3, 1};
  CoverMemory::WestLength(0);           // West is void
  CoverMemory::EastLength(0);           // East is void
  CoverMemory::WestLength(0, 0, true);  // West is not void
  CoverMemory::WestLengthRange(1, 2);   // 1=2 or 2=1

  CoverMemory::EastLength(0, 0, true);  // East is not void

  CoverMemory::WestTop1(1);             // West has the top
  CoverMemory::EastTop1(1);             // East has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East

  // OR these two together:  Singleton honor either side.
  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1, 1); // H with East
}


void CoverMemory::prepare_3_2()
{
  coverGlobal = {3, 2};
  CoverMemory::WestLength(0, 0, true);  // West is not void
  CoverMemory::WestLengthRange(1, 2);   // 1=2 or 2=1

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // HH(x) with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
}


void CoverMemory::prepare_3_3()
{
  coverGlobal = {3, 3};
  CoverMemory::WestLengthRange(1, 2);   // 1=2 or 2=1
}


void CoverMemory::prepare_4_1()
{
  coverGlobal = {4, 1};
  CoverMemory::WestLength(0);           // West is void
  CoverMemory::EastLength(0);           // East is void
  CoverMemory::WestLength(0, 0, true);  // West is not void
  CoverMemory::EastLength(0, 0, true);  // East is not void
  CoverMemory::WestLength(2);           // 2=2
  CoverMemory::WestLengthRange(1, 2);   // 1=2 or 2=1
  CoverMemory::WestLengthRange(1, 3);   // 1=3, 2=2 or 3=1
  CoverMemory::WestLengthRange(2, 3);   // 2=2 or 3=1

  CoverMemory::WestTop1(1);             // West has the top
  CoverMemory::EastTop1(1);             // East has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with West
  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // H(xx) with West
  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // Hx with West
  CoverMemory::EastGeneralAnd(2, 2, 1, 1); // Hx with East

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // Hx with East
}


void CoverMemory::prepare_4_2()
{
  coverGlobal = {4, 2};
  CoverMemory::WestLength(0);           // West is void
  CoverMemory::EastLength(0);           // East is void
  CoverMemory::WestLength(0, 0, true);  // West is not void
  CoverMemory::WestLength(2);           // 2=2
  CoverMemory::WestLengthRange(1, 3);   // 1=3, 2=2 or 3=1
  CoverMemory::WestLengthRange(2, 3);   // 2=2 or 3=1

  CoverMemory::WestTop1(2);             // HH onside
  CoverMemory::EastTop1(2);             // HH offside

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // H(xx) with West
  CoverMemory::WestGeneralAnd(2, 2, 1, 2); // Hx, HH with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // HH with West
  CoverMemory::WestGeneralAnd(2, 3, 1, 1); // Hx(x) with West
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // HH(x) with West
  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // HHx with West
  CoverMemory::WestGeneralAnd(3, 4, 2, 2); // HHx(x) with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(2, 2, 1, 2); // Hx, HH with East
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // HH with East
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // Hx(x) with East
  CoverMemory::EastGeneralAnd(3, 3, 2, 2); // HHx with East

  // OR these two together:  Singleton honor either side.
  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1, 1); // H with East

  // TODO Simplify comment OR these two together:  East or West has HH
  // CoverMemory::WestGeneralAnd(2, 2, 2, 2);
  // CoverMemory::EastGeneralAnd(2, 2, 2, 2, 1);
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2);
}


void CoverMemory::prepare_4_3()
{
  coverGlobal = {4, 3};
  CoverMemory::EastLength(0);           // East is void
  CoverMemory::WestLength(0, 0, true);  // West is not void
  CoverMemory::WestLength(2);           // 2=2
  CoverMemory::WestLengthRange(1, 3);   // 1=3, 2=2 or 3=1

  CoverMemory::WestTop1(3);             // HHH(x) onside

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // Exactly HH with West
  CoverMemory::WestGeneralAnd(3, 3, 3, 3); // Exactly HHH with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
}


void CoverMemory::prepare_4_4()
{
  coverGlobal = {4, 4};
  CoverMemory::WestLength(2);           // 2=2
  CoverMemory::WestLengthRange(1, 3);   // 1=3, 2=2 or 3=1
}


void CoverMemory::prepare_5_1()
{
  coverGlobal = {5, 1};
  CoverMemory::WestLength(0);           // West is void
  CoverMemory::EastLength(0);           // East is void
  CoverMemory::EastLength(0, 0, true);  // East is not void
  CoverMemory::WestLengthRange(2, 3);   // 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);   // 1=4, 2=3, 3=2 or 4=1

  CoverMemory::WestTop1(1);             // West has the top
  CoverMemory::EastTop1(1);             // East has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with West
  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // H(xx) with West
  CoverMemory::WestGeneralAnd(1, 4, 1, 1); // H(xxx) with West
  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // Hx with West
  CoverMemory::WestGeneralAnd(2, 3, 1, 1); // Hx(x) with West

  CoverMemory::EastGeneralAnd(1, 1, 0, 0); // x with East
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // H(x) with East
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // H(xx) with East
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // Hx(x) with East
  CoverMemory::EastGeneralAnd(4, 5, 1, 1); // Hxxx(x) with East
}


void CoverMemory::prepare_5_2()
{
  coverGlobal = {5, 2};
  CoverMemory::WestLength(0);           // West is void
  CoverMemory::EastLength(0);           // East is void
  CoverMemory::WestLength(0, 0, true);  // West is not void
  CoverMemory::WestLengthRange(2, 3);   // 2=3 or 3=2
  CoverMemory::WestLengthRange(0, 3);   // 0=5, 1=4, 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);   // 1=4, 2=3, 3=2 or 4=1

  CoverMemory::WestTop1(2);             // West has the tops
  CoverMemory::EastTop1(2);             // East has the tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with West
  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // H(xx) with West
  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // Hx with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // HH with West
  CoverMemory::WestGeneralAnd(2, 3, 1, 1); // Hx(x) with West
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // HH(x) with West
  CoverMemory::WestGeneralAnd(2, 4, 2, 2); // HH(xx) with West
  CoverMemory::WestGeneralAnd(4, 5, 2, 2); // HHxx(x) with West

  CoverMemory::EastGeneralAnd(1, 1, 0, 0); // x with East
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with East
  CoverMemory::EastGeneralAnd(2, 2, 1, 1); // Hx with East
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // HH with East
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // Hx(x) with East
}


void CoverMemory::prepare_5_3()
{
  coverGlobal = {5, 3};
  CoverMemory::EastLength(0);           // East is void
  CoverMemory::WestLengthRange(2, 3);   // 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);   // 1=4, 2=3, 3=2 or 4=1
  CoverMemory::WestLengthRange(2, 5);   // 2=3, 3=2, 4=1 or 5=0

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(4, 4, 3, 3); // HHHx with West
  CoverMemory::WestGeneralAnd(4, 5, 3, 3); // HHHx(x) with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East

  // OR these two together:  West has 0-3c or all honors
  CoverMemory::WestLengthRange(0, 3);
  CoverMemory::WestTop1(3, 1);

  // OR these two together:  East has 0-3c or all honors
  CoverMemory::EastLengthRange(0, 3);
  CoverMemory::EastTop1(3, 1);
}


void CoverMemory::prepare_5_4()
{
  coverGlobal = {5, 4};
  CoverMemory::EastLength(0);           // East is void
  CoverMemory::WestLengthRange(2, 3);   // 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);   // 1=4, 2=3, 3=2 or 4=1
}


void CoverMemory::prepare_5_5()
{
  coverGlobal = {5, 5};
  CoverMemory::WestLengthRange(2, 3);   // 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);   // 1=4, 2=3, 3=2 or 4=1
}


void CoverMemory::prepare_6_splitsNonVoid()
{
  CoverMemory::WestLength(3);           // 3=3
  CoverMemory::WestLengthRange(2, 4);   // 2=4, 3=3 or 4=2
  CoverMemory::WestLengthRange(1, 5);   // 1=5, 2=4, 3=3, 4=2 or 5=1
}


void CoverMemory::prepare_6_1()
{
  coverGlobal = {6, 1};
  CoverMemory::WestLength(0);           // West is void
  CoverMemory::EastLength(0);           // East is void
  CoverMemory::prepare_6_splitsNonVoid();

  CoverMemory::WestTop1(1);             // West has the top
  CoverMemory::EastTop1(1);             // East has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with West
  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // H(xx) with West
  CoverMemory::WestGeneralAnd(3, 3, 1, 1); // Hxx with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(1, 5, 1, 1); // H(xxxx) with East
}


void CoverMemory::prepare_6_2()
{
  coverGlobal = {6, 2};
  CoverMemory::prepare_6_splitsNonVoid();

  CoverMemory::WestTop1(2);             // West has both tops
  CoverMemory::WestTop1Range(1, 2);     // West has 1-2 tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // Hx with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // HH with West
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // HH(x) with West
  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // HHx with West
  CoverMemory::WestGeneralAnd(2, 5, 1, 1); // Hx(xxx) with West
  CoverMemory::WestGeneralAnd(4, 6, 2, 2); // HHxx(xx) with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // HH with East
  CoverMemory::EastGeneralAnd(3, 3, 2, 2); // HHx with East

  CoverMemory::SymmGeneralAnd(1, 3, 1, 2); // 3-3 or single/double honor(s)

  // TODO Delete comment: OR these two together:  East or West has HH
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2);
  // CoverMemory::WestGeneralAnd(2, 2, 2, 2);
  // CoverMemory::EastGeneralAnd(2, 2, 2, 2, 1);

  // OR these two together:  West has both H's or any 3-3
  CoverMemory::WestLength(3);
  CoverMemory::WestTop1(2, 1);
}


void CoverMemory::prepare_6_3()
{
  coverGlobal = {6, 3};
  CoverMemory::prepare_6_splitsNonVoid();

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 2); // H, Hx, HH onside
  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // Hx with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // HH with West
  CoverMemory::WestGeneralAnd(3, 3, 3, 3); // HHH with West
  CoverMemory::WestGeneralAnd(4, 6, 2, 2); // HHxx(xx) onside

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // HH with East
  CoverMemory::WestGeneralAnd(3, 3, 3, 3); // HHH with East

  // OR these two together:  West has all H's or any 3-3
  CoverMemory::WestLength(3);
  CoverMemory::WestTop1(3, 1);
}


void CoverMemory::prepare_6_4()
{
  coverGlobal = {6, 4};
  CoverMemory::prepare_6_splitsNonVoid();

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
}


void CoverMemory::prepare_6_5()
{
  coverGlobal = {6, 5};
  CoverMemory::prepare_6_splitsNonVoid();
}


void CoverMemory::prepare_6_6()
{
  coverGlobal = {6, 6};
  CoverMemory::prepare_6_splitsNonVoid();
}


void CoverMemory::prepare_7_1()
{
  coverGlobal = {7, 1};
  CoverMemory::EastLength(0);           // East is void

  CoverMemory::WestTop1(1);             // West has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with West
  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // H(xx) with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // H(x) with East
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // H(xx) with East
}


void CoverMemory::prepare_7_2()
{
  coverGlobal = {7, 2};
  CoverMemory::WestLengthRange(3, 4);   // 3-4 cards each
  CoverMemory::WestLengthRange(2, 5);   // 2-5 cards each
  CoverMemory::WestLengthRange(1, 6);   // 1-6 cards each

  CoverMemory::WestTop1(2);             // West has the tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // HH with West
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // HH(x) with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // HH with East
  CoverMemory::EastGeneralAnd(2, 3, 2, 2); // HH(x) with East
}


void CoverMemory::prepare_7_3()
{
  coverGlobal = {7, 3};
  CoverMemory::WestGeneralAnd(3, 3, 3, 3); // HHH with West
  CoverMemory::EastGeneralAnd(3, 3, 3, 3); // HHH with East
}


void CoverMemory::prepare_8_1()
{
  coverGlobal = {8, 1};
  CoverMemory::EastLength(0);           // East is void

  CoverMemory::WestTop1(1);             // West has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // H(x) with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // H(x) with East
}


void CoverMemory::prepare_8_2()
{
  coverGlobal = {8, 2};

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // HH with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // HH with East
}


void CoverMemory::prepare_9_1()
{
  coverGlobal = {9, 1};

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // H with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // H with East
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


unsigned CoverMemory::size(
  const unsigned cards,
  const unsigned tops1) const
{
  assert(cards < specs.size());
  assert(tops1 < specs[cards].size());

  return specs[cards][tops1].size();
}


string CoverMemory::str(
  const unsigned cards,
  const unsigned tops1) const
{
  string s = "";

  for (auto iter = CoverMemory::begin(cards, tops1); 
      iter != CoverMemory::end(cards, tops1); iter++)
  {
    s += iter->str() + "\n";
  }
    
  return s; 
}


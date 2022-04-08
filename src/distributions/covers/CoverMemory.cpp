/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "Covers.h"

#include "CoverMemory.h"
#include "ExplStats.h"

#include "Profile.h"
#include "ProfilePair.h"
#include "Product.h"
#include "ProductMemory.h"

// Global to make the many cases more streamlined.

struct CoverParams
{
  ProductMemory * memoryPtr;
  Profile * sumProfilePtr;
  unsigned char cards;
  unsigned char tops1;
  unsigned char numTops;
  unsigned char index;
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


void CoverMemory::resize(const unsigned char maxCards)
{
  counts.resize(maxCards+1);
  for (unsigned char c = 0; c <= maxCards; c++)
    counts[c].resize(14);
}


void CoverMemory::resizeStats(ExplStats& explStats) const
{
  explStats.resize(counts);
}


CoverSpec& CoverMemory::add()
{
  specs.emplace_back(CoverSpec());
  CoverSpec& spec = specs.back();
  spec.setID(
    coverGlobal.numTops,
    coverGlobal.cards, 
    coverGlobal.tops1);
  spec.setIndex(specs.size() - 1);

  return spec;
}


// ----- Length only -----

void CoverMemory::WestLength(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverMemory::WestLengthRange(len, len, ctrl);
}


void CoverMemory::EastLength(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverMemory::WestLength(coverGlobal.cards - len, ctrl);
}


void CoverMemory::WestLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverMemory::WestGeneralAnd(
    len1, len2, 0, coverGlobal.tops1, ctrl);
  // CoverSpec& spec = CoverMemory::add();
  // spec.westLengthRange(* coverGlobal.memoryPtr, len1, len2, ctrl);

  /*
  const unsigned char highestTop =
    static_cast<unsigned char>(coverGlobal.sumProfilePtr->size()-1);

  ProfilePair profilePair(* coverGlobal.sumProfilePtr);
  profilePair.setLength(len1, len2);
  profilePair.addTop(highestTop, 0,
    coverGlobal.sumProfilePtr->count(highestTop));

  list<Cover> coverList;
  coverList.emplace_back(Cover);
  Cover& cover = coverList.back();

  cover.set(
    * coverGlobal.productMemoryPtr,
    * coverGlobal.sumProfilePtr,
    profilePair,
    false);

  covers.prepareRowNew(
    coverList,
    * coverGlobal.sumProfilePtr,
    counts[coverGlobal.maxLength][coverGlobal.maxTops],
    distProfiles, 
    cases);

  counts[coverGlobal.maxLength][coverGlobal.maxTops]++;
  */
}


void CoverMemory::EastLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverMemory::WestLengthRange(
    coverGlobal.cards - len2,
    coverGlobal.cards - len1,
    ctrl);
}


// ----- Top-1 only -----

void CoverMemory::WestTop1(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverMemory::WestTop1Range(len, len, ctrl);
}


void CoverMemory::EastTop1(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverMemory::WestTop1(coverGlobal.tops1 - len, ctrl);
}


void CoverMemory::WestTop1Range(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  // CoverSpec& spec = CoverMemory::add();
  // spec.westTop1Range(* coverGlobal.memoryPtr, len1, len2, ctrl);

  CoverMemory::WestGeneralAnd(
    0, coverGlobal.cards, len1, len2, ctrl);
}


void CoverMemory::EastTop1Range(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverMemory::WestTop1Range(
    coverGlobal.tops1 - len2,
    coverGlobal.tops1 - len1,
    ctrl);
}

// ----- Length AND top-1 -----

void CoverMemory::WestGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::add();
  spec.westGeneral(* coverGlobal.memoryPtr, lowerCardsIncl, upperCardsIncl,
    lowerTopsIncl, upperTopsIncl, false, ctrl);
}


void CoverMemory::EastGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl,
  const CoverControl ctrl)
{
  CoverMemory::WestGeneralAnd(
    coverGlobal.cards - upperCardsIncl,
    coverGlobal.cards - lowerCardsIncl,
    coverGlobal.tops1 - upperTopsIncl,
    coverGlobal.tops1 - lowerTopsIncl,
    ctrl);
}


void CoverMemory::SymmGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::add();
  spec.westGeneral(* coverGlobal.memoryPtr, lowerCardsIncl, upperCardsIncl,
    lowerTopsIncl, upperTopsIncl, true, ctrl);
}


void CoverMemory::WestGeneralTwo(
  const unsigned char lowerCardsIncl1,
  const unsigned char upperCardsIncl1,
  const unsigned char lowerTopsIncl1,
  const unsigned char upperTopsIncl1,
  const unsigned char lowerCardsIncl2,
  const unsigned char upperCardsIncl2,
  const unsigned char lowerTopsIncl2,
  const unsigned char upperTopsIncl2)
{
  CoverSpec& spec = CoverMemory::add();

  spec.westGeneral(* coverGlobal.memoryPtr, 
    lowerCardsIncl1, upperCardsIncl1,
    lowerTopsIncl1, upperTopsIncl1, 
    false, 
    COVER_ADD);

  spec.westGeneral(* coverGlobal.memoryPtr, 
    lowerCardsIncl2, upperCardsIncl2,
    lowerTopsIncl2, upperTopsIncl2, 
    false, 
    COVER_EXTEND);
}


void CoverMemory::prepare_2_1()
{
  CoverMemory::WestLength(1);              // 0. 1-1
  CoverMemory::WestLengthRange(1, 2);      // 1. West is not void
  CoverMemory::WestTop1(1);                // 2. West has the top
}


void CoverMemory::prepare_2_2()
{
  CoverMemory::WestLength(1);              // 0. 1-1
}


void CoverMemory::prepare_3_1()
{
  CoverMemory::WestLengthRange(1, 3);      // 0. West is not void
  CoverMemory::WestLengthRange(0, 2);      // 1. East is not void
  CoverMemory::WestLengthRange(1, 2);      // 2. 1=2 or 2=1
  CoverMemory::WestLength(0);              // 3. West is void
  CoverMemory::EastLength(0);              // 4. East is void

  CoverMemory::WestTop1(1);                // 5. West has the top
  CoverMemory::EastTop1(1);                // 6. East has the top

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 7. Singleton H on either side
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 8. H(x) with West
}


void CoverMemory::prepare_3_2()
{
  CoverMemory::WestLengthRange(1, 3);      // 0. West is not void
  CoverMemory::WestLengthRange(1, 2);      // 1. 1=2 or 2=1

  CoverMemory::WestTop1(2);                // 2. West has both tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 3. H singleton with West
}


void CoverMemory::prepare_3_3()
{
  CoverMemory::WestLengthRange(1, 2);      // 0. 1=2 or 2=1
}


void CoverMemory::prepare_4_1()
{
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLength(2);              // 2. 2=2
  CoverMemory::WestLengthRange(0, 2);      // 3. 0=4, 1=3, 2=2
  CoverMemory::WestLengthRange(1, 2);      // 4. 1=3, 2=2
  CoverMemory::WestLengthRange(2, 4);      // 5. 2=2, 3=1, 4=0
  CoverMemory::WestLengthRange(0, 3);      // 6. East is not void
  CoverMemory::WestLengthRange(1, 3);      // 7. Nobody is void
  CoverMemory::WestLengthRange(1, 4);      // 8. West is not void
  CoverMemory::WestLengthRange(2, 3);      // 9. 2=2, 3=1

  CoverMemory::WestTop1(1);                // 10. West has the top
  CoverMemory::EastTop1(1);                // 11. East has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 12. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 13. H singleton with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 14. H singleton either way

  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // 15. Hx with West
  CoverMemory::EastGeneralAnd(2, 2, 1, 1); // 16. Hx with East

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 17. H(x) with West

  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // 19. H(xx) with West
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // 20. H(xx) with East

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 21. Hxx=H
}


void CoverMemory::prepare_4_2()
{
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::EastLength(1);              // 1. East has any singleton
  CoverMemory::WestLength(2);              // 2. 2=2

  CoverMemory::WestLengthRange(0, 2);      // 3. 0=4, 1=3, 2=2
  CoverMemory::WestLengthRange(0, 3);      // 4. East is not void
  CoverMemory::WestLengthRange(1, 2);      // 5. 1=3 or 2=2
  CoverMemory::WestLengthRange(1, 3);      // 6. 1=3, 2=2 or 3=1
  CoverMemory::WestLengthRange(1, 4);      // 7. West is not void
  CoverMemory::WestLengthRange(2, 3);      // 8. 2=2 or 3=1
  CoverMemory::WestLengthRange(2, 4);      // 9. 2=2, 3=1 or 4=0

  CoverMemory::WestTop1(2);                // 10. HH West
  CoverMemory::WestTop1Range(1, 2);        // 11. H or HH West
  CoverMemory::EastTop1(2);                // 10. HH East

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 12. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 13. H singleton with East

  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 14. HH doubleton with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 15. HH doubleton with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 16. HH doubleton either side

  CoverMemory::WestGeneralAnd(2, 3, 1, 1); // 17. Hx(x) with West
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // 18. HH(x) with West
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // 19. Hx(x) with East

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // X. H, Hx with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 2); // X. H, Hx, HH with West
  CoverMemory::EastGeneralAnd(1, 2, 1, 2); // X. H, Hx, HH with East
  CoverMemory::WestGeneralAnd(1, 3, 1, 2); // 20. No void; West has an H
  CoverMemory::WestGeneralAnd(2, 3, 1, 2); // X. West has 2-3 with 1+ top
  CoverMemory::WestGeneralAnd(2, 4, 1, 2); // X. West has 2-4 with 1+ top
  CoverMemory::WestGeneralAnd(3, 4, 2, 2); // 21. HHx(x) with West

  CoverMemory::EastGeneralAnd(3, 3, 2, 2); // 22. HHx with East
  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // 23. HHx with West

  // 24. West has exactly one top, or the suit splits 2=2.
  CoverMemory::WestGeneralTwo(
    0, 4, 1, 1,  // WestTop1(1)
    2, 2, 0, 2); // WestLength(2)

  // 25. West has both tops, or the suit splits 2=2.
  CoverMemory::WestGeneralTwo(
    0, 4, 2, 2,  // WestTop1(2)
    2, 2, 0, 2); // WestLength(2)

  // 26. East has exactly one top, or the suit splits 2=2.
  CoverMemory::WestGeneralTwo(
    0, 4, 1, 1,  // EastTop1(1)
    2, 2, 0, 2); // EastLength(2)

  // 27. East has both tops, or the suit splits 2=2.
  CoverMemory::WestGeneralTwo(
    0, 4, 0, 0,  // EastTop1(2)
    2, 2, 0, 2); // EastLength(2)

  // 28. West has both tops, or West has H, Hx, HH.
  CoverMemory::WestGeneralTwo(
    0, 4, 2, 2,  // WestTop1(2)
    1, 2, 1, 2); // WestGeneralAnd(1, 2, 1, 2)

  // 29. East has both tops, or East has H, Hx, HH.
  CoverMemory::WestGeneralTwo(
    0, 4, 0, 0,  // EastTop1(2)
    2, 3, 0, 1); // EastGeneralAnd(1, 2, 1, 2)

  // 30. East has at most 2 cards, or East has the tops.
  CoverMemory::WestGeneralTwo(
    0, 4, 0, 0,  // EastTop1(2)
    2, 4, 0, 2); // EastLengthRange(0, 2)
}


void CoverMemory::prepare_4_3()
{
  CoverMemory::WestLength(2);              // 0. 2=2
  CoverMemory::WestLengthRange(1, 3);      // 1. 1=3, 2=2 or 3=1
  CoverMemory::WestLengthRange(1, 4);      // 2. West is not void

  CoverMemory::WestTop1(3);                // 3. West has all tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 4. H singleton with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 5. HH doubleton with West
}


void CoverMemory::prepare_4_4()
{
  CoverMemory::WestLength(2);              // 0. 2=2
  CoverMemory::WestLengthRange(1, 3);      // 1. 1=3, 2=2 or 3=1
}


void CoverMemory::prepare_5_1()
{
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLengthRange(0, 4);      // 2. East is not void
  CoverMemory::WestLengthRange(1, 4);      // 3. 1=4, 2=3, 3=2 or 4=1
  CoverMemory::WestLengthRange(2, 3);      // 4. 3-2 either way

  CoverMemory::WestTop1(1);                // 5. West has the top
  CoverMemory::EastTop1(1);                // 6. East has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 7. H with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 8. H with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 9. H singleton either side

  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // 10. Hx doubleton with West

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 11. H(x) with West
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // 12. H(x) with East
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 13. H(x) either way

  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // 14. H(xx) with West
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // 15. H(xx) with East

  CoverMemory::WestGeneralAnd(1, 4, 1, 1); // 16. H(xxx) with West

  CoverMemory::WestGeneralAnd(2, 3, 1, 1); // 17. Hx(x) with West
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // 18. Hx(x) with East

  CoverMemory::WestGeneralAnd(4, 4, 1, 1); // 19. Hxxx with West

  CoverMemory::EastGeneralAnd(4, 5, 1, 1); // 20. Hxxx(x) with East

  // X. 2=3, 3=2 or East has the top.
  CoverMemory::WestGeneralTwo(
    0, 5, 0, 0,  // EastTop1(1)
    2, 3, 0, 1); // WestLengthRange(2, 3)
}


void CoverMemory::prepare_5_2()
{
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLengthRange(0, 3);      // 2. 0=5, 1=4, 2=3 or 3=2
  CoverMemory::WestLengthRange(0, 4);      // 3. East is not void
  CoverMemory::WestLengthRange(1, 4);      // 4. 1=4, 2=3, 3=2 or 4=1
  CoverMemory::WestLengthRange(1, 5);      // X. West is not void
  CoverMemory::WestLengthRange(2, 3);      // 5. 2=3 or 3=2
  CoverMemory::WestLengthRange(2, 5);      // 6. 2=3, 3=2, 4=1 or 5=0

  CoverMemory::WestTop1(2);                // 7. West has the tops
  CoverMemory::WestTop1Range(1, 2);        // 8. West has 1+ tops
  CoverMemory::EastTop1(2);                // 7. West has the tops

  CoverMemory::EastGeneralAnd(0, 2, 0, 0); // X. -, x, xx with East

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 9. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 10. H singleton with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 11. H singleton either way

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 12. H(x) with West
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // 13. H(x) with East

  CoverMemory::WestGeneralAnd(1, 2, 1, 2); // X. H, Hx, HH West
  CoverMemory::EastGeneralAnd(1, 2, 1, 2); // X. H, Hx, HH East

  CoverMemory::WestGeneralAnd(1, 3, 1, 2); // X. H, Hx, HH, Hxx, HHx West

  CoverMemory::WestGeneralAnd(1, 1, 0, 0); // 14. x with West
  CoverMemory::EastGeneralAnd(1, 1, 0, 0); // 15. x with East

  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 16. HH doubleton with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 17. HH doubleton with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 18. HH doubleton either way

  CoverMemory::WestGeneralAnd(2, 2, 0, 0); // 19. West xx
  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // 20. West Hx
  CoverMemory::WestGeneralAnd(2, 2, 1, 2); // 21. HH with West

  CoverMemory::WestGeneralAnd(2, 3, 1, 2); // 22. Hx(x) with West
  CoverMemory::EastGeneralAnd(2, 3, 1, 2); // 22. Hx(x) with East
  CoverMemory::EastGeneralAnd(2, 3, 2, 2); // 23. HH(x) with East

  CoverMemory::WestGeneralAnd(2, 5, 1, 2); // X. 2+ cards, 1+ top West

  CoverMemory::WestGeneralAnd(3, 3, 1, 1); // 24. Hxx with West
  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // 25. HHx with West

  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // 25. HH, HHx with West

  // 29. 3-2 either way, or West has both H's.
  CoverMemory::WestGeneralTwo(
    0, 5, 2, 2,  // WestTop1(2)
    2, 3, 0, 2); // WestLengthRange(2, 3)

  // 29. 3-2 either way, or East has both H's.
  CoverMemory::WestGeneralTwo(
    0, 5, 0, 0,  // EastTop1(2)
    2, 3, 0, 2); // EastLengthRange(2, 3)
}


void CoverMemory::prepare_5_3()
{
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLengthRange(1, 4);      // 2. 1=4, 2=3, 3=2 or 4=1
  CoverMemory::WestLengthRange(1, 5);      // 3. West is not void
  CoverMemory::WestLengthRange(2, 3);      // 4. 2=3 or 3=2
  CoverMemory::WestLengthRange(2, 5);      // 5. 2=3, 3=2, 4=1 or 5=0

  CoverMemory::WestLengthRange(0, 4);      // 6. East is not void

  CoverMemory::WestTop1Range(1, 3);        // 5. West has 1+ tops

  CoverMemory::WestGeneralAnd(1, 1, 0, 0); // 8. x with West

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 7. H with West

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 7. H with East

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 7. H with either

  CoverMemory::WestGeneralAnd(4, 4, 3, 3); // 8. HHHx / x

  CoverMemory::EastGeneralAnd(3, 3, 3, 3); // 7. HHH with East
}


void CoverMemory::prepare_5_4()
{
  CoverMemory::WestLengthRange(1, 5);      // 0. East is not void
  CoverMemory::WestLengthRange(2, 3);      // 1. 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);      // 2. 1=4, 2=3, 3=2 or 4=1
}


void CoverMemory::prepare_5_5()
{
  CoverMemory::WestLengthRange(2, 3);      // 0. 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);      // 1. 1=4, 2=3, 3=2 or 4=1
}


void CoverMemory::prepare_6_1()
{
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLength(3);              // 2. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 3. 2=4, 3=3 or 4=2

  CoverMemory::WestTop1(1);                // 4. West has the top
  CoverMemory::EastTop1(1);                // 5. East has the top

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 6. H singleton either way
  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 7. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 8. East has H singleton

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 9. H(x) with West
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // 10. H(x) with East
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 11. H(x) on either side

  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // 12. H(xx) with West
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // 12. H(xx) with East
  CoverMemory::WestGeneralAnd(3, 3, 1, 1); // 13. Hxx with West
}


void CoverMemory::prepare_6_2()
{
  CoverMemory::EastLength(0);              // 0. West is void
  CoverMemory::WestLength(0);              // 1. West is void
  CoverMemory::WestLength(3);              // 2. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 3. 2=4, 3=3 or 4=2

  CoverMemory::WestTop1(2);                // 4. West has both tops
  CoverMemory::WestTop1Range(1, 2);        // 5. West has 1-2 tops
  CoverMemory::EastTop1(2);                // 4. East has both tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 6. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 7. H singleton with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1);

  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 8. HH doubleton with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 9. HH doubleton with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 10. HH doubleton either side

  CoverMemory::WestGeneralAnd(1, 2, 0, 0); // X. x(x) with West
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 11. H(x) with West

  CoverMemory::WestGeneralAnd(1, 3, 1, 2); // 12. 1-3 or H, Hx, HH stiff W
  CoverMemory::SymmGeneralAnd(1, 3, 1, 2); // 12. 1-3 or H, Hx, HH stiff
  CoverMemory::EastGeneralAnd(3, 3, 2, 2); // 13. HHx with East
  CoverMemory::EastGeneralAnd(2, 2, 1, 1); // 14. Hx with East
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // 15. HH(x) with West
  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // 16. HHx with West
  CoverMemory::WestGeneralAnd(4, 5, 2, 2); // 16. HHxx(x) with West
}


void CoverMemory::prepare_6_3()
{
  CoverMemory::WestLength(3);              // 0. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 1. 2=4, 3=3 or 4=2

  CoverMemory::WestTop1(3);                // 2. West has all tops

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 3. H singleton either side
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 4. HH doubleton either side
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 5. H, Hx with West

  CoverMemory::WestGeneralAnd(4, 6, 3, 3); // 6. HHH(xxx) with West

  CoverMemory::EastGeneralAnd(3, 3, 1, 3); // 7. 3=3, 1+ H with East

  // 8. 2=4 or singleton top
  CoverMemory::WestGeneralTwo(
    0, 6, 3, 3,  // WestTop1(3)
    2, 4, 0, 3); // WestLengthRange(2, 4)
}


void CoverMemory::prepare_6_4()
{
  CoverMemory::WestLength(3);              // 0. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 1. 2=4, 3=3 or 4=2

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 2. H singleton
}


void CoverMemory::prepare_6_5()
{
  CoverMemory::WestLength(3);              // 0. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 1. 2=4, 3=3 or 4=2
}


void CoverMemory::prepare_6_6()
{
  CoverMemory::WestLength(3);              // 0. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 1. 2=4, 3=3 or 4=2
}


void CoverMemory::prepare_7_1()
{
  CoverMemory::WestTop1(1);                // 0. West has the top

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 1. H(x) with West

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 2. H either side
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 3. H(x) either side
  CoverMemory::SymmGeneralAnd(1, 3, 1, 1); // 4. H(xx) either side
}


void CoverMemory::prepare_7_2()
{
  CoverMemory::WestTop1(2);                // 0. West has the tops

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 1. H singleton

  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 2. HH with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 3. HH doubleton

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 4. H(x) with West
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 5. H(x) either side
  CoverMemory::SymmGeneralAnd(1, 2, 1, 2); // 5. H(x) either side

  CoverMemory::SymmGeneralAnd(2, 3, 2, 2); // 6. HH(x) either side
}


void CoverMemory::prepare_7_3()
{
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 0. HH singleton either side
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 1. HH doubleton either side
  CoverMemory::SymmGeneralAnd(3, 3, 3, 3); // 2. HHH tripleton either side
}


void CoverMemory::prepare_8_1()
{
  CoverMemory::WestTop1(1);                // 0. West has the top
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 1. H singleton either side
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 2. H(x) either side
}


void CoverMemory::prepare_8_2()
{
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 0. H singleton either side
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 1. HH doubleton either side
}


void CoverMemory::prepare_9_1()
{
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 0. H singleton either side
}


void CoverMemory::prepare(
  const unsigned char maxLength,
  const unsigned char maxTops)
{
  if (maxLength == 2)
  {
    if (maxTops == 1)
      CoverMemory::prepare_2_1();
    else if (maxTops == 2)
      CoverMemory::prepare_2_2();
    else
      assert(false);
  }
  else if (maxLength == 3)
  {
    if (maxTops == 1)
      CoverMemory::prepare_3_1();
    else if (maxTops == 2)
      CoverMemory::prepare_3_2();
    else if (maxTops == 3)
      CoverMemory::prepare_3_3();
    else
      assert(false);
  }
  else if (maxLength == 4)
  {
    if (maxTops == 1)
      CoverMemory::prepare_4_1();
    else if (maxTops == 2)
      CoverMemory::prepare_4_2();
    else if (maxTops == 3)
      CoverMemory::prepare_4_3();
    else if (maxTops == 4)
      CoverMemory::prepare_4_4();
    else
      assert(false);
  }
  else if (maxLength == 5)
  {
    if (maxTops == 1)
      CoverMemory::prepare_5_1();
    else if (maxTops == 2)
      CoverMemory::prepare_5_2();
    else if (maxTops == 3)
      CoverMemory::prepare_5_3();
    else if (maxTops == 4)
      CoverMemory::prepare_5_4();
    else if (maxTops == 5)
      CoverMemory::prepare_5_5();
    else
      assert(false);
  }
  else if (maxLength == 6)
  {
    if (maxTops == 1)
      CoverMemory::prepare_6_1();
    else if (maxTops == 2)
      CoverMemory::prepare_6_2();
    else if (maxTops == 3)
      CoverMemory::prepare_6_3();
    else if (maxTops == 4)
      CoverMemory::prepare_6_4();
    else if (maxTops == 5)
      CoverMemory::prepare_6_5();
    else if (maxTops == 6)
      CoverMemory::prepare_6_6();
    else
      assert(false);
  }
  else if (maxLength == 7)
  {
    if (maxTops == 1)
      CoverMemory::prepare_7_1();
    else if (maxTops == 2)
      CoverMemory::prepare_7_2();
    else if (maxTops == 3)
      CoverMemory::prepare_7_3();
    else
      cout << "WARN maxTops " << +maxLength << " / " << +maxTops << endl;
  }
  else if (maxLength == 8)
  {
    if (maxTops == 1)
      CoverMemory::prepare_8_1();
    else if (maxTops == 2)
      CoverMemory::prepare_8_2();
    else
      cout << "WARN maxTops " << +maxLength << " / " << +maxTops << endl;
  }
  else if (maxLength == 9)
  {
    if (maxTops == 1)
      CoverMemory::prepare_9_1();
    else
      cout << "WARN maxTops " << +maxLength << " / " << +maxTops << endl;
  }
  else
    cout << "WARN maxTops " << +maxLength << " / " << +maxTops << endl;
}


void CoverMemory::prepareRows(
  Covers& covers,
  ProductMemory& productMemory,
  const unsigned char maxLength,
  const unsigned char maxTops,
  const unsigned char numTops,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases) // Should be const again some day?
{
  assert(maxLength < counts.size());
  assert(maxTops < counts[maxLength].size());

  specs.clear();

  Profile sumProfile;
  sumProfile.setSingle(numTops, maxLength, maxTops);

  coverGlobal = {&productMemory, &sumProfile, maxLength, maxTops, numTops, 0};

  CoverMemory::prepare(maxLength, maxTops);

  counts[maxLength][maxTops] = specs.size();

  for (auto& spec: specs)
    covers.prepareRow(spec, distProfiles, cases);
}


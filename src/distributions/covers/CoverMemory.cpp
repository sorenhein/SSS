/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "CoverMemory.h"
#include "Compositions.h"
#include "ExplStats.h"

#include "CoverSetNew.h"

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


void CoverMemory::resizeStats(ExplStats& explStats) const
{
  explStats.resize(specs);
}


CoverSpec& CoverMemory::addOrExtend(const CoverControl ctrl)
{
  assert(coverGlobal.cards < specs.size());
  assert(coverGlobal.tops1 < specs[coverGlobal.cards].size());
  auto& covers = specs[coverGlobal.cards][coverGlobal.tops1];

  if (ctrl == COVER_ADD)
  {
    // Add
    covers.emplace_back(CoverSpec());
    CoverSpec& spec = covers.back();
    spec.setID(coverGlobal.cards, coverGlobal.tops1);
    spec.setIndex(covers.size() - 1);

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
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.westLength(len, ctrl);
}


void CoverMemory::EastLength(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.eastLength(len, ctrl);
}


void CoverMemory::WestLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.westLengthRange(len1, len2, ctrl);
}


void CoverMemory::EastLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.eastLengthRange(len1, len2, ctrl);
}


// ----- Top-1 only -----

void CoverMemory::WestTop1(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.westTop1(len, ctrl);
}


void CoverMemory::EastTop1(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.eastTop1(len, ctrl);
}


void CoverMemory::WestTop1Range(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.westTop1Range(len1, len2, ctrl);
}


void CoverMemory::EastTop1Range(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.eastTop1Range(len1, len2, ctrl);
}

// ----- Length AND top-1 -----

void CoverMemory::WestGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.westGeneral(lowerCardsIncl, upperCardsIncl,
    lowerTopsIncl, upperTopsIncl, false, ctrl);
}


void CoverMemory::EastGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.eastGeneral(lowerCardsIncl, upperCardsIncl,
    lowerTopsIncl, upperTopsIncl, false, ctrl);
}


void CoverMemory::SymmGeneralAnd(
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl,
  const CoverControl ctrl)
{
  CoverSpec& spec = CoverMemory::addOrExtend(ctrl);
  spec.westGeneral(lowerCardsIncl, upperCardsIncl,
    lowerTopsIncl, upperTopsIncl, true, ctrl);
}


void CoverMemory::prepare_2_1()            // ***** DONE-2 *****
{
  coverGlobal = {2, 1};
  CoverMemory::WestLength(1);              // 0. 1-1
  CoverMemory::WestLengthRange(1, 2);      // 1. West is not void
  CoverMemory::WestTop1(1);                // 2. West has the top
}


void CoverMemory::prepare_2_2()            // ***** DONE-2 *****
{
  coverGlobal = {2, 2};
  CoverMemory::WestLength(1);              // 0. 1-1
}


void CoverMemory::prepare_3_1()            // ***** DONE-2 *****
{
  coverGlobal = {3, 1};

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


void CoverMemory::prepare_3_2()            // ***** DONE-2 *****
{
  coverGlobal = {3, 2};
  CoverMemory::WestLengthRange(1, 3);      // 0. West is not void
  CoverMemory::WestLengthRange(1, 2);      // 1. 1=2 or 2=1

  // 2. West has both tops or there is a singleton honor.
  CoverMemory::WestTop1(2);

  // 3. West has a singleton honor.
  CoverMemory::WestGeneralAnd(1, 1, 1, 1);
}


void CoverMemory::prepare_3_3()            // ***** DONE-2 *****
{
  coverGlobal = {3, 3};
  CoverMemory::WestLengthRange(1, 2);      // 0. 1=2 or 2=1
}


void CoverMemory::prepare_4_1()            // ***** DONE-1 *****
{
  coverGlobal = {4, 1};
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
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 18. H(x) on either side

  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // 19. H(xx) with West
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // 20. H(xx) with East

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 21. Hxx=H
}


void CoverMemory::prepare_4_2()
{
  coverGlobal = {4, 2};
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLength(2);              // 2. 2=2

  CoverMemory::WestLengthRange(0, 2);      // 3. 0=4, 1=3, 2=2
  CoverMemory::WestLengthRange(0, 3);      // 4. East is not void
  CoverMemory::WestLengthRange(1, 2);      // 5. 1=3 or 2=2
  CoverMemory::WestLengthRange(1, 3);      // 6. 1=3, 2=2 or 3=1
  CoverMemory::WestLengthRange(1, 4);      // 7. West is not void
  CoverMemory::WestLengthRange(2, 3);      // 8. 2=2 or 3=1
  CoverMemory::WestLengthRange(2, 4);      // 9. 2=2, 3=1 or 4=0

  CoverMemory::WestTop1(2);                // 10. HH onside
  CoverMemory::WestTop1Range(1, 2);        // 11. H or HH onside

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 12. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 13. H singleton with East

  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 14. HH doubleton with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 15. HH doubleton with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 16. HH doubleton either side

  CoverMemory::WestGeneralAnd(2, 3, 1, 1); // 17. Hx(x) with West
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // 18. HH(x) with West
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // 19. Hx(x) with East

  CoverMemory::WestGeneralAnd(1, 3, 1, 2); // 20. No void; West has an H
  CoverMemory::WestGeneralAnd(3, 4, 2, 2); // 21. HHx(x) with West

  CoverMemory::EastGeneralAnd(3, 3, 2, 2); // 22. HHx with East
  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // 23. HHx with West

  // 24. West has exactly one top, or the suit splits 2=2.
  CoverMemory::WestTop1(1);
  CoverMemory::WestLength(2, COVER_EXTEND);
}


void CoverMemory::prepare_4_3()            // ***** DONE-1 *****
{
  coverGlobal = {4, 3};
  CoverMemory::WestLength(2);              // 0. 2=2
  CoverMemory::WestLengthRange(1, 3);      // 1. 1=3, 2=2 or 3=1
  CoverMemory::WestLengthRange(1, 4);      // 2. West is not void

  CoverMemory::WestTop1(3);                // 3. West has all tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 4. H singleton with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 5. HH doubleton with West
}


void CoverMemory::prepare_4_4()            // ***** DONE-1 *****
{
  coverGlobal = {4, 4};
  CoverMemory::WestLength(2);              // 0. 2=2
  CoverMemory::WestLengthRange(1, 3);      // 1. 1=3, 2=2 or 3=1
}


void CoverMemory::prepare_5_1()
{
  coverGlobal = {5, 1};
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLengthRange(1, 4);      // 2. 1=4, 2=3, 3=2 or 4=1
  CoverMemory::WestLengthRange(0, 4);      // 3. East is not void

  CoverMemory::WestTop1(1);                // 4. West has the top
  CoverMemory::EastTop1(1);                // 5. East has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 6. H with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 7. H with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 8. H singleton either side

  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // 9. Hx doubleton with West

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 10. H(x) with West
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // 11. H(x) with East
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 12. H(x) either way

  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // 13. H(xx) with West
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // 14. H(xx) with East

  CoverMemory::WestGeneralAnd(1, 4, 1, 1); // 15. H(xxx) with West

  CoverMemory::WestGeneralAnd(2, 3, 1, 1); // 16. Hx(x) with West
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // 17. Hx(x) with East
  CoverMemory::SymmGeneralAnd(2, 3, 1, 1); // 18. Hx(x) either side

  CoverMemory::WestGeneralAnd(4, 4, 1, 1); // 19. Hxxx with West

  CoverMemory::EastGeneralAnd(4, 5, 1, 1); // 20. Hxxx(x) with East
}


void CoverMemory::prepare_5_2()
{
  coverGlobal = {5, 2};
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLengthRange(0, 3);      // 2. 0=5, 1=4, 2=3 or 3=2
  CoverMemory::WestLengthRange(0, 4);      // 3. East is not void
  CoverMemory::WestLengthRange(1, 4);      // 4. 1=4, 2=3, 3=2 or 4=1
  CoverMemory::WestLengthRange(2, 3);      // 5. 2=3 or 3=2
  CoverMemory::WestLengthRange(2, 5);      // 6. 2=3, 3=2, 4=1 or 5=0

  CoverMemory::WestTop1(2);                // 7. West has the tops
  CoverMemory::WestTop1Range(1, 2);        // 8. West has 1+ tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 9. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 10. H singleton with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 11. H singleton either way

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 12. H(x) with West
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // 13. H(x) with East

  CoverMemory::WestGeneralAnd(1, 1, 0, 0); // 14. x with West
  CoverMemory::EastGeneralAnd(1, 1, 0, 0); // 15. x with East

  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 16. HH doubleton with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 17. HH doubleton with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 18. HH doubleton either way

  CoverMemory::WestGeneralAnd(2, 2, 0, 0); // 19. West xx
  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // 20. West Hx
  CoverMemory::WestGeneralAnd(2, 2, 1, 2); // 21. HH with West

  CoverMemory::WestGeneralAnd(2, 3, 1, 2); // 22. Hx(x) with East
  CoverMemory::SymmGeneralAnd(2, 3, 2, 2); // 23. HH(x) with West

  CoverMemory::WestGeneralAnd(3, 3, 1, 1); // 24. Hxx with West
  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // 25. HHx with West

  // 26. East has HH+ or a singleton honor
  CoverMemory::EastTop1(2);
  CoverMemory::EastGeneralAnd(1, 1, 1, 1, COVER_EXTEND);

  // 27. West has 2+ cards or singleton honor
  CoverMemory::WestLengthRange(2, 5);
  CoverMemory::WestGeneralAnd(1, 1, 1, 1, COVER_EXTEND);

  // 28. West has the honors, or East has the singleton honor
  CoverMemory::WestTop1(2);
  CoverMemory::EastGeneralAnd(1, 1, 1, 1, COVER_EXTEND);

  // 29. 3=3 or West has both H's.
  CoverMemory::WestLengthRange(2, 3);
  CoverMemory::WestTop1(2, COVER_EXTEND);

  // 30. Hx(x), HH(x) with East; or H singleton with West
  CoverMemory::EastGeneralAnd(2, 3, 1, 2);
  CoverMemory::WestGeneralAnd(1, 1, 1, 1, COVER_EXTEND);
}


void CoverMemory::prepare_5_3()            // ***** DONE-1 *****
{
  coverGlobal = {5, 3};
  CoverMemory::EastLength(0);              // 0. East is void
  CoverMemory::WestLengthRange(1, 4);      // 1. 1=4, 2=3, 3=2 or 4=1
  CoverMemory::WestLengthRange(1, 5);      // 2. West is not void
  CoverMemory::WestLengthRange(2, 3);      // 3. 2=3 or 3=2
  CoverMemory::WestLengthRange(2, 5);      // 4. 2=3, 3=2, 4=1 or 5=0

  CoverMemory::WestLengthRange(0, 4);      // 5. East is not void

  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 6. H with East

  CoverMemory::WestGeneralAnd(4, 4, 3, 3); // 7. HHHx / x

  // 8. 2=3, 3=2 or singleton H either way
  CoverMemory::WestLengthRange(2, 3);
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1, COVER_EXTEND);

  // 9. 2=3, 3=2 or singleton H with West
  CoverMemory::WestLengthRange(2, 3);
  CoverMemory::WestGeneralAnd(1, 1, 1, 1, COVER_EXTEND);

  // 10. West has 0-3c or all honors
  CoverMemory::WestLengthRange(0, 3);
  CoverMemory::WestTop1(3, COVER_EXTEND);

  // 11. East has 2-3c, all honors or a singleton honor
  CoverMemory::EastLengthRange(2, 3);
  CoverMemory::EastTop1(3, COVER_EXTEND);
  CoverMemory::EastGeneralAnd(1, 1, 1, 1, COVER_EXTEND);

  // 12. Effectively, no small singleton on either side
  CoverMemory::WestLength(0);
  CoverMemory::WestLengthRange(2, 3, COVER_EXTEND);
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1, COVER_EXTEND);
}


void CoverMemory::prepare_5_4()            // ***** DONE-1 *****
{
  coverGlobal = {5, 4};
  CoverMemory::WestLengthRange(1, 5);      // 0. East is not void
  CoverMemory::WestLengthRange(2, 3);      // 1. 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);      // 2. 1=4, 2=3, 3=2 or 4=1
}


void CoverMemory::prepare_5_5()            // ***** DONE-1 *****
{
  coverGlobal = {5, 5};
  CoverMemory::WestLengthRange(2, 3);      // 0. 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);      // 1. 1=4, 2=3, 3=2 or 4=1
}


void CoverMemory::prepare_6_1()            // ***** DONE-1 *****
{
  coverGlobal = {6, 1};
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
  CoverMemory::WestGeneralAnd(3, 3, 1, 1); // 13. Hxx with West

  // 14. 3=3 or Hx doubleton on either side
  CoverMemory::WestLength(3);
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1, COVER_EXTEND);

  // 15. 3=3 or West has the short honor (H, Hx)
  CoverMemory::WestLength(3);
  CoverMemory::WestGeneralAnd(1, 2, 1, 1, COVER_EXTEND);

  // 16. East has the honor, or West has the singleton honor
  CoverMemory::EastTop1(1);
  CoverMemory::WestGeneralAnd(1, 1, 1, 1, COVER_EXTEND);

  // 17. West has the top or is void
  CoverMemory::WestTop1(1);
  CoverMemory::WestLength(0, COVER_EXTEND);

  // 18. East has the top or is void
  CoverMemory::EastTop1(1);
  CoverMemory::EastLength(0, COVER_EXTEND);
}


void CoverMemory::prepare_6_2()            // ***** DONE-1 *****
{
  coverGlobal = {6, 2};
  CoverMemory::EastLength(0);              // 0. West is void
  CoverMemory::WestLength(0);              // 1. West is void
  CoverMemory::WestLength(3);              // 2. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 3. 2=4, 3=3 or 4=2

  CoverMemory::WestTop1(2);                // 4. West has both tops
  CoverMemory::WestTop1Range(1, 2);        // 5. West has 1-2 tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 6. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 7. H singleton with East

  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 8. HH doubleton with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 9. HH doubleton with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 10. HH doubleton either side

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 11. H(x) with West

  CoverMemory::SymmGeneralAnd(1, 3, 1, 2); // 12. 3-3 or H, Hx, HH stiff
  CoverMemory::EastGeneralAnd(3, 3, 2, 2); // 13. HHx with East
  CoverMemory::EastGeneralAnd(2, 2, 1, 1); // 14. Hx with East
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // 15. HH(x) with West
  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // 16. HHx with West

  // 17. HH-any or H(x) with West
  CoverMemory::WestTop1(2);
  CoverMemory::WestGeneralAnd(1, 2, 1, 1, COVER_EXTEND);

  // 18. 3=3 or West has both H's.
  CoverMemory::WestLength(3);
  CoverMemory::WestTop1(2, COVER_EXTEND);

  // 19. HH-any or H singleton with East
  CoverMemory::EastGeneralAnd(2, 6, 2, 2);
  CoverMemory::EastGeneralAnd(1, 1, 1, 1, COVER_EXTEND);

  // 20. West has the honors, or East has the singleton honor
  CoverMemory::WestTop1(2);
  CoverMemory::EastGeneralAnd(1, 1, 1, 1, COVER_EXTEND);

  // 21. H singleton or HH doubleton either way, or West is void
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1);
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2, COVER_EXTEND);
  CoverMemory::WestLength(0, COVER_EXTEND);

  // 22. HH+ on the same side; or H singleton with West
  CoverMemory::SymmGeneralAnd(2, 6, 2, 2);
  CoverMemory::WestGeneralAnd(1, 1, 1, 1, COVER_EXTEND);
}


void CoverMemory::prepare_6_3()            // ***** DONE-1 *****
{
  coverGlobal = {6, 3};
  CoverMemory::WestLength(3);              // 0. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 1. 2=4, 3=3 or 4=2

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 2. H singleton either side
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 4. HH doubleton either side
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 5. H, Hx with West

  // 6. 3=3 or West has all the tops
  CoverMemory::WestLength(3);
  CoverMemory::WestTop1(3, COVER_EXTEND);
}


void CoverMemory::prepare_6_4()            // ***** DONE-1 *****
{
  coverGlobal = {6, 4};
  CoverMemory::WestLength(3);              // 0. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 1. 2=4, 3=3 or 4=2

  // 2. 2=4, 3=3, 4=2 or a single honor.
  CoverMemory::WestLengthRange(2, 4);
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1, COVER_EXTEND);
}


void CoverMemory::prepare_6_5()            // ***** DONE-1 *****
{
  coverGlobal = {6, 5};
  CoverMemory::WestLength(3);              // 0. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 1. 2=4, 3=3 or 4=2
}


void CoverMemory::prepare_6_6()            // ***** DONE-1 *****
{
  coverGlobal = {6, 6};
  CoverMemory::WestLength(3);              // 0. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 1. 2=4, 3=3 or 4=2
}


void CoverMemory::prepare_7_1()            // ***** DONE-1 *****
{
  coverGlobal = {7, 1};
  CoverMemory::WestTop1(1);                // 0. West has the top

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 1. H(x) with West

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 2. H either side
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 3. H(x) either side
  CoverMemory::SymmGeneralAnd(1, 3, 1, 1); // 4. H(xx) either side
}


void CoverMemory::prepare_7_2()            // ***** DONE-1 *****
{
  coverGlobal = {7, 2};
  // These three always occur together.
  CoverMemory::WestTop1(2);                // 0. West has the tops
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 1. H(x) with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 2. HH with East

  CoverMemory::SymmGeneralAnd(2, 3, 2, 2); // 3. HH(x) either side
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 4. HH doubleton either side

  // 5. H, Hx; HH, HHx either way
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1);
  CoverMemory::SymmGeneralAnd(2, 3, 2, 2, COVER_EXTEND);

  // 6. Singleton H or doubleton HH either side
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1);
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2, COVER_EXTEND);
}


void CoverMemory::prepare_7_3()            // ***** DONE-1 *****
{
  coverGlobal = {7, 3};
  CoverMemory::SymmGeneralAnd(3, 3, 3, 3); // 0. HHH tripleton either side

  // 1. HH or HHH short either side
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2);
  CoverMemory::SymmGeneralAnd(3, 3, 3, 3, COVER_EXTEND);

  // 2. H, HH or HHH short either side
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1);
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2, COVER_EXTEND);
  CoverMemory::SymmGeneralAnd(3, 3, 3, 3, COVER_EXTEND);
}


void CoverMemory::prepare_8_1()            // ***** DONE-1 *****
{
  coverGlobal = {8, 1};
  CoverMemory::WestTop1(1);                // 0. West has the top
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 1. H singleton either side
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 2. H(x) either side
}


void CoverMemory::prepare_8_2()            // ***** DONE-1 *****
{
  coverGlobal = {8, 2};
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 0. HH doubleton either side

  //  1. Singleton H or doubleton HH either side
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1);
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2, COVER_EXTEND);
}


void CoverMemory::prepare_9_1()            // ***** DONE-1 *****
{
  coverGlobal = {9, 1};
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 0. H singleton either side
}


#define COVER_CHUNK_SIZE 1000
void CoverMemory::makeSets(
  const unsigned char length,
  const Composition& comp)
{
  struct StackInfo
  {
    vector<unsigned char> topsLow;
    vector<unsigned char> topsHigh;

    unsigned char minWest; // Sum of West's top minima
    unsigned char minEast; // Sum of East's top minima
    unsigned char maxDiff; // Large difference max-min for a top
    unsigned char maxWest; // Largest West maximum
    unsigned char maxEast; // Largest East maximum
    unsigned char topNext; // Running top number

    StackInfo(
      const unsigned compSize,
      const unsigned char len)
    {
      topsLow.resize(compSize, 0);
      topsHigh.resize(compSize, len);

      minWest = 0;
      minEast = 0;
      maxDiff = 0;
      maxWest = 0;
      maxEast = 0;
      topNext = 0;
    };
  };

  list<StackInfo> stack; // Unfinished expansions
  stack.emplace_back(StackInfo(comp.size(), length));

  list<CoverSetNew> sets;
  sets.resize(COVER_CHUNK_SIZE);
  for (auto& s: sets)
    s.resize(comp.size());

  auto iter = sets.begin(); // Next one to write

  while (! stack.empty())
  {
    auto stackIter = stack.begin();

    unsigned char topNumber = stackIter->topNext; // Next to write
// cout << "Looking up " << +topNumber << " vs. " << comp.size() << endl;
    if (topNumber >= comp.size())
      break;

    const unsigned char topCountActual = comp.count(topNumber);

// cout << "top number " << +topNumber << ", count " <<
  // +topCountActual << endl;

    for (unsigned char topCountLow = 0; 
        topCountLow <= topCountActual; topCountLow++)
    {
      for (unsigned char topCountHigh = topCountLow; 
        topCountHigh <= topCountActual; topCountHigh++)
      {
        const unsigned char minWest = stackIter->minWest + topCountLow;
        const unsigned char minEast = stackIter->minEast + 
          topCountActual - topCountHigh;

        unsigned char diff = topCountHigh - topCountLow;
        if (diff < stackIter->maxDiff)
          diff = stackIter->maxDiff;

        if (minWest + diff > length)
        {
          // There is no room for this worst-case single maximum,
          // so we skip the entire set, as there will be a more 
          // accurate other set.
          continue;
        }

        if (minEast + diff > length)
          continue;

        if (minWest + minEast > length)
          continue;

        // If there is a top that in itself exceeds the length range,
        // there is a more economical version of this entry.
        if (topCountHigh > stackIter->maxWest)
          stackIter->maxWest = topCountHigh;

        const unsigned char maxEast = topCountActual - topCountLow;
        if (maxEast > stackIter->maxEast)
          stackIter->maxEast = maxEast;

        stackIter->topsLow[topNumber] = topCountLow;
        stackIter->topsHigh[topNumber] = topCountHigh;

// cout << "top number " << +topNumber << ": (" << +topCountLow << ", " << +topCountHigh << ")" << endl;

        // Add the "don't care" with respect to length.
        if (iter == sets.end())
        {
          cout << "CM End reached" << endl;
          assert(false);
        }
// cout << "Adding top without length constraint" << endl;
        iter->set(length, 0, length, 
          comp.getTops(), stackIter->topsLow, stackIter->topsHigh);
// cout << "Added" << endl;
        iter++;

        // Add the possible length constraints.
        const unsigned char lenMax = length - minEast;

// cout << "L  (" << +minWest << ", " << +lenMax << ")" << endl;

        for (unsigned char lenLow = minWest; lenLow <= lenMax; lenLow++)
        {
          for (unsigned char lenHigh = lenLow; 
            lenHigh <= lenMax; lenHigh++)
          { 
            if (lenLow == minWest && lenHigh == lenMax)
              continue;

 // cout << "  C  (" << +lenLow << ", " << +lenHigh << ")" << 
   // ", maxes " << +stackIter->maxWest << ", " << +stackIter->maxEast << "\n";
 // cout << "comp1: " << +stackIter->maxWest << " vs. " << +lenHigh << endl;
 // cout << "comp2: " << +stackIter->maxEast << " vs. " << +(length-lenLow) <<
   "\n";
            if (stackIter->maxWest > lenHigh)
              continue;
            if (stackIter->maxEast > length - lenLow)
              continue;

// cout << "    storing\n";

            if (iter == sets.end())
            {
              cout << "CM End reached" << endl;
              assert(false);
            }
            iter->set(length, lenLow, lenHigh, 
              comp.getTops(), stackIter->topsLow, stackIter->topsHigh);
            iter++;
          }
        }

        stackIter = stack.insert(stackIter, * stackIter);
        auto nextIter = next(stackIter);
        nextIter->minWest = minWest;
        nextIter->minEast = minEast;
        nextIter->maxDiff = diff;
        nextIter->topNext++;
      }
    }
    assert(! stack.empty());
    stack.pop_front();
  }

  assert(! sets.empty());
  /*
  cout << sets.front().strHeader();
  for (auto it = sets.begin(); it != iter; it++)
    cout << it->strLine(length);
  cout << "\n";
  */

// cout << "DONE " << endl;
}


void CoverMemory::prepare([[maybe_unused]] const unsigned char maxCards)
{
  // TODO TMP

  /* */
  Compositions compositions;
cout << "Making compositions" << endl;
  compositions.make(11);
// cout << "Writing strings" << endl;
  // cout << compositions.str();
// cout << "Done" << endl;

  for (unsigned char len = 2; len <= 11; len++)
  {
// cout << "length " << +len << endl;
    for (auto iter = compositions.begin(len); 
        iter != compositions.end(len); iter++)
    {
      if (iter->size() > 2)
        continue;
  // cout << "Starting composition " << iter->strLine() << endl;
      CoverMemory::makeSets(len, * iter);
    }
  }



  // if (maxCards > 0)
    // exit(0);
  /* */

  // TODO TMP

  /* */
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
  /* */
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


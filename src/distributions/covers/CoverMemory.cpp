/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "CoverMemory.h"
#include "ExplStats.h"

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
  const unsigned ssize = specs.size();

  explStats.singles.resize(ssize);
  explStats.lengths.resize(ssize);
  explStats.pairs.resize(ssize);

  for (unsigned s = 0; s < ssize; s++)
  {
    const unsigned s2size = specs[s].size();

    explStats.singles[s].resize(s2size);
    explStats.lengths[s].resize(s2size);
    explStats.pairs[s].resize(s2size);

    for (unsigned t = 0; t < s2size; t++)
    {
      const unsigned csize = specs[s][t].size();

      explStats.singles[s][t].resize(csize, 0);
      explStats.lengths[s][t].resize(20, 0); // 20 explanations per strat

      explStats.pairs[s][t].resize(csize);

      for (unsigned c = 0; c < csize; c++)
        explStats.pairs[s][t][c].resize(csize, 0);
    }
  }
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
  const unsigned specNumber)
{
  // Exactly len

  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.mode[specNumber] = COVER_LENGTHS_ONLY;
  spec.westLength[specNumber].set(len, COVER_EQUAL);
}


void CoverMemory::EastLength(
  const unsigned char len,
  const unsigned specNumber)
{
  // Exactly len
  // TODO CoverMemory::WestLength(coverGlobal.cards - len);
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.mode[specNumber] = COVER_LENGTHS_ONLY;
  spec.westLength[specNumber].set(coverGlobal.cards - len, COVER_EQUAL);
}


void CoverMemory::WestLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.mode[specNumber] = COVER_LENGTHS_ONLY;
  spec.westLength[specNumber].set(len1, len2, COVER_INSIDE_RANGE);
}


void CoverMemory::EastLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.mode[specNumber] = COVER_LENGTHS_ONLY;
  spec.westLength[specNumber].set(
    coverGlobal.cards - len2, 
    coverGlobal.cards - len1, 
    COVER_INSIDE_RANGE);
}


// ----- Top-1 only -----

void CoverMemory::WestTop1(
  const unsigned char len,
  const unsigned specNumber)
{
  // Exactly len
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.mode[specNumber] = COVER_TOPS_ONLY;
  spec.westTop1[specNumber].set(len, COVER_EQUAL);
}


void CoverMemory::EastTop1(
  const unsigned char len,
  const unsigned specNumber)
{
  // Exactly len
  // TODO CoverMemory::WestLength(coverGlobal.cards - len);
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.mode[specNumber] = COVER_TOPS_ONLY;
  spec.westTop1[specNumber].set(coverGlobal.tops1 - len, COVER_EQUAL);
}


void CoverMemory::WestTop1Range(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
  spec.mode[specNumber] = COVER_TOPS_ONLY;
  spec.westTop1[specNumber].set(len1, len2, COVER_INSIDE_RANGE);
}


void CoverMemory::EastTop1Range(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber)
{
  // [len1, len2] inclusive
  CoverSpec& spec = CoverMemory::addOrExtend(specNumber);
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


void CoverMemory::prepare_2_1()            // ***** DONE-1 *****
{
  coverGlobal = {2, 1};
  CoverMemory::WestLength(1);              // 0. 1-1
  CoverMemory::WestLengthRange(1, 2);      // 1. West is not void

  CoverMemory::WestTop1(1);                // 2. West has the top
}


void CoverMemory::prepare_2_2()            // ***** DONE-1 *****
{
  coverGlobal = {2, 2};
  CoverMemory::WestLength(1);              // 0. 1-1
}


void CoverMemory::prepare_3_1()            // ***** DONE-1 *****
{
  coverGlobal = {3, 1};
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void

  CoverMemory::WestLengthRange(0, 2);      // 2. East is not void
  CoverMemory::WestLengthRange(1, 2);      // 3. 1=2 or 2=1
  CoverMemory::WestLengthRange(1, 3);      // 4. West is not void

  CoverMemory::WestTop1(1);                // 5. West has the top
  CoverMemory::EastTop1(1);                // 6. East has the top

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 7. H(x) with West
  // CoverMemory::WestGeneralAnd(1, 3, 1, 1); // 7. H(xx) with West

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 8. Singleton H on either side
}


void CoverMemory::prepare_3_2()            // ***** DONE-1 *****
{
  coverGlobal = {3, 2};
  CoverMemory::WestLengthRange(1, 2);      // 0. 1=2 or 2=1
  CoverMemory::WestLengthRange(1, 3);      // 1. West is not void

  // These two always occur together
  CoverMemory::WestTop1(2);                // 2. West has HH(x)
  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 3. Singleton H with West
}


void CoverMemory::prepare_3_3()            // ***** DONE-1 *****
{
  coverGlobal = {3, 3};
  CoverMemory::WestLengthRange(1, 2);      // 0. 1=2 or 2=1
}


void CoverMemory::prepare_4_1()            // ***** DONE-1 *****
{
  coverGlobal = {4, 1};
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLengthRange(0, 2);      // 2. 0=4, 1=3 or 2=2
  CoverMemory::WestLengthRange(0, 3);      // 3. East is not void
  CoverMemory::WestLengthRange(1, 2);      // 4. 1=3 or 2=2
  CoverMemory::WestLengthRange(1, 4);      // 5. West is not void
  CoverMemory::WestLengthRange(2, 3);      // 6. 2=2 or 3=1
  CoverMemory::WestLengthRange(2, 4);      // 7. 2=2, 3=1 or 4=0

  CoverMemory::WestTop1(1);                // 8. West has the top
  CoverMemory::EastTop1(1);                // 9. East has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 10. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 11. H singleton with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 12. H singleton either way

  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // 13. H(xx) with West
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // 14. H(xx) with East
  CoverMemory::SymmGeneralAnd(1, 3, 1, 1); // 15. H(xx) either side

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 16. H(x) with West
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 17. H(x) on either side

  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // 18. Hx with West
  CoverMemory::EastGeneralAnd(2, 2, 1, 1); // 19. Hx with East
  CoverMemory::SymmGeneralAnd(2, 2, 1, 1); // 20. Hx with East
}


void CoverMemory::prepare_4_2()
{
  coverGlobal = {4, 2};
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLength(2);              // 2. 2=2
  CoverMemory::WestLengthRange(0, 3);      // 3. East is not void
  CoverMemory::WestLengthRange(1, 2);      // 4. 1=3 or 2=2
  CoverMemory::WestLengthRange(1, 3);      // 5. 1=3, 2=2 or 3=1
  CoverMemory::WestLengthRange(1, 4);      // 6. West is not void
  CoverMemory::WestLengthRange(2, 3);      // 7. 2=2 or 3=1
  CoverMemory::WestLengthRange(2, 4);      // 8. 2=2, 3=1 or 4=0

  CoverMemory::WestTop1(2);                // 9. HH onside
  CoverMemory::EastTop1(2);                // 10. HH offside

  CoverMemory::WestGeneralAnd(0, 1, 0, 0); // 11. x or void with West

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 12. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 13. H singleton with East

  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 14. HH doubleton with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 15. HH doubleton with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 16. HH doubleton either side

  CoverMemory::WestGeneralAnd(2, 3, 1, 1); // 17. Hx(x) with West
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // 18. Hx(x) with East

  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // 19. HHx with West
  CoverMemory::EastGeneralAnd(3, 3, 2, 2); // 20. HHx with East

  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // 21. H(xx) with West
  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // 22. HH(x) with West
  CoverMemory::WestGeneralAnd(3, 4, 2, 2); // 23. HHx(x) with West

  CoverMemory::SymmGeneralAnd(0, 0, 0, 0); // 24. Either side is void
}


void CoverMemory::prepare_4_3()            // ***** DONE-1 *****
{
  coverGlobal = {4, 3};
  CoverMemory::WestLength(2);              // 0. 2=2
  CoverMemory::WestLengthRange(1, 3);      // 1. 1=3, 2=2 or 3=1
  CoverMemory::WestLengthRange(1, 4);      // 2. West is not void

  // #4 can occur independently, and the rest occur together.
  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 3. H singleton with West
  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 4. HH doubleton with West
  CoverMemory::WestTop1(3);                // 5. HHH(x) onside
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
  CoverMemory::WestLengthRange(0, 4);      // 2. East is not void
  CoverMemory::WestLengthRange(1, 4);      // 3. 1=4, 2=3, 3=2 or 4=1

  CoverMemory::WestTop1(1);                // 4. West has the top
  CoverMemory::EastTop1(1);                // 5. East has the top

  CoverMemory::SymmGeneralAnd(0, 0, 0, 0); // 6. Either side is void

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 7. H with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 8. H with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 9. H singleton either side

  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // 10. Hx doubleton with West
  CoverMemory::SymmGeneralAnd(2, 2, 1, 1); // 11. Hx doubleton either side

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 12. H(x) doubleton with West
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // 13. H(x) doubleton with East
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 14. H(x) either way

  CoverMemory::WestGeneralAnd(1, 3, 1, 1); // 15. H(xx) with West
  CoverMemory::EastGeneralAnd(1, 3, 1, 1); // 16. H(xx) with East
  CoverMemory::SymmGeneralAnd(1, 3, 1, 1); // 17. H(xx) either side

  CoverMemory::WestGeneralAnd(2, 3, 1, 1); // 18. Hx(x) with West
  CoverMemory::SymmGeneralAnd(2, 3, 1, 1); // 19. Hx(x) either side

  CoverMemory::WestGeneralAnd(1, 4, 1, 1); // 20. H(xxx) with West

  CoverMemory::EastGeneralAnd(1, 1, 0, 0); // 21. x with East
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // 22. Hx(x) with East
  CoverMemory::EastGeneralAnd(4, 5, 1, 1); // 23. Hxxx(x) with East

}


void CoverMemory::prepare_5_2()
{
  coverGlobal = {5, 2};
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::EastLength(1);              // 2. 4=1
  CoverMemory::WestLengthRange(0, 1);      // 3. 0=5 or 1=4
  CoverMemory::WestLengthRange(0, 3);      // 4. 0=5, 1=4, 2=3 or 3=2
  CoverMemory::WestLengthRange(1, 4);      // 5. 1=4, 2=3, 3=2 or 4=1
  CoverMemory::WestLengthRange(2, 3);      // 6. 2=3 or 3=2
  CoverMemory::WestLengthRange(2, 5);      // 7. 2=3, 3=2, 4=1 or 5=0

  CoverMemory::WestTop1(2);                // 8. West has the tops
  CoverMemory::EastTop1(2);                // 9. East has the tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 10. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 11. H singleton with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 12. H singleton either way

  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 13. HH doubleton with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 14. HH doubleton with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 15. HH doubleton either way

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 16. H(x) with West
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // 17. H(x) with East
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 18. H(x) with East

  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // 19. HH(x) with West
  CoverMemory::SymmGeneralAnd(2, 3, 2, 2); // 20. HH(x) with West

  CoverMemory::WestGeneralAnd(2, 2, 1, 1); // 21. Hx with West

  CoverMemory::WestGeneralAnd(4, 5, 2, 2); // 22. HHxx(x) with West

  CoverMemory::WestGeneralAnd(1, 1, 0, 0); // 23. x with East
  CoverMemory::EastGeneralAnd(1, 1, 0, 0); // 24. x with East
  CoverMemory::EastGeneralAnd(2, 3, 1, 1); // 25. Hx(x) with East
  CoverMemory::EastGeneralAnd(2, 3, 1, 2); // 26. Hx(x), HH(x) with East
  CoverMemory::EastGeneralAnd(2, 4, 1, 1); // 27. Hx(xx) with East

  CoverMemory::SymmGeneralAnd(2, 5, 2, 2); // 28. HH-any on the same side
}


void CoverMemory::prepare_5_3()            // ***** DONE-1 *****
{
  coverGlobal = {5, 3};
  CoverMemory::WestLength(0);              // 0. West is void
  CoverMemory::EastLength(0);              // 1. East is void
  CoverMemory::WestLengthRange(0, 4);      // 2. East is not void
  CoverMemory::WestLengthRange(1, 4);      // 3. 1=4, 2=3, 3=2 or 4=1
  CoverMemory::WestLengthRange(1, 5);      // 4. West is not void
  CoverMemory::WestLengthRange(2, 3);      // 5. 2=3 or 3=2
  CoverMemory::WestLengthRange(2, 5);      // 6. 2=3, 3=2, 4=1 or 5=0

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 7. H with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 8. H with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 9. H singleton either way

  CoverMemory::EastGeneralAnd(1, 1, 0, 0); // 10. x singleton with East

  // 11. West has 0-3c or all honors
  CoverMemory::WestLengthRange(0, 3);
  CoverMemory::WestTop1(3, 1);

  // 12. West has 2-3c or all honors
  CoverMemory::WestLengthRange(2, 3);
  CoverMemory::WestTop1(3, 1);

  // 13. East has 0-3c or all honors
  CoverMemory::EastLengthRange(0, 3);
  CoverMemory::EastTop1(3, 1);

  // 14. East has 2-3c or all honors
  CoverMemory::EastLengthRange(2, 3);
  CoverMemory::EastTop1(3, 1);
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


void CoverMemory::prepare_6_splitsNonVoid()
{
  CoverMemory::WestLength(3);              // 0. 3=3
  CoverMemory::WestLengthRange(2, 4);      // 1. 2=4, 3=3 or 4=2
  CoverMemory::WestLengthRange(1, 5);      // 2. 1=5, 2=4, 3=3, 4=2 or 5=1
}


void CoverMemory::prepare_6_1()            // ***** DONE-1 *****
{
  coverGlobal = {6, 1};
  CoverMemory::prepare_6_splitsNonVoid();  // 0-2.
  CoverMemory::WestLength(0);              // 3. West is void
  CoverMemory::EastLength(0);              // 4. East is void

  CoverMemory::WestTop1(1);                // 5. West has the top
  CoverMemory::EastTop1(1);                // 6. East has the top

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 7. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 8. H singleton with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 9. H singleton either way

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 10. H(x) with West
  CoverMemory::SymmGeneralAnd(1, 2, 1, 1); // 11. H(x) on either side

  CoverMemory::WestGeneralAnd(3, 3, 1, 1); // 12. Hxx with West
  CoverMemory::EastGeneralAnd(2, 2, 1, 1); // 13. Hx with East
}


void CoverMemory::prepare_6_2()            // ***** DONE-1 *****
{
  coverGlobal = {6, 2};
  CoverMemory::prepare_6_splitsNonVoid();  // 0-2.
  CoverMemory::WestLength(0);              // 3. West is void
  CoverMemory::EastLength(0);              // 4. West is void
  CoverMemory::WestTop1(2);                // 5. West has both tops
  CoverMemory::WestTop1Range(1, 2);        // 6. West has 1-2 tops

  CoverMemory::WestGeneralAnd(1, 1, 1, 1); // 7. H singleton with West
  CoverMemory::EastGeneralAnd(1, 1, 1, 1); // 8. H singleton with East
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 9. H singleton either way

  CoverMemory::WestGeneralAnd(2, 2, 2, 2); // 10. HH doubleton with West
  CoverMemory::EastGeneralAnd(2, 2, 2, 2); // 11. HH doubleton with East
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 12. HH doubleton either side

  CoverMemory::WestGeneralAnd(3, 3, 2, 2); // 13. HHx with West

  CoverMemory::EastGeneralAnd(3, 3, 2, 2); // 14. HHx with East

  CoverMemory::WestGeneralAnd(2, 3, 2, 2); // 15. HH(x) with West

  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 16. H(x) with West
  CoverMemory::EastGeneralAnd(1, 2, 1, 1); // 17. H(x) with East

  CoverMemory::SymmGeneralAnd(1, 3, 1, 2); // 18. 3-3 or H, HH stiff

  CoverMemory::EastGeneralAnd(2, 6, 2, 2); // 19. HH-any with East
  CoverMemory::SymmGeneralAnd(2, 6, 2, 2); // 20. HH-any on the same side

  // 21. 3=3 or West has both H's.
  CoverMemory::WestLength(3);
  CoverMemory::WestTop1(2, 1);
}


void CoverMemory::prepare_6_3()            // ***** DONE-1 *****
{
  coverGlobal = {6, 3};
  CoverMemory::prepare_6_splitsNonVoid();  // 0-2.

  CoverMemory::SymmGeneralAnd(2, 3, 2, 3); // 3. Eff. 3=3 or doubleton HH
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 4. H singleton either side

  // These last three always occur together.
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 5. HH doubleton either side
  CoverMemory::WestGeneralAnd(1, 2, 1, 1); // 6. H, Hx with West

  CoverMemory::WestLength(3);              // 7. West has HHH+ or any 3=3
  CoverMemory::WestTop1(3, 1);
}


void CoverMemory::prepare_6_4()            // ***** DONE-1 *****
{
  coverGlobal = {6, 4};
  CoverMemory::prepare_6_splitsNonVoid();  // 0-2.

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 3. H singleton either side
}


void CoverMemory::prepare_6_5()            // ***** DONE-1 *****
{
  coverGlobal = {6, 5};
  CoverMemory::prepare_6_splitsNonVoid();  // 0-2.
}


void CoverMemory::prepare_6_6()            // ***** DONE-1 *****
{
  coverGlobal = {6, 6};
  CoverMemory::prepare_6_splitsNonVoid();  // 0-2.
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

  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 3. H singleton either side
  CoverMemory::SymmGeneralAnd(1, 2, 1, 2); // 4. H(x), H(H) either side
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 5. HH doubleton either side
  CoverMemory::SymmGeneralAnd(2, 3, 2, 2); // 6. HH(x) either side
}


void CoverMemory::prepare_7_3()            // ***** DONE-1 *****
{
  coverGlobal = {7, 3};
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 0. H singleton either side
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 1. HH doubleton either side
  CoverMemory::SymmGeneralAnd(3, 3, 3, 3); // 2. HHH tripleton either side
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
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 0. H singleton either side
  CoverMemory::SymmGeneralAnd(2, 2, 2, 2); // 1. HH doubleton either side
}


void CoverMemory::prepare_9_1()            // ***** DONE-1 *****
{
  coverGlobal = {9, 1};
  CoverMemory::SymmGeneralAnd(1, 1, 1, 1); // 0. H singleton either side
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


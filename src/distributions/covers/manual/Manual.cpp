/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

/*
   These cover rows are defined by hand, although they are by now
   heavily inspired by the algorithmic results.  They do work, and
   they offer the same or similar results.

   % perl extract.pl outputfile

   yields the differences.

   The way to turn the manual results on again is:
   - In DistCore::prepareCovers, call prepareManualCovers.
   - This puts the cover rows in rows in Covers.cpp, so these
     must be turned on in Cover.h.
   - In Slist, call coversManual.
      
 */

#include <cassert>

#include "Manual.h"


struct DistData
{
  list<list<ManualData>> * dataPtr;
  Profile const * sumProfilePtr;
  unsigned char maxLength;
  unsigned char maxTops;
  unsigned char numTops;
};


// ----- Length only -----

void Manual::WestLength(
  DistData& distData,
  const unsigned char len) const
{
  Manual::WestGeneralAnd(distData, len, len, 0, distData.maxTops);
}


void Manual::EastLength(
  DistData& distData,
  const unsigned char len) const
{
  Manual::WestLength(distData, distData.maxLength - len);
}


void Manual::WestLengthRange(
  DistData& distData,
  const unsigned char len1,
  const unsigned char len2) const
{
  Manual::WestGeneralAnd(distData, len1, len2, 0, distData.maxTops);
}


void Manual::EastLengthRange(
  DistData& distData,
  const unsigned char len1,
  const unsigned char len2) const
{
  Manual::WestGeneralAnd(
    distData,
    distData.maxLength - len2, 
    distData.maxLength - len1, 
    0, 
    distData.maxTops);
}


// ----- Top-1 only -----

void Manual::WestTop1(
  DistData& distData,
  const unsigned char len) const
{
  Manual::WestGeneralAnd(distData, 0, distData.maxLength, len, len);
}


void Manual::EastTop1(
  DistData& distData,
  const unsigned char len) const
{
  Manual::WestTop1(distData, distData.maxTops - len);
}


void Manual::WestTop1Range(
  DistData& distData,
  const unsigned char len1,
  const unsigned char len2) const
{
  Manual::WestGeneralAnd(distData, 0, distData.maxLength, len1, len2);
}


void Manual::EastTop1Range(
  DistData& distData,
  const unsigned char len1,
  const unsigned char len2) const
{
  Manual::WestGeneralAnd(
    distData,
    0,
    distData.maxLength,
    distData.maxTops - len2,
    distData.maxTops - len1);
}


// ----- Length AND top-1 -----

void Manual::WestGeneralAnd(
  DistData& distData,
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl) const
{
  distData.dataPtr->emplace_back(list<ManualData>());
  auto& mlist = distData.dataPtr->back();

  mlist.emplace_back(ManualData(* distData.sumProfilePtr));
  auto& mdata = mlist.back();

  const unsigned char highestTop =
    static_cast<unsigned char>(distData.numTops-1);

  mdata.profilePair.setLength(lowerCardsIncl, upperCardsIncl);
  mdata.profilePair.setTop(highestTop, lowerTopsIncl, upperTopsIncl);
}


void Manual::EastGeneralAnd(
  DistData& distData,
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl) const
{
  Manual::WestGeneralAnd(
    distData,
    distData.maxLength - upperCardsIncl,
    distData.maxLength - lowerCardsIncl,
    distData.maxTops - upperTopsIncl,
    distData.maxTops - lowerTopsIncl);
}


void Manual::SymmGeneralAnd(
  DistData& distData,
  const unsigned char lowerCardsIncl,
  const unsigned char upperCardsIncl,
  const unsigned char lowerTopsIncl,
  const unsigned char upperTopsIncl) const
{
  Manual::WestGeneralAnd(distData, lowerCardsIncl, upperCardsIncl,
    lowerTopsIncl, upperTopsIncl);

  distData.dataPtr->back().back().symmFlag = true;
}


void Manual::WestGeneralTwo(
  DistData& distData,
  const unsigned char lowerCardsIncl1,
  const unsigned char upperCardsIncl1,
  const unsigned char lowerTopsIncl1,
  const unsigned char upperTopsIncl1,
  const unsigned char lowerCardsIncl2,
  const unsigned char upperCardsIncl2,
  const unsigned char lowerTopsIncl2,
  const unsigned char upperTopsIncl2) const
{
  distData.dataPtr->emplace_back(list<ManualData>());
  auto& mlist = distData.dataPtr->back();

  mlist.emplace_back(ManualData(* distData.sumProfilePtr));
  auto& mdata1 = mlist.back();

  const unsigned char highestTop =
    static_cast<unsigned char>(distData.numTops-1);

  mdata1.profilePair.setLength(lowerCardsIncl1, upperCardsIncl1);
  mdata1.profilePair.setTop(highestTop, lowerTopsIncl1, upperTopsIncl1);

  mlist.emplace_back(ManualData(* distData.sumProfilePtr));
  auto& mdata2 = mlist.back();

  mdata2.profilePair.setLength(lowerCardsIncl2, upperCardsIncl2);
  mdata2.profilePair.setTop(highestTop, lowerTopsIncl2, upperTopsIncl2);
}


// ----- Specific, hand-chosen data -----

void Manual::prepare_2_1(DistData& distData) const
{
  Manual::WestLength(distData, 1);              // 0. 1-1
  Manual::WestLengthRange(distData, 1, 2);      // 1. West is not void
  Manual::WestTop1(distData, 1);                // 2. West has the top
}


void Manual::prepare_2_2(DistData& distData) const
{
  Manual::WestLength(distData, 1);              // 0. 1-1
}


void Manual::prepare_3_1(DistData& distData) const
{
  Manual::WestLengthRange(distData, 1, 3);      // 0. West is not void
  Manual::WestLengthRange(distData, 0, 2);      // 1. East is not void
  Manual::WestLengthRange(distData, 1, 2);      // 2. 1=2 or 2=1
  Manual::WestLength(distData, 0);              // 3. West is void
  Manual::EastLength(distData, 0);              // 4. East is void

  Manual::WestTop1(distData, 1);                // 5. West has the top
  Manual::EastTop1(distData, 1);                // 6. East has the top

  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 7. Singleton H 
  Manual::WestGeneralAnd(distData, 0, 2, 1, 1); // 8. H(x) with West
}


void Manual::prepare_3_2(DistData& distData) const
{
  Manual::WestLengthRange(distData, 1, 3);      // 0. West is not void
  Manual::WestLengthRange(distData, 1, 2);      // 1. 1=2 or 2=1

  Manual::WestTop1(distData, 2);                // 2. West has both tops

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 3. H singleton West
}


void Manual::prepare_3_3(DistData& distData) const
{
  Manual::WestLengthRange(distData, 1, 2);      // 0. 1=2 or 2=1
}


void Manual::prepare_4_1(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::WestLength(distData, 2);              // 2. 2=2
  Manual::WestLengthRange(distData, 0, 2);      // 3. 0=4, 1=3, 2=2
  Manual::WestLengthRange(distData, 2, 4);      // 4. 2=2, 3=1, 4=0
  Manual::WestLengthRange(distData, 0, 3);      // 5. East is not void
  Manual::WestLengthRange(distData, 1, 3);      // 6. Nobody is void
  Manual::WestLengthRange(distData, 1, 4);      // 7. West is not void

  Manual::WestTop1(distData, 1);                // 8. West has the top
  Manual::EastTop1(distData, 1);                // 9. East has the top

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 10. H singleton West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 11. H singleton East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 12. H singleton

  Manual::WestGeneralAnd(distData, 2, 2, 1, 1); // 13. Hx with West

  Manual::WestGeneralAnd(distData, 0, 2, 1, 1); // 14. H(x) with West

  Manual::WestGeneralAnd(distData, 0, 3, 1, 1); // 15. H(xx) with West
  Manual::EastGeneralAnd(distData, 0, 3, 1, 1); // 16. H(xx) with East

  Manual::EastGeneralAnd(distData, 0, 2, 1, 1); // 17. H(x) with East
  Manual::WestLength(distData, 1);              // 18. West has singleton
  Manual::WestLength(distData, 3);              // 19. East has singleton

  // 20. West has the top, or the suit splits 2=2.
  Manual::WestGeneralTwo(distData, 0, 4, 1, 1,    2, 2, 0, 1);

  // 21. West has at most a doubleton, or East has the top.
  Manual::WestGeneralTwo(distData, 0, 2, 0, 1,    0, 4, 0, 0);

  // 22. East has the top, or West has a singleton.
  Manual::WestGeneralTwo(distData, 0, 4, 0, 0,    1, 1, 0, 1);

  // 23. West has the top, or East has a singleton.
  Manual::WestGeneralTwo(distData, 0, 4, 1, 1,    3, 3, 0, 1);

  // 24. East has at most a doubleton, or West has the top.
  Manual::WestGeneralTwo(distData, 2, 4, 0, 1,    0, 4, 1, 1);

  // 25. East has the top, or the suit splits 2=2.
  Manual::WestGeneralTwo(distData, 0, 4, 0, 0,    2, 2, 0, 1);
}


void Manual::prepare_4_2(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::EastLength(distData, 1);              // 2. East has singleton
  Manual::WestLength(distData, 2);              // 3. 2=2

  Manual::WestLengthRange(distData, 0, 2);      // 4. 0=4, 1=3, 2=2
  Manual::WestLengthRange(distData, 0, 3);      // 5. East is not void
  Manual::WestLengthRange(distData, 1, 2);      // 6. 1=3 or 2=2
  Manual::WestLengthRange(distData, 1, 3);      // 7. 1=3, 2=2 or 3=1
  Manual::WestLengthRange(distData, 1, 4);      // 8. West is not void
  Manual::WestLengthRange(distData, 2, 3);      // 9. 2=2 or 3=1
  Manual::WestLengthRange(distData, 2, 4);      // 10. 2=2, 3=1 or 4=0

  Manual::WestTop1(distData, 2);                // 11. HH-any West
  Manual::WestTop1Range(distData, 1, 2);        // 12. H or HH any West
  Manual::EastTop1(distData, 2);                // 13. HH any East
  Manual::WestTop1(distData, 1);                // 14. Ex. 1 H with West

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 15. H singleton West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 16. H singleton East

  Manual::WestGeneralAnd(distData, 2, 2, 2, 2); // 17. HH doubleton West
  Manual::EastGeneralAnd(distData, 2, 2, 2, 2); // 18. HH doubleton East
  Manual::SymmGeneralAnd(distData, 2, 2, 0, 0); // 19. HH doubleton

  Manual::WestGeneralAnd(distData, 1, 1, 0, 0); // 20. x singleton West
  Manual::WestGeneralAnd(distData, 2, 2, 0, 1); // 21. xx, Hx West
  Manual::WestGeneralAnd(distData, 2, 2, 1, 2); // 22. Hx, HH West

  Manual::WestGeneralAnd(distData, 3, 3, 2, 2); // 23. HHx with West
  Manual::EastGeneralAnd(distData, 3, 3, 2, 2); // 24. HHx with East

  Manual::WestGeneralAnd(distData, 0, 3, 1, 2); // 25. No void; West 1+ H
  Manual::WestGeneralAnd(distData, 2, 4, 1, 2); // 26. West 2-4c with 1+ H

  // 27. The suit splits 2=2, or West has both tops.
  Manual::WestGeneralTwo(distData, 2, 2, 0, 2,     0, 4, 2, 2);

  // 28. The suit splits 2=2, or East has both tops.
  Manual::WestGeneralTwo(distData, 2, 2, 0, 2,     0, 4, 0, 0);

  // 29. West has at most a doubleton, or West has both tops.
  Manual::WestGeneralTwo(distData, 0, 2, 0, 2,     0, 4, 2, 2);

  // 30. East has at most a doubleton, or East has both tops.
  Manual::WestGeneralTwo(distData, 2, 4, 0, 2,     0, 4, 0, 0);

  // 31. East has at most a doubleton, or West has exactly one top.
  Manual::WestGeneralTwo(distData, 2, 4, 0, 2,     0, 4, 1, 1);

  // 32. West has exactly one top, or the suit splits 2=2.
  Manual::WestGeneralTwo(distData, 0, 4, 1, 1,     2, 2, 0, 2);

  // 33. 1=3 or 2=2, or West has both tops.
  Manual::WestGeneralTwo(distData, 1, 2, 0, 2,     0, 4, 2, 2);

  // 34. 1=3 or 2=2, or West has exactly one top.
  Manual::WestGeneralTwo(distData, 1, 2, 0, 2,     0, 4, 1, 1);

  // 35. 2=2 or 3=1, or West has exactly one top.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 2,     0, 4, 1, 1);

  // 36. 2=2 or 3=1, or West has both tops.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 2,     0, 4, 2, 2);

  // 37. 0-2 cards, or exactly one top.
  Manual::WestGeneralTwo(distData, 0, 2, 0, 2,     0, 4, 1, 1);

  // 38. 2-4 cards, or exactly one top.
  Manual::WestGeneralTwo(distData, 2, 4, 0, 2,     0, 4, 1, 1);

  // 39. 1-2 cards, or exactly one top.
  Manual::WestGeneralTwo(distData, 1, 2, 0, 2,     0, 4, 1, 1);

  // 40. 2-3 cards, or East has both tops.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 2,     0, 4, 0, 0);
}


void Manual::prepare_4_3(DistData& distData) const
{
  Manual::WestLength(distData, 2);              // 0. 2=2
  Manual::WestLengthRange(distData, 1, 2);      // 1. 1=3 or 2=2
  Manual::WestLengthRange(distData, 1, 3);      // 1. 1=3, 2=2 or 3=1
  Manual::WestLengthRange(distData, 1, 4);      // 2. West is not void

  Manual::WestTop1(distData, 3);                // 3. West has all tops
  Manual::WestTop1Range(distData, 2, 3);        // 3. West has 2+ tops

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 4. H singleton West
  Manual::WestGeneralAnd(distData, 2, 2, 2, 2); // 5. HH doubleton West
}


void Manual::prepare_4_4(DistData& distData) const
{
  Manual::WestLength(distData, 2);              // 0. 2=2
  Manual::WestLengthRange(distData, 1, 3);      // 1. 1=3, 2=2 or 3=1
  Manual::WestLengthRange(distData, 1, 2);      // 2. 1=3 or 2=2
  Manual::WestLengthRange(distData, 2, 3);      // 3. 2=2 or 3=1
}


void Manual::prepare_5_1(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::WestLengthRange(distData, 0, 4);      // 2. East is not void
  Manual::WestLengthRange(distData, 1, 4);      // 3. 1=4, 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 2, 3);      // 4. 3-2 either way

  Manual::WestLength(distData, 1);              // 5. 1=4
  Manual::WestLengthRange(distData, 1, 3);      // 6. 1=4, 2=3 or 3=2
  Manual::WestLength(distData, 2);              // 7. 2=3
  Manual::WestLengthRange(distData, 2, 4);      // 8. 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 2, 5);      // 9. 2=3, 3=2, 4=1, 5=0
  Manual::WestLength(distData, 3);              // 10. 3=2
  Manual::WestLengthRange(distData, 0, 3);      // 11. 0=5, 1=4, 2=3, 3=2
  Manual::WestLength(distData, 4);              // 12. 4=1

  Manual::WestTop1(distData, 1);                // 13. West has the top
  Manual::EastTop1(distData, 1);                // 14. East has the top

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 15. H with West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 16. H with East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 17. H singleton 

  Manual::WestGeneralAnd(distData, 2, 2, 1, 1); // 18. Hx doubleton West

  Manual::WestGeneralAnd(distData, 2, 3, 1, 1); // 19. Hx(x) with West
  Manual::EastGeneralAnd(distData, 2, 3, 1, 1); // 20. Hx(x) with East

  Manual::WestGeneralAnd(distData, 0, 2, 1, 1); // 21. H(x) with West
  Manual::EastGeneralAnd(distData, 0, 2, 1, 1); // 22. H(x) with East
  Manual::SymmGeneralAnd(distData, 0, 2, 1, 1); // 23. H(x) either way

  Manual::WestGeneralAnd(distData, 0, 3, 1, 1); // 24. H(xx) with West
  Manual::EastGeneralAnd(distData, 0, 3, 1, 1); // 25. H(xx) with East

  Manual::WestGeneralAnd(distData, 0, 4, 1, 1); // 26. H(xxx) with West

  // 27. West has the top, or East has a singleton.
  Manual::WestGeneralTwo(distData, 0, 5, 1, 1,     4, 4, 0, 1);

  // 28. East has the top, or West has a singleton.
  Manual::WestGeneralTwo(distData, 0, 5, 0, 0,     1, 1, 0, 1);

  // 29. East has at most 3 cards, or West has the top.
  Manual::WestGeneralTwo(distData, 2, 5, 0, 1,     0, 5, 1, 1);

  // 30. West has at most 3 cards, or East has the top.
  Manual::WestGeneralTwo(distData, 0, 3, 0, 1,     0, 5, 0, 0);

  // 31. The suit splits 2-3 either way, or East has the top.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 1,     0, 5, 0, 0);
}


void Manual::prepare_5_2(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::WestLength(distData, 1);              // 2. 1=4
  Manual::WestLength(distData, 2);              // 3. 2=3
  Manual::WestLength(distData, 3);              // 4. 3=2
  Manual::WestLength(distData, 4);              // 5. 4=1

  Manual::WestLengthRange(distData, 0, 2);      // 6. 0=5, 1=4 or 2=3
  Manual::WestLengthRange(distData, 0, 3);      // 7. 0=5, 1=4, 2=3 or 3=2
  Manual::WestLengthRange(distData, 0, 4);      // 8. East is not void

  Manual::WestLengthRange(distData, 1, 3);      // 9. 1=4, 2=3 or 3=2
  Manual::WestLengthRange(distData, 1, 4);      // 10. 1=4, 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 1, 5);      // 11. West is not void
  Manual::WestLengthRange(distData, 2, 3);      // 12. 2=3 or 3=2
  Manual::WestLengthRange(distData, 2, 4);      // 13. 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 2, 5);      // 14. 2=3, 3=2, 4=1 or 5=0

  Manual::WestTop1(distData, 1);                // 15. West has ex. 1 top
  Manual::WestTop1(distData, 2);                // 16. West has the tops
  Manual::WestTop1Range(distData, 1, 2);        // 17. West has 1+ tops
  Manual::EastTop1(distData, 2);                // 18. West has the tops

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 19. H singleton with West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 20. H singleton with East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 21. H singleton

  Manual::WestGeneralAnd(distData, 2, 2, 2, 2); // 22. HH doubleton West
  Manual::EastGeneralAnd(distData, 2, 2, 2, 2); // 23. HH doubleton East
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 24. HH doubleton 

  Manual::WestGeneralAnd(distData, 2, 2, 1, 1); // 25. Hx / Hxx
  Manual::WestGeneralAnd(distData, 3, 3, 2, 2); // 26. HHx / xx

  Manual::WestGeneralAnd(distData, 0, 2, 1, 1); // 27. H(x) with West
  Manual::EastGeneralAnd(distData, 0, 2, 1, 1); // 28. H(x) with East

  Manual::WestGeneralAnd(distData, 0, 2, 1, 2); // 29. H, Hx, HH West
  Manual::EastGeneralAnd(distData, 0, 2, 1, 2); // 30. H, Hx, HH East
  Manual::WestGeneralAnd(distData, 2, 2, 1, 2); // 31. Hx, HH West

  Manual::WestGeneralAnd(distData, 0, 3, 1, 1); // 32. H(xx) with West
  Manual::EastGeneralAnd(distData, 0, 3, 1, 2); // 33. H(xx), HH(x) West
  Manual::WestGeneralAnd(distData, 0, 3, 2, 2); // 34. HH(x) West
  Manual::EastGeneralAnd(distData, 0, 3, 2, 2); // 35. HH(x) East

  Manual::WestGeneralAnd(distData, 1, 5, 0, 0); // 36. x(xx) West
  Manual::EastGeneralAnd(distData, 0, 4, 2, 2); // 37. x(xx) East

  Manual::WestGeneralAnd(distData, 2, 3, 1, 2); // 38. Hx(x) with West
  Manual::EastGeneralAnd(distData, 2, 3, 1, 2); // 39. Hx(x) with East

  Manual::WestGeneralAnd(distData, 0, 3, 1, 2); // 40.
  Manual::EastGeneralAnd(distData, 0, 3, 1, 2); // 41. 
  Manual::EastGeneralAnd(distData, 0, 3, 1, 1); // 42. 
  Manual::EastGeneralAnd(distData, 1, 3, 0, 1); // 43. 
  Manual::EastGeneralAnd(distData, 0, 3, 0, 1); // 44. 

  // 45. 3-2 either way, or West has both H's.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 2,     0, 5, 2, 2);

  // 46. 3-2 either way, or East has both H's.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 2,     0, 5, 0, 0);

  // 47. East has a doubleton, or East has x(xx).
  Manual::WestGeneralTwo(distData, 3, 3, 0, 2,     0, 4, 2, 2);

  // 48. 3-2 either way, or West has exactly 1 top.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 2,     0, 5, 1, 1);

  // 49. West has both tops, or East has a singleton.
  Manual::WestGeneralTwo(distData, 0, 5, 2, 2,     4, 4, 0, 2);

  // 50. East has a doubleton, or West has both tops.
  Manual::WestGeneralTwo(distData, 3, 3, 0, 2,     0, 5, 2, 2);

  // 51. West has a doubleton, or East has both tops.
  Manual::WestGeneralTwo(distData, 2, 2, 0, 2,     0, 5, 0, 0);

  // 52. East has at most 3 cards, or West has exactly 1 top.
  Manual::WestGeneralTwo(distData, 2, 5, 0, 2,     0, 5, 1, 1);

  // 53. East has at most 3 cards, or East has both tops.
  Manual::WestGeneralTwo(distData, 2, 5, 0, 2,     0, 5, 0, 0);

  // 54. West has at most 3 cards, or West has exactly 1 top.
  Manual::WestGeneralTwo(distData, 0, 3, 0, 2,     0, 5, 1, 1);

  // 55. 2=3 to 4=1, West has exactly 1 top.
  Manual::WestGeneralTwo(distData, 2, 4, 0, 2,     0, 5, 1, 1);
}


void Manual::prepare_5_3(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::SymmGeneralAnd(distData, 0, 0, 0, 3); // 2. Either side void
  Manual::WestLength(distData, 2);              // 3. 2=3
  Manual::WestLength(distData, 3);              // 4. 3=2

  Manual::WestLengthRange(distData, 0, 3);      // 5. East is not void
  Manual::WestLengthRange(distData, 1, 3);      // 6. 1=4, 2=3, or 3=2
  Manual::WestLengthRange(distData, 1, 4);      // 7. 1=4, 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 1, 5);      // 8. West is not void
  Manual::WestLengthRange(distData, 2, 3);      // 9. 2=3 or 3=2
  Manual::WestLengthRange(distData, 2, 4);      // 10. 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 2, 5);      // 11. 2=3, 3=2, 4=1 or 5=0

  Manual::WestTop1(distData, 0);                // 12. West has no tops
  Manual::WestTop1(distData, 1);                // 13. West has 1 top
  Manual::WestTop1(distData, 2);                // 14. West has 2 tops
  Manual::WestTop1(distData, 3);                // 15. West has 3 tops

  Manual::WestGeneralAnd(distData, 1, 1, 0, 0); // 16. x / HHHx
  Manual::EastGeneralAnd(distData, 1, 1, 0, 0); // 17. x / HHHx
  Manual::SymmGeneralAnd(distData, 1, 1, 0, 0); // 18. Either way

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 19. Stiff H West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 20. Stiff H East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 21. Stiff H

  // 22. West has at most 3 cards, or West has all tops.
  Manual::WestGeneralTwo(distData, 0, 3, 0, 3,    0, 5, 3, 3);

  // 23. East has at most 3 cards, or East has all tops.
  Manual::WestGeneralTwo(distData, 2, 5, 0, 3,    0, 5, 0, 0);

  // 24. East has at most 3 cards, or West has exactly one top.
  Manual::WestGeneralTwo(distData, 2, 5, 0, 3,    0, 5, 1, 1);

  // 25. The suit splits 2-3 either way, or West has all the tops.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 3,    0, 5, 3, 3);

  // 26. The suit splits 2-3 either way, or East has all the tops.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 3,    0, 5, 0, 0);

  // 27. The suit splits 2-3 either way, or West has exactly one top.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 3,    0, 5, 1, 1);

  // 28. The suit splits 2-3 either way, or East has exactly one top.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 3,    0, 5, 2, 2);

  // 29. West has 1-2 tops, or 2-3 either way.
  Manual::WestGeneralTwo(distData, 0, 5, 1, 2,    2, 3, 0, 3);

  // 30. West has 1-3 cards, or West has all tops.
  Manual::WestGeneralTwo(distData, 1, 3, 0, 3,    0, 5, 3, 3);

  // 31. West has 2-4 cards, or West has exactly one top.
  Manual::WestGeneralTwo(distData, 2, 4, 0, 3,    0, 5, 1, 1);

  // 32. West has 1-2 tops, or 2-3 either way.
  Manual::WestGeneralTwo(distData, 0, 5, 1, 2,    2, 3, 0, 3);
}


void Manual::prepare_5_4(DistData& distData) const
{
  Manual::WestLengthRange(distData, 1, 3);      // 0. 1=4, 2=3 or 3=2
  Manual::WestLengthRange(distData, 1, 4);      // 1. 1=4, 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 2, 3);      // 2. 2=3 or 3=2
  Manual::WestLengthRange(distData, 2, 4);      // 3. 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 2, 5);      // 4. 2=3, 3=2, 4=1 or 5=0
}


void Manual::prepare_5_5(DistData& distData) const
{
  Manual::WestLengthRange(distData, 1, 3);      // 0. 1=4, 2=3 or 3=2
  Manual::WestLengthRange(distData, 1, 4);      // 1. 1=4, 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 2, 3);      // 2. 2=3 or 3=2
  Manual::WestLengthRange(distData, 2, 4);      // 3. 2=3, 3=2 or 4=1
}


void Manual::prepare_6_1(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::WestLength(distData, 1);              // 2. West singleton
  Manual::EastLength(distData, 1);              // 3. East singleton
  Manual::WestLength(distData, 3);              // 4. 3=3
  Manual::WestLengthRange(distData, 2, 4);      // 5. 2=4, 3=3 or 4=2

  Manual::WestTop1(distData, 1);                // 6. West has the top
  Manual::EastTop1(distData, 1);                // 7. East has the top

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 8. H singleton West
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 9. H singleton

  Manual::WestGeneralAnd(distData, 0, 2, 1, 1); // 10. H(x) with West
  Manual::EastGeneralAnd(distData, 0, 2, 1, 1); // 11. H(x) with East
  Manual::SymmGeneralAnd(distData, 0, 2, 1, 1); // 12. H(x) on either side

  Manual::WestGeneralAnd(distData, 0, 4, 1, 1); // 13. H(xxx) with West
  Manual::EastGeneralAnd(distData, 0, 4, 1, 1); // 14. H(xxx) with East

  Manual::WestGeneralAnd(distData, 3, 3, 1, 1); // 15. Hxx with West

  Manual::WestGeneralAnd(distData, 0, 3, 1, 1); // 16. H(xx) with West
  Manual::EastGeneralAnd(distData, 0, 3, 1, 1); // 17. H(xx) with East

  // 18. West has the top, or East has a singleton.
  Manual::WestGeneralTwo(distData, 0, 6, 1, 1,     5, 5, 0, 1);

  // 19. East has the top, or West has a singleton.
  Manual::WestGeneralTwo(distData, 0, 6, 0, 0,     1, 1, 0, 1);
}


void Manual::prepare_6_2(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::WestLength(distData, 1);              // 2. West has singleton
  Manual::EastLength(distData, 1);              // 3. East has singleton
  Manual::WestLength(distData, 2);              // 4. West has doubleton
  Manual::WestLength(distData, 3);              // 5. 3=3
  Manual::WestLengthRange(distData, 2, 4);      // 6. 2=4, 3=3 or 4=2

  Manual::WestTop1(distData, 1);                // 7. West has ex. 1 top
  Manual::WestTop1(distData, 2);                // 8. West has both tops
  Manual::WestTop1Range(distData, 1, 2);        // 9. West has 1-2 tops
  Manual::EastTop1(distData, 2);                // 10. East has both tops
  Manual::SymmGeneralAnd(distData, 0, 6, 0, 0); // 11. Tops same side

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 12. H singleton West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 13. H singleton East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 14. H singleton

  Manual::WestGeneralAnd(distData, 2, 2, 2, 2); // 15. HH doubleton West
  Manual::EastGeneralAnd(distData, 2, 2, 2, 2); // 16. HH doubleton East
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 17. HH doubleton

  Manual::WestGeneralAnd(distData, 3, 3, 2, 2); // 18. HHx with West
  Manual::EastGeneralAnd(distData, 3, 3, 2, 2); // 19. HHx with East

  Manual::WestGeneralAnd(distData, 0, 2, 1, 1); // 20. H(x) with West
  Manual::WestGeneralAnd(distData, 0, 3, 1, 2); // 21. 1-3 or H(x), HH W

  Manual::WestGeneralAnd(distData, 0, 2, 1, 2); // 22. H, Hx, HH West
  Manual::EastGeneralAnd(distData, 0, 3, 0, 0); // 23. (xxx) East
  Manual::WestGeneralAnd(distData, 3, 6, 0, 0); // 24. x(xxx) West
  Manual::EastGeneralAnd(distData, 0, 2, 1, 1); // 25. H(x) East

  Manual::SymmGeneralAnd(distData, 0, 2, 1, 2); // 26. H, Hx, HH West
  Manual::WestGeneralAnd(distData, 4, 5, 2, 2); // 27. HHxx(x) with West

  // 28. The suit splits 3=3, or West has both tops.
  Manual::WestGeneralTwo(distData, 3, 3, 0, 2,     0, 6, 2, 2);

  // 29. West has both tops, or East has at most a singleton.
  Manual::WestGeneralTwo(distData, 0, 6, 2, 2,     5, 6, 0, 2);

  // 30. West has both tops, or East has a singleton.
  Manual::WestGeneralTwo(distData, 0, 6, 2, 2,     5, 5, 0, 2);

  // 31. The suit splits between 2-4 and 4-2, or
  // West has exactly one top.
  Manual::WestGeneralTwo(distData, 2, 4, 0, 2,     0, 6, 1, 1);
}


void Manual::prepare_6_3(DistData& distData) const
{
  Manual::WestLength(distData, 3);              // 0. 3=3
  Manual::WestLengthRange(distData, 2, 4);      // 1. 2=4, 3=3 or 4=2

  Manual::WestTop1(distData, 3);                // 2. West has all tops

  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 3. H singleton
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 4. HH doubleton

  Manual::WestGeneralAnd(distData, 0, 3, 1, 3); // 5. 1+ H, length <= 3
  Manual::EastGeneralAnd(distData, 0, 3, 1, 3); // 6. 1+ H, length <= 3

  // Why are these needed?
  Manual::WestGeneralAnd(distData, 0, 2, 1, 1); // 7. H, Hx with West
  Manual::WestGeneralAnd(distData, 4, 6, 3, 3); // 8. (xx) with East

  // 9. West has all three tops, or the suit splits between 2=4 and 4=2.
  Manual::WestGeneralTwo(distData, 0, 6, 3, 3,     2, 4, 0, 3);

  // 10. 2=4 or 3=3, or West has exactly one top.
  Manual::WestGeneralTwo(distData, 2, 3, 0, 3,     0, 6, 1, 1);

  // 11. 3=3 or 4=2, or East has exactly one top.
  Manual::WestGeneralTwo(distData, 3, 4, 0, 3,     0, 6, 2, 2);
}


void Manual::prepare_6_4(DistData& distData) const
{
  Manual::WestLength(distData, 3);              // 0. 3=3
  Manual::WestLengthRange(distData, 2, 4);      // 1. 2=4, 3=3 or 4=2

  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 2. H singleton
}


void Manual::prepare_6_5(DistData& distData) const
{
  Manual::WestLength(distData, 3);              // 0. 3=3
  Manual::WestLengthRange(distData, 2, 4);      // 1. 2=4, 3=3 or 4=2
}


void Manual::prepare_6_6(DistData& distData) const
{
  Manual::WestLength(distData, 3);              // 0. 3=3
  Manual::WestLengthRange(distData, 2, 4);      // 1. 2=4, 3=3 or 4=2
  Manual::WestLengthRange(distData, 2, 3);      // 2. 2=4 or 3=3
  Manual::WestLengthRange(distData, 3, 4);      // 3. 3=3 or 4=2
}


void Manual::prepare_7_1(DistData& distData) const
{
  Manual::WestTop1(distData, 1);                // 0. West has the top

  Manual::WestGeneralAnd(distData, 0, 2, 1, 1); // 1. H(x) with West

  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 2. H either side
  Manual::SymmGeneralAnd(distData, 0, 2, 1, 1); // 3. H(x) either side
  Manual::SymmGeneralAnd(distData, 0, 3, 1, 1); // 4. H(xx) either side
}


void Manual::prepare_7_2(DistData& distData) const
{
  Manual::WestTop1(distData, 2);                // 0. West has the tops

  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 1. H singleton

  Manual::EastGeneralAnd(distData, 2, 2, 2, 2); // 2. HH with East
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 3. HH doubleton

  Manual::WestGeneralAnd(distData, 0, 2, 1, 1); // 4. H(x) with West
  Manual::SymmGeneralAnd(distData, 0, 2, 1, 1); // 5. H(x) either side
  Manual::SymmGeneralAnd(distData, 0, 2, 1, 2); // 6. H(x) either side

  Manual::SymmGeneralAnd(distData, 0, 3, 2, 2); // 7. HH(x) either side
}


void Manual::prepare_7_3(DistData& distData) const
{
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 0. HH singleton
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 1. HH doubleton
  Manual::SymmGeneralAnd(distData, 3, 3, 3, 3); // 2. HHH tripleton
}


void Manual::prepare_8_1(DistData& distData) const
{
  Manual::WestTop1(distData, 1);                // 0. West has the top
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 1. H singleton
  Manual::SymmGeneralAnd(distData, 0, 2, 1, 1); // 2. H(x) either side
}


void Manual::prepare_8_2(DistData& distData) const
{
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 0. H singleton
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 1. HH doubleton
}


void Manual::prepare_9_1(DistData& distData) const
{
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 0. H singleton
}


Manual::Manual(
  const Profile& sumProfile,
  list<list<ManualData>>& data)
{
  // Some parameter distributions leads this methods to return empty data,
  // as the solution is always trivial.

  const unsigned char numTops = 
    static_cast<unsigned char>(sumProfile.size());

  const unsigned char maxLength = sumProfile.length();
  const unsigned char maxTops = sumProfile[numTops-1];

  DistData distData = {&data, &sumProfile, maxLength, maxTops, numTops};
  
  if (maxLength == 2)
  {
    if (maxTops == 1)
      Manual::prepare_2_1(distData);
    else if (maxTops == 2)
      Manual::prepare_2_2(distData);
  }
  else if (maxLength == 3)
  {
    if (maxTops == 1)
      Manual::prepare_3_1(distData);
    else if (maxTops == 2)
      Manual::prepare_3_2(distData);
    else if (maxTops == 3)
      Manual::prepare_3_3(distData);
  }
  else if (maxLength == 4)
  {
    if (maxTops == 1)
      Manual::prepare_4_1(distData);
    else if (maxTops == 2)
      Manual::prepare_4_2(distData);
    else if (maxTops == 3)
      Manual::prepare_4_3(distData);
    else if (maxTops == 4)
      Manual::prepare_4_4(distData);
  }
  else if (maxLength == 5)
  {
    if (maxTops == 1)
      Manual::prepare_5_1(distData);
    else if (maxTops == 2)
      Manual::prepare_5_2(distData);
    else if (maxTops == 3)
      Manual::prepare_5_3(distData);
    else if (maxTops == 4)
      Manual::prepare_5_4(distData);
    else if (maxTops == 5)
      Manual::prepare_5_5(distData);
  }
  else if (maxLength == 6)
  {
    if (maxTops == 1)
      Manual::prepare_6_1(distData);
    else if (maxTops == 2)
      Manual::prepare_6_2(distData);
    else if (maxTops == 3)
      Manual::prepare_6_3(distData);
    else if (maxTops == 4)
      Manual::prepare_6_4(distData);
    else if (maxTops == 5)
      Manual::prepare_6_5(distData);
    else if (maxTops == 6)
      Manual::prepare_6_6(distData);
  }
  else if (maxLength == 7)
  {
    if (maxTops == 1)
      Manual::prepare_7_1(distData);
    else if (maxTops == 2)
      Manual::prepare_7_2(distData);
    else if (maxTops == 3)
      Manual::prepare_7_3(distData);
  }
  else if (maxLength == 8)
  {
    if (maxTops == 1)
      Manual::prepare_8_1(distData);
    else if (maxTops == 2)
      Manual::prepare_8_2(distData);
  }
  else if (maxLength == 9)
  {
    if (maxTops == 1)
      Manual::prepare_9_1(distData);
  }
}


/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
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
  mdata.profilePair.addTop(highestTop, lowerTopsIncl, upperTopsIncl);
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
  mdata1.profilePair.addTop(highestTop, lowerTopsIncl1, upperTopsIncl1);

  mlist.emplace_back(ManualData(* distData.sumProfilePtr));
  auto& mdata2 = mlist.back();

  mdata2.profilePair.setLength(lowerCardsIncl2, upperCardsIncl2);
  mdata2.profilePair.addTop(highestTop, lowerTopsIncl2, upperTopsIncl2);
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
  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 8. H(x) with West
}


void Manual::prepare_3_2(DistData& distData) const
{
  Manual::WestLengthRange(distData, 1, 3);      // 0. West is not void
  Manual::WestLengthRange(distData, 1, 2);      // 1. 1=2 or 2=1

  Manual::WestTop1(distData, 2);                // 2. West has both tops

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 3. H singleton with West
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
  Manual::WestLengthRange(distData, 1, 2);      // 4. 1=3, 2=2
  Manual::WestLengthRange(distData, 2, 4);      // 5. 2=2, 3=1, 4=0
  Manual::WestLengthRange(distData, 0, 3);      // 6. East is not void
  Manual::WestLengthRange(distData, 1, 3);      // 7. Nobody is void
  Manual::WestLengthRange(distData, 1, 4);      // 8. West is not void
  Manual::WestLengthRange(distData, 2, 3);      // 9. 2=2, 3=1

  Manual::WestTop1(distData, 1);                // 10. West has the top
  Manual::EastTop1(distData, 1);                // 11. East has the top

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 12. H singleton with West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 13. H singleton with East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 14. H singleton

  Manual::WestGeneralAnd(distData, 2, 2, 1, 1); // 15. Hx with West
  Manual::EastGeneralAnd(distData, 2, 2, 1, 1); // 16. Hx with East

  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 17. H(x) with West

  Manual::WestGeneralAnd(distData, 1, 3, 1, 1); // 18. H(xx) with West
  Manual::EastGeneralAnd(distData, 1, 3, 1, 1); // 19. H(xx) with East

  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 20. Hxx=H
}


void Manual::prepare_4_2(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::EastLength(distData, 1);              // 2. East has any singleton
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

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 14. H singleton with West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 15. H singleton with East

  Manual::WestGeneralAnd(distData, 2, 2, 2, 2); // 16. HH doubleton West
  Manual::EastGeneralAnd(distData, 2, 2, 2, 2); // 17. HH doubleton East
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 18. HH doubleton

  Manual::WestGeneralAnd(distData, 2, 3, 1, 1); // 19. Hx(x) with West
  Manual::WestGeneralAnd(distData, 2, 3, 2, 2); // 20. HH(x) with West
  Manual::EastGeneralAnd(distData, 2, 3, 1, 1); // 21. Hx(x) with East

  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 22. X. H, Hx with West
  Manual::WestGeneralAnd(distData, 1, 2, 1, 2); // 23. X. H, Hx, HH West
  Manual::EastGeneralAnd(distData, 1, 2, 1, 2); // 24. X. H, Hx, HH East
  Manual::WestGeneralAnd(distData, 1, 3, 1, 2); // 25. No void; West 1+ H
  Manual::WestGeneralAnd(distData, 2, 3, 1, 2); // 26. West 2-3c with 1+ H
  Manual::WestGeneralAnd(distData, 2, 4, 1, 2); // 27. West 2-4c with 1+ H
  Manual::WestGeneralAnd(distData, 3, 4, 2, 2); // 28. HHx(x) with West

  Manual::EastGeneralAnd(distData, 3, 3, 2, 2); // 29. HHx with East
  Manual::WestGeneralAnd(distData, 3, 3, 2, 2); // 30. HHx with West

  // 31. West has exactly one top, or the suit splits 2=2.
  Manual::WestGeneralTwo(
    distData,
    0, 4, 1, 1,  // WestTop1(1)
    2, 2, 0, 2); // WestLength(2)

  // 32. West has both tops, or the suit splits 2=2.
  Manual::WestGeneralTwo(
    distData,
    0, 4, 2, 2,  // WestTop1(2)
    2, 2, 0, 2); // WestLength(2)

  // 33. East has exactly one top, or the suit splits 2=2.
  Manual::WestGeneralTwo(
    distData,
    0, 4, 1, 1,  // EastTop1(1)
    2, 2, 0, 2); // EastLength(2)

  // 34. East has both tops, or the suit splits 2=2.
  Manual::WestGeneralTwo(
    distData,
    0, 4, 0, 0,  // EastTop1(2)
    2, 2, 0, 2); // EastLength(2)

  // 35. West has both tops, or West has H, Hx, HH.
  Manual::WestGeneralTwo(
    distData,
    0, 4, 2, 2,  // WestTop1(2)
    1, 2, 1, 2); // WestGeneralAnd(1, 2, 1, 2)

  // 36. East has both tops, or East has H, Hx, HH.
  Manual::WestGeneralTwo(
    distData,
    0, 4, 0, 0,  // EastTop1(2)
    2, 3, 0, 1); // EastGeneralAnd(1, 2, 1, 2)

  // 37. East has at most 2 cards, or East has the tops.
  Manual::WestGeneralTwo(
    distData,
    0, 4, 0, 0,  // EastTop1(2)
    2, 4, 0, 2); // EastLengthRange(0, 2)
}


void Manual::prepare_4_3(DistData& distData) const
{
  Manual::WestLength(distData, 2);              // 0. 2=2
  Manual::WestLengthRange(distData, 1, 3);      // 1. 1=3, 2=2 or 3=1
  Manual::WestLengthRange(distData, 1, 4);      // 2. West is not void

  Manual::WestTop1(distData, 3);                // 3. West has all tops

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 4. H singleton with West
  Manual::WestGeneralAnd(distData, 2, 2, 2, 2); // 5. HH doubleton with West
}


void Manual::prepare_4_4(DistData& distData) const
{
  Manual::WestLength(distData, 2);              // 0. 2=2
  Manual::WestLengthRange(distData, 1, 3);      // 1. 1=3, 2=2 or 3=1
}


void Manual::prepare_5_1(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::WestLengthRange(distData, 0, 4);      // 2. East is not void
  Manual::WestLengthRange(distData, 1, 4);      // 3. 1=4, 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 2, 3);      // 4. 3-2 either way

  Manual::WestTop1(distData, 1);                // 5. West has the top
  Manual::EastTop1(distData, 1);                // 6. East has the top

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 7. H with West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 8. H with East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 9. H singleton 

  Manual::WestGeneralAnd(distData, 2, 2, 1, 1); // 10. Hx doubleton West

  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 11. H(x) with West
  Manual::EastGeneralAnd(distData, 1, 2, 1, 1); // 12. H(x) with East
  Manual::SymmGeneralAnd(distData, 1, 2, 1, 1); // 13. H(x) either way

  Manual::WestGeneralAnd(distData, 1, 3, 1, 1); // 14. H(xx) with West
  Manual::EastGeneralAnd(distData, 1, 3, 1, 1); // 15. H(xx) with East

  Manual::WestGeneralAnd(distData, 1, 4, 1, 1); // 16. H(xxx) with West

  Manual::WestGeneralAnd(distData, 2, 3, 1, 1); // 17. Hx(x) with West
  Manual::EastGeneralAnd(distData, 2, 3, 1, 1); // 18. Hx(x) with East

  Manual::WestGeneralAnd(distData, 4, 4, 1, 1); // 19. Hxxx with West

  Manual::EastGeneralAnd(distData, 4, 5, 1, 1); // 20. Hxxx(x) with East

  // 21. 2=3, 3=2 or East has the top.
  Manual::WestGeneralTwo(
    distData,
    0, 5, 0, 0,  // EastTop1(1)
    2, 3, 0, 1); // WestLengthRange(2, 3)
}


void Manual::prepare_5_2(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::WestLengthRange(distData, 0, 3);      // 2. 0=5, 1=4, 2=3 or 3=2
  Manual::WestLengthRange(distData, 0, 4);      // 3. East is not void
  Manual::WestLengthRange(distData, 1, 4);      // 4. 1=4, 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 1, 5);      // 5. West is not void
  Manual::WestLengthRange(distData, 2, 3);      // 6. 2=3 or 3=2
  Manual::WestLengthRange(distData, 2, 5);      // 7. 2=3, 3=2, 4=1 or 5=0

  Manual::WestTop1(distData, 2);                // 8. West has the tops
  Manual::WestTop1Range(distData, 1, 2);        // 9. West has 1+ tops
  Manual::EastTop1(distData, 2);                // 10. West has the tops

  Manual::EastGeneralAnd(distData, 0, 2, 0, 0); // 11. -, x, xx with East

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 12. H singleton with West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 13. H singleton with East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 14. H singleton

  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 15. H(x) with West
  Manual::EastGeneralAnd(distData, 1, 2, 1, 1); // 16. H(x) with East

  Manual::WestGeneralAnd(distData, 1, 2, 1, 2); // 17. H, Hx, HH West
  Manual::EastGeneralAnd(distData, 1, 2, 1, 2); // 18. H, Hx, HH East

  Manual::WestGeneralAnd(distData, 1, 3, 1, 2); // 19. H(xx), HH(x) West

  Manual::WestGeneralAnd(distData, 1, 1, 0, 0); // 20. x with West
  Manual::EastGeneralAnd(distData, 1, 1, 0, 0); // 21. x with East

  Manual::WestGeneralAnd(distData, 2, 2, 2, 2); // 22. HH doubleton West
  Manual::EastGeneralAnd(distData, 2, 2, 2, 2); // 23. HH doubleton East
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 24. HH doubleton 

  Manual::WestGeneralAnd(distData, 2, 2, 0, 0); // 25. West xx
  Manual::WestGeneralAnd(distData, 2, 2, 1, 1); // 26. West Hx
  Manual::WestGeneralAnd(distData, 2, 2, 1, 2); // 27. HH with West

  Manual::WestGeneralAnd(distData, 2, 3, 1, 2); // 28. Hx(x) with West
  Manual::EastGeneralAnd(distData, 2, 3, 1, 2); // 29. Hx(x) with East
  Manual::EastGeneralAnd(distData, 2, 3, 2, 2); // 30. HH(x) with East

  Manual::WestGeneralAnd(distData, 2, 5, 1, 2); // 31. 2+ cards, 1+ top West

  Manual::WestGeneralAnd(distData, 3, 3, 1, 1); // 32. Hxx with West
  Manual::WestGeneralAnd(distData, 3, 3, 2, 2); // 33. HHx with West

  Manual::WestGeneralAnd(distData, 2, 3, 2, 2); // 34. HH, HHx with West

  // 35. 3-2 either way, or West has both H's.
  Manual::WestGeneralTwo(
    distData,
    0, 5, 2, 2,  // WestTop1(2)
    2, 3, 0, 2); // WestLengthRange(2, 3)

  // 36. 3-2 either way, or East has both H's.
  Manual::WestGeneralTwo(
    distData,
    0, 5, 0, 0,  // EastTop1(2)
    2, 3, 0, 2); // EastLengthRange(2, 3)
}


void Manual::prepare_5_3(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void

  Manual::WestLengthRange(distData, 0, 4);      // 2. East is not void
  Manual::WestLengthRange(distData, 1, 4);      // 3. 1=4, 2=3, 3=2 or 4=1
  Manual::WestLengthRange(distData, 1, 5);      // 4. West is not void
  Manual::WestLengthRange(distData, 2, 3);      // 5. 2=3 or 3=2
  Manual::WestLengthRange(distData, 2, 5);      // 6. 2=3, 3=2, 4=1 or 5=0

  Manual::WestTop1Range(distData, 1, 3);        // 7. West has 1+ tops

  Manual::WestGeneralAnd(distData, 1, 1, 0, 0); // 8. x with West
  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 9. Stiff H West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 10. Stiff H East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 11. Stiff H

  Manual::WestGeneralAnd(distData, 4, 4, 3, 3); // 12. HHHx / x

  Manual::EastGeneralAnd(distData, 3, 3, 3, 3); // 13. HHH with East
}


void Manual::prepare_5_4(DistData& distData) const
{
  Manual::WestLengthRange(distData, 1, 5);      // 0. East is not void
  Manual::WestLengthRange(distData, 2, 3);      // 1. 2=3 or 3=2
  Manual::WestLengthRange(distData, 1, 4);      // 2. 1=4, 2=3, 3=2 or 4=1
}


void Manual::prepare_5_5(DistData& distData) const
{
  Manual::WestLengthRange(distData, 2, 3);      // 0. 2=3 or 3=2
  Manual::WestLengthRange(distData, 1, 4);      // 1. 1=4, 2=3, 3=2 or 4=1
}


void Manual::prepare_6_1(DistData& distData) const
{
  Manual::WestLength(distData, 0);              // 0. West is void
  Manual::EastLength(distData, 0);              // 1. East is void
  Manual::WestLength(distData, 3);              // 2. 3=3
  Manual::WestLengthRange(distData, 2, 4);      // 3. 2=4, 3=3 or 4=2

  Manual::WestTop1(distData, 1);                // 4. West has the top
  Manual::EastTop1(distData, 1);                // 5. East has the top

  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 6. H singleton either way
  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 7. H singleton with West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 8. East has H singleton

  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 9. H(x) with West
  Manual::EastGeneralAnd(distData, 1, 2, 1, 1); // 10. H(x) with East
  Manual::SymmGeneralAnd(distData, 1, 2, 1, 1); // 11. H(x) on either side

  Manual::WestGeneralAnd(distData, 1, 3, 1, 1); // 12. H(xx) with West
  Manual::EastGeneralAnd(distData, 1, 3, 1, 1); // 12. H(xx) with East
  Manual::WestGeneralAnd(distData, 3, 3, 1, 1); // 13. Hxx with West
}


void Manual::prepare_6_2(DistData& distData) const
{
  Manual::EastLength(distData, 0);              // 0. West is void
  Manual::WestLength(distData, 0);              // 1. West is void
  Manual::WestLength(distData, 3);              // 2. 3=3
  Manual::WestLengthRange(distData, 2, 4);      // 3. 2=4, 3=3 or 4=2

  Manual::WestTop1(distData, 2);                // 4. West has both tops
  Manual::WestTop1Range(distData, 1, 2);        // 5. West has 1-2 tops
  Manual::EastTop1(distData, 2);                // 6. East has both tops

  Manual::WestGeneralAnd(distData, 1, 1, 1, 1); // 7. H singleton with West
  Manual::EastGeneralAnd(distData, 1, 1, 1, 1); // 8. H singleton with East
  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 9. H singleton

  Manual::WestGeneralAnd(distData, 2, 2, 2, 2); // 10. HH doubleton West
  Manual::EastGeneralAnd(distData, 2, 2, 2, 2); // 11. HH doubleton East
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 12. HH doubleton

  Manual::WestGeneralAnd(distData, 1, 2, 0, 0); // 13. x(x) with West
  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 14. H(x) with West

  Manual::WestGeneralAnd(distData, 1, 3, 1, 2); // 15. 1-3 or H(x), HH W
  Manual::SymmGeneralAnd(distData, 1, 3, 1, 2); // 16. 1-3 or H(x) HH stiff
  Manual::EastGeneralAnd(distData, 3, 3, 2, 2); // 17. HHx with East
  Manual::EastGeneralAnd(distData, 2, 2, 1, 1); // 18. Hx with East
  Manual::WestGeneralAnd(distData, 2, 3, 2, 2); // 19. HH(x) with West
  Manual::WestGeneralAnd(distData, 3, 3, 2, 2); // 20. HHx with West
  Manual::WestGeneralAnd(distData, 4, 5, 2, 2); // 21. HHxx(x) with West
}


void Manual::prepare_6_3(DistData& distData) const
{
  Manual::WestLength(distData, 3);              // 0. 3=3
  Manual::WestLengthRange(distData, 2, 4);      // 1. 2=4, 3=3 or 4=2

  Manual::WestTop1(distData, 3);                // 2. West has all tops

  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 3. H singleton
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 4. HH doubleton
  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 5. H, Hx with West

  Manual::WestGeneralAnd(distData, 4, 6, 3, 3); // 6. HHH(xxx) with West

  Manual::EastGeneralAnd(distData, 3, 3, 1, 3); // 7. 3=3, 1+ H with East

  // 8. 2=4 or singleton top
  Manual::WestGeneralTwo(
    distData,
    0, 6, 3, 3,  // WestTop1(3)
    2, 4, 0, 3); // WestLengthRange(2, 4)
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
}


void Manual::prepare_7_1(DistData& distData) const
{
  Manual::WestTop1(distData, 1);                // 0. West has the top

  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 1. H(x) with West

  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 2. H either side
  Manual::SymmGeneralAnd(distData, 1, 2, 1, 1); // 3. H(x) either side
  Manual::SymmGeneralAnd(distData, 1, 3, 1, 1); // 4. H(xx) either side
}


void Manual::prepare_7_2(DistData& distData) const
{
  Manual::WestTop1(distData, 2);                // 0. West has the tops

  Manual::SymmGeneralAnd(distData, 1, 1, 1, 1); // 1. H singleton

  Manual::EastGeneralAnd(distData, 2, 2, 2, 2); // 2. HH with East
  Manual::SymmGeneralAnd(distData, 2, 2, 2, 2); // 3. HH doubleton

  Manual::WestGeneralAnd(distData, 1, 2, 1, 1); // 4. H(x) with West
  Manual::SymmGeneralAnd(distData, 1, 2, 1, 1); // 5. H(x) either side
  Manual::SymmGeneralAnd(distData, 1, 2, 1, 2); // 5. H(x) either side

  Manual::SymmGeneralAnd(distData, 2, 3, 2, 2); // 6. HH(x) either side
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
  Manual::SymmGeneralAnd(distData, 1, 2, 1, 1); // 2. H(x) either side
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


void Manual::make(
  const Profile& sumProfile,
  list<list<ManualData>>& data) const
{
  // Some parameter distributions leads this methods to return empty data,
  // as the solution is always trivial.

  const unsigned char numTops = 
    static_cast<unsigned char>(sumProfile.size());

  const unsigned char maxLength = sumProfile.getLength();
  const unsigned char maxTops = sumProfile.count(numTops-1);

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


/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERMEMORY_H
#define SSS_COVERMEMORY_H

#include <vector>
#include <list>
#include <string>

#include "CoverHelp.h"

using namespace std;


class CoverMemory
{
  private:

    vector<vector<list<CoverSpec>>> specs;


    // Uses global counters for more succinct notation
    CoverSpec& add();

    // ----- Pure length methods -----

    void WestLength(
      const unsigned char len,
      const bool invertFlag = false);

    void EastLength(
      const unsigned char len,
      const bool invertFlag = false);

    void WestLengthRange(
      const unsigned char len1, 
      const unsigned char len2,
      const bool invertFlag = false);

    void EastLengthRange(
      const unsigned char len1, 
      const unsigned char len2,
      const bool invertFlag = false);

    // ----- Pure rank-1 methods -----

    void WestTop1(
      const unsigned char len,
      const bool invertFlag = false);

    void EastTop1(
      const unsigned char len,
      const bool invertFlag = false);

    void WestTop1Range(
      const unsigned char len1, 
      const unsigned char len2,
      const bool invertFlag = false);

    void EastTop1Range(
      const unsigned char len1, 
      const unsigned char len2,
      const bool invertFlag = false);

    // ----- Length and top-1 methods -----

    void WestGeneralAnd(
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl);

    void EastGeneralAnd(
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl);

    // ------------

    void WestHasShortHonors(const unsigned char len);
    void EastHasShortHonors(const unsigned char len);

    void WestHasARangeHonor(
      const unsigned char lowerIncl,
      const unsigned char upperIncl);

    void EastHasARangeHonor(
      const unsigned char lowerIncl,
      const unsigned char upperIncl);

    void AnHonorIsShort(const unsigned char len);

    void HonorsAreShort(const unsigned char len);

    void WestHasGeneralOr(
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl);

    void EastHasGeneralOr(
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl);

    void prepare_2_1();
    void prepare_2_2();

    void prepare_3_1();
    void prepare_3_2();
    void prepare_3_3();

    void prepare_4_1();
    void prepare_4_2();
    void prepare_4_3();
    void prepare_4_4();

    void prepare_5_1();
    void prepare_5_2();
    void prepare_5_3();
    void prepare_5_4();
    void prepare_5_5();

    void prepare_6_splitsNonVoid();
    void prepare_6_1();
    void prepare_6_2();
    void prepare_6_3();
    void prepare_6_4();
    void prepare_6_5();
    void prepare_6_6();

    void prepare_7_1();
    void prepare_7_2();
    void prepare_7_3();

    void prepare_8_1();
    void prepare_8_2();

    void prepare_9_1();


  public:

    CoverMemory();

    void reset();

    void prepare(const unsigned char maxCards);

    list<CoverSpec>::const_iterator begin(
      const unsigned cards,
      const unsigned tops1) const;

    list<CoverSpec>::const_iterator end(
      const unsigned cards,
      const unsigned tops1) const;

    string str(
      const unsigned cards,
      const unsigned tops1) const;
};

#endif

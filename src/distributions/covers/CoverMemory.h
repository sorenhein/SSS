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

#include "CoverSpec.h"

using namespace std;

class Profile;
class ExplStats;
class ProductMemory;


class CoverMemory
{
  private:

    list<CoverSpec> specs;

    vector<vector<unsigned>> counts;



    // Uses global counters for more succinct notation
    CoverSpec& add();

    CoverSpec& addOrExtend(const CoverControl ctrl);

    // ----- Pure length methods -----

    void WestLength(
      const unsigned char len,
      const CoverControl ctrl = COVER_ADD);

    void EastLength(
      const unsigned char len,
      const CoverControl ctrl = COVER_ADD);

    void WestLengthRange(
      const unsigned char len1, 
      const unsigned char len2,
      const CoverControl ctrl = COVER_ADD);

    void EastLengthRange(
      const unsigned char len1, 
      const unsigned char len2,
      const CoverControl ctrl = COVER_ADD);

    // ----- Pure rank-1 methods -----

    void WestTop1(
      const unsigned char len,
      const CoverControl ctrl = COVER_ADD);

    void EastTop1(
      const unsigned char len,
      const CoverControl ctrl = COVER_ADD);

    void WestTop1Range(
      const unsigned char len1, 
      const unsigned char len2,
      const CoverControl ctrl = COVER_ADD);

    void EastTop1Range(
      const unsigned char len1, 
      const unsigned char len2,
      const CoverControl ctrl = COVER_ADD);

    // ----- Length and top-1 methods -----

    void WestGeneralAnd(
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl,
      const CoverControl ctrl = COVER_ADD);

    void EastGeneralAnd(
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl,
      const CoverControl ctrl = COVER_ADD);

    void SymmGeneralAnd(
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl,
      const CoverControl ctrl = COVER_ADD);

    void WestGeneralTwo(
      const unsigned char lowerCardsIncl1,
      const unsigned char upperCardsIncl1,
      const unsigned char lowerTopsIncl1,
      const unsigned char upperTopsIncl1,
      const unsigned char lowerCardsIncl2,
      const unsigned char upperCardsIncl2,
      const unsigned char lowerTopsIncl2,
      const unsigned char upperTopsIncl2);

    // ----- Length OR top-1 methods -----

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

    void prepare(
      const unsigned char maxLength,
      const unsigned char maxTops);


  public:

    CoverMemory();

    void reset();

    void resize(const unsigned char maxCards);

    void resizeStats(ExplStats& explStats) const;

    void prepareRows(
      Covers& covers,
      ProductMemory& productMemory,
      const unsigned char maxLength,
      const unsigned char maxTops,
      const unsigned char numTops,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);
};

#endif

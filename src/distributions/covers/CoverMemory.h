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

    vector<vector<list<CoverSpec>>> specs;


    // Uses global counters for more succinct notation
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

    // ----- Length OR top-1 methods -----

    void prepare_2_1(ProductMemory& productMemory);
    void prepare_2_2(ProductMemory& productMemory);

    void prepare_3_1(ProductMemory& productMemory);
    void prepare_3_2(ProductMemory& productMemory);
    void prepare_3_3(ProductMemory& productMemory);

    void prepare_4_1(ProductMemory& productMemory);
    void prepare_4_2(ProductMemory& productMemory);
    void prepare_4_3(ProductMemory& productMemory);
    void prepare_4_4(ProductMemory& productMemory);

    void prepare_5_1(ProductMemory& productMemory);
    void prepare_5_2(ProductMemory& productMemory);
    void prepare_5_3(ProductMemory& productMemory);
    void prepare_5_4(ProductMemory& productMemory);
    void prepare_5_5(ProductMemory& productMemory);

    void prepare_6_1(ProductMemory& productMemory);
    void prepare_6_2(ProductMemory& productMemory);
    void prepare_6_3(ProductMemory& productMemory);
    void prepare_6_4(ProductMemory& productMemory);
    void prepare_6_5(ProductMemory& productMemory);
    void prepare_6_6(ProductMemory& productMemory);

    void prepare_7_1(ProductMemory& productMemory);
    void prepare_7_2(ProductMemory& productMemory);
    void prepare_7_3(ProductMemory& productMemory);

    void prepare_8_1(ProductMemory& productMemory);
    void prepare_8_2(ProductMemory& productMemory);

    void prepare_9_1(ProductMemory& productMemory);

    void makeSets(
      const unsigned char length,
      const Profile& comp);


  public:

    CoverMemory();

    void reset();

    void resizeStats(ExplStats& explStats) const;

    void prepare(
      ProductMemory& productMemory,
      const unsigned char maxCards);

    void prepareNew(
      ProductMemory& productMemory,
      const unsigned char maxLength,
      const unsigned char maxTops);

    void prepareRow(
      Covers& covers,
      ProductMemory& productMemory,
      const unsigned char maxLength,
      const unsigned char maxTops,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    list<CoverSpec>::const_iterator begin(
      const unsigned cards,
      const unsigned tops1) const;

    list<CoverSpec>::const_iterator end(
      const unsigned cards,
      const unsigned tops1) const;

    unsigned size(
      const unsigned cards,
      const unsigned tops1) const;
};

#endif

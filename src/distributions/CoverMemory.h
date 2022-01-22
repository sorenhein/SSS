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


    CoverSpec& add(
      const unsigned char cards,
      const unsigned char tops1);

    void WestIsNotVoid(
      const unsigned char cards,
      const unsigned char tops1);

    void EastIsNotVoid(
      const unsigned char cards,
      const unsigned char tops1);

    void EastIsVoid(
      const unsigned char cards,
      const unsigned char tops1);

    void WestHasSingleton(
      const unsigned char cards,
      const unsigned char tops1);

    void WestHasExactlyOneTop(
      const unsigned char cards,
      const unsigned char tops1);

    void EastHasExactlyOneTop(
      const unsigned char cards,
      const unsigned char tops1);

    void WestHasDoubleton(
      const unsigned char cards,
      const unsigned char tops1);

    void WestHasTripleton(
      const unsigned char cards,
      const unsigned char tops1);

    void WestHasCardRange(
      const unsigned char cards,
      const unsigned char tops1,
      const unsigned char lowerIncl,
      const unsigned char upperIncl);

    void WestHasShortHonors(
      const unsigned char cards,
      const unsigned char tops1,
      const unsigned char len);

    void AnHonorIsShort(
      const unsigned char cards,
      const unsigned char tops1,
      const unsigned char len);

    void HonorsAreShort(
      const unsigned char cards,
      const unsigned char tops1,
      const unsigned char len);

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

    void prepare_6_1();
    void prepare_6_3();

    void prepare_7_1();
    void prepare_7_2();

    void prepare_8_1();


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

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_MANUAL_H
#define SSS_MANUAL_H

#include <list>

#include "ProfilePair.h"

struct DistData;

using namespace std;


struct ManualData
{
  ProfilePair profilePair;
  bool symmFlag;

  ManualData(const Profile& sumProfile)
  {
    profilePair.init(sumProfile);
    symmFlag = false;
  };
};


class Manual
{
  private:

    // ----- Pure length methods -----

    void WestLength(
      DistData& distData,
      const unsigned char len) const;

    void EastLength(
      DistData& distData,
      const unsigned char len) const;

    void WestLengthRange(
      DistData& distData,
      const unsigned char len1, 
      const unsigned char len2) const;

    void EastLengthRange(
      DistData& distData,
      const unsigned char len1, 
      const unsigned char len2) const;

    // ----- Pure rank-1 methods -----

    void WestTop1(
      DistData& distData,
      const unsigned char len) const;

    void EastTop1(
      DistData& distData,
      const unsigned char len) const;

    void WestTop1Range(
      DistData& distData,
      const unsigned char len1, 
      const unsigned char len2) const;

    void EastTop1Range(
      DistData& distData,
      const unsigned char len1, 
      const unsigned char len2) const;

    // ----- Length and top-1 methods -----

    void WestGeneralAnd(
      DistData& distData,
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl) const;

    void EastGeneralAnd(
      DistData& distData,
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl) const;

    void SymmGeneralAnd(
      DistData& distData,
      const unsigned char lowerCardsIncl,
      const unsigned char upperCardsIncl,
      const unsigned char lowerTopsIncl,
      const unsigned char upperTopsIncl) const;

    void WestGeneralTwo(
      DistData& distData,
      const unsigned char lowerCardsIncl1,
      const unsigned char upperCardsIncl1,
      const unsigned char lowerTopsIncl1,
      const unsigned char upperTopsIncl1,
      const unsigned char lowerCardsIncl2,
      const unsigned char upperCardsIncl2,
      const unsigned char lowerTopsIncl2,
      const unsigned char upperTopsIncl2) const;

    // ----- Length OR top-1 methods -----

    void prepare_2_1(DistData& distData) const;
    void prepare_2_2(DistData& distData) const;

    void prepare_3_1(DistData& distData) const;
    void prepare_3_2(DistData& distData) const;
    void prepare_3_3(DistData& distData) const;

    void prepare_4_1(DistData& distData) const;
    void prepare_4_2(DistData& distData) const;
    void prepare_4_3(DistData& distData) const;
    void prepare_4_4(DistData& distData) const;

    void prepare_5_1(DistData& distData) const;
    void prepare_5_2(DistData& distData) const;
    void prepare_5_3(DistData& distData) const;
    void prepare_5_4(DistData& distData) const;
    void prepare_5_5(DistData& distData) const;

    void prepare_6_1(DistData& distData) const;
    void prepare_6_2(DistData& distData) const;
    void prepare_6_3(DistData& distData) const;
    void prepare_6_4(DistData& distData) const;
    void prepare_6_5(DistData& distData) const;
    void prepare_6_6(DistData& distData) const;

    void prepare_7_1(DistData& distData) const;
    void prepare_7_2(DistData& distData) const;
    void prepare_7_3(DistData& distData) const;

    void prepare_8_1(DistData& distData) const;
    void prepare_8_2(DistData& distData) const;

    void prepare_9_1(DistData& distData) const;


  public:

    void make(
      const unsigned char maxLength,
      const unsigned char maxTops,
      const unsigned char numTops,
      list<list<ManualData>>& data) const;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RESULT_H
#define SSS_RESULT_H

#include <string>

#include "Winners.h"

using namespace std;


enum CompareDetail
{
  WIN_NEUTRAL_OVERALL = 0x0,
  WIN_EQUAL_OVERALL = 0x1,
  WIN_FIRST_PRIMARY = 0x2,
  WIN_SECOND_PRIMARY = 0x4,
  WIN_FIRST_SECONDARY = 0x8,
  WIN_SECOND_SECONDARY = 0x10,
  WIN_DIFFERENT_PRIMARY = 0x20,
  WIN_DIFFERENT_SECONDARY = 0x40
};

#define WIN_FIRST_OVERALL 0xa
#define WIN_SECOND_OVERALL 0x14
#define WIN_PRIMARY (WIN_FIRST_PRIMARY | WIN_SECOND_PRIMARY | \
  WIN_DIFFERENT_PRIMARY)

CompareType compressCore(const unsigned detail);
CompareType compressCompareDetail(const unsigned detail);
CompareType compressCompareSecondaryDetail(const unsigned detail);


class Result
{
  friend class Ranks;

  private:

    unsigned char distInt;

    unsigned char tricksInt;

    Winners winnersInt;

  public:

    Result();

    void set(
      const unsigned char distIn,
      const unsigned char tricksIn,
      const Winners& winnersIn);

    void setDist(const unsigned char distIn);

    void setTricks(const unsigned char tricksIn);

    void update(
      const Play& play,
      const unsigned char dist);

    void flip();

    void multiplyWinnersOnto(Result& result) const;

    void operator *= (const Result& result2);

    void operator += (const Result& result2);
  
    bool operator == (const Result& res2) const;

    bool operator != (const Result& res2) const;

    CompareDetail comparePrimaryInDetail(const Result& res2) const;

    Compare compareComplete(const Result& res2) const;

    CompareDetail compareSecondaryInDetail(const Result& res2) const;

    CompareDetail compareInDetail(const Result& res2) const;

    unsigned char dist() const;

    unsigned char tricks() const;

    unsigned char rank() const;

    string strHeaderEntry(
      const bool rankFlag,
      const string& title = "") const;

    string strEntry(const bool rankFlag) const;

    string strWinners() const;

    string strHeader(const string& title) const;

    string str(const bool rankFlag) const;
};

#endif

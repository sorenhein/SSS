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


class Result
{
  // TODO We use result.winnersInt in three places.
  // Once we move to a single winner, maybe make an interface method
  // and unfriend.
  friend class MultiResult;

  private:

    unsigned char distInt;

    unsigned char tricksInt;

    // Winners winnersInt;
    Winner winner;

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

    void expand(
      const unsigned char dist,
      const char rankAdder);

    void flip();

    void multiplyWinnersOnto(Result& result) const;

    void operator *= (const Result& result2);

    bool operator == (const Result& res2) const;

    bool operator != (const Result& res2) const;

    CompareDetail comparePrimaryInDetail(const Result& res2) const;

    Compare compareComplete(const Result& res2) const;

    CompareDetail compareSecondaryInDetail(const Result& res2) const;

    CompareDetail compareInDetail(const Result& res2) const;

    unsigned char dist() const;

    unsigned char tricks() const;

    unsigned char rank() const;

    unsigned char winAbsNumber() const;

    string strEntry(const bool rankFlag) const;

    string strWinners() const;

    string strHeader(const string& title) const;

    string str(const bool rankFlag) const;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RESULT_H
#define SSS_RESULT_H

#include <string>

#include "Winners.h"

using namespace std;


class Result
{
  private:

    unsigned char dist;

    unsigned char tricks;

    Winner winner;

  public:

    Result();

    void set(
      const unsigned char distIn,
      const unsigned char tricksIn,
      const Winner& winnerIn);

    void setDist(const unsigned char distIn);

    void setTricks(const unsigned char tricksIn);

    void update(
      const Play& play,
      const unsigned char distIn);

    void expand(
      const unsigned char distIn,
      const char rankAdder);

    void flip();

    void multiplyWinnersOnto(Result& result) const;

    void operator *= (const Result& result);

    bool operator == (const Result& result) const;

    bool operator != (const Result& result) const;

    CompareDetail comparePrimaryInDetail(const Result& result) const;

    Compare compareComplete(const Result& result) const;

    CompareDetail compareSecondaryInDetail(const Result& result) const;

    CompareDetail compareInDetail(const Result& result) const;

    unsigned char getDist() const;

    unsigned char getTricks() const;

    unsigned char getRank() const;

    unsigned char winAbsNumber() const;

    const Winner& getWinner() const;

    string strEntry(const bool rankFlag) const;

    string strWinners() const;

    string strHeader(const string& title) const;

    string str(const bool rankFlag) const;
};

#endif

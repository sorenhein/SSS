/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMPLETION_H
#define SSS_COMPLETION_H

#include <vector>
#include <list>
#include <string>

using namespace std;

class RanksNames;
enum Opponent: unsigned;


class Completion
{
  // A single combination such as 97x.

  private:

    vector<unsigned char> west;
    vector<unsigned char> east;

    vector<bool> used;

    list<unsigned char> openTopNumbers;

    unsigned char lengthInt;


  public:

    void resize(const size_t numTops);

    void setTop(
      const unsigned char topNo,
      const bool usedFlag,
      const unsigned char countWest,
      const unsigned char maximum);

    void updateTop(
      const unsigned char topNo,
      const unsigned char count,
      const unsigned char maximum);

    const list<unsigned char>& openTops() const;

    unsigned char length() const;

    bool operator < (const Completion& comp2) const;

    bool operator == (const Completion& comp2) const;

    string strDebug() const;

    string strSet(
      const RanksNames& ranksNames,
      const Opponent side,
      const bool expandFlag,
      const bool singleRankFlag,
      const bool explicitVoidFlag = false) const;

    string strUnset(
      const RanksNames& ranksNames,
      const Opponent side) const;
};

#endif

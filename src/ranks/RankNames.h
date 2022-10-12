/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RANKNAMES_H
#define SSS_RANKNAMES_H

#include <string>

using namespace std;

enum Side: unsigned;


enum RankName: unsigned
{
  RANKNAME_ACTUAL_FULL = 0,     // jack-ten
  RANKNAME_ACTUAL_SHORT = 1,    // JT
  RANKNAME_ACTUAL_LOW_DEF = 2,    // the ten (the lowest of that rank)
  RANKNAME_ABSOLUTE_SHORT = 3,  // HH
  RANKNAME_RELATIVE_SHORT = 4,
  RANKNAME_SIZE = 5
};


class RankNames
{
  private:

    size_t count;

    Side sideInt;

    string names[RANKNAME_SIZE];


  public:

    RankNames();

    void add(
      const Side side,
      const size_t index);

    void completeOpps(
      size_t& noAbs,
      size_t& noRel);

    void completeNS();

    void makeXes();

    Side side() const;

    size_t size() const;

    string strComponent(const RankName rankName) const;

    string strOpponents(const unsigned char numCards) const;

    string strHeader() const;

    string str(const size_t number) const;
};

#endif

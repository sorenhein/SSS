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
  RANKNAME_ABSOLUTE_SHORT = 2,  // HH
  RANKNAME_RELATIVE_SHORT = 3,
  RANKNAME_SIZE = 4
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

    string strOpponentsExpanded(const unsigned char numCards) const;

    string strHeader() const;

    string str(const size_t number) const;
};

#endif

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
  RANKNAME_ACTUAL_FULL = 0,
  RANKNAME_ACTUAL_SHORT = 1,
  RANKNAME_ABSOLUTE_FULL = 2,
  RANKNAME_ABSOLUTE_SHORT = 3,
  RANKNAME_RELATIVE_SHORT = 4,
  RANKNAME_FULL = 5
};


class RankNames
{
  private:

    size_t count;

    Side sideInt;

    string strFullInt;
    string strShortInt;


  public:

    RankNames();

    void add(
      const Side side,
      const size_t index);

    Side side() const;

    size_t size() const;

    string strFull() const;

    string strShort() const;

    string strHeader() const;

    string str(const size_t number) const;

    string str(
      const RankName rankName,
      const size_t number) const;
};

#endif

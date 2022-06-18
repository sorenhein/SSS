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
};

#endif

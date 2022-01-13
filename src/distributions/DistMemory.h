/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTMEMORY_H
#define SSS_DISTMEMORY_H

#include <vector>
#include <string>

#include "Distribution.h"
#include "DistCore.h"

using namespace std;


class DistMemory
{
  private:

    unsigned char maxCards;

    bool fullFlag;

    vector<vector<Distribution>> distributions;

    vector<vector<DistCore>> uniques;

    vector<unsigned> counters;

    vector<unsigned> cumulSplits;


  public:

    DistMemory();

    void reset();

    void resize(
      const unsigned char maxCardsIn,
      const bool fullFlag = true);

    // Thread-safe
    void add(
      const unsigned char cards,
      const unsigned holding);

    const Distribution& get(
      const unsigned char cards,
      const unsigned holding) const;

    unsigned size(const unsigned char cards) const;

    unsigned numUniques(const unsigned char cards) const;

    unsigned numSplits(const unsigned char cards) const;

    string strDynamic() const;
    string str(const unsigned char cards) const;
};

#endif

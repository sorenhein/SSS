/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTMEMORY_H
#define SSS_DISTMEMORY_H

#include <vector>
#include <string>

#include "DistributionX.h"
#include "DistCore.h"

using namespace std;


class DistMemory
{
  private:

    unsigned maxCards;

    bool fullFlag;

    vector<vector<DistributionX>> distributions;

    vector<vector<DistCore>> uniques;

    vector<unsigned> counters;

    vector<unsigned> cumulSplits;


  public:

    DistMemory();

    void reset();

    void resize(
      const unsigned maxCardsIn,
      const bool fullFlag = true);

    // Thread-safe
    void add(
      const unsigned cards,
      const unsigned holding);

    const DistributionX& get(
      const unsigned cards,
      const unsigned holding) const;

    unsigned numUniques(const unsigned cards) const;

    unsigned numSplits(const unsigned cards) const;

    string strDynamic() const;
    string str(const unsigned cards) const;
};

#endif

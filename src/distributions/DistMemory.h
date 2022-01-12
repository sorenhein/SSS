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

using namespace std;


class DistMemory
{
  private:


    unsigned maxCards;

    bool fullFlag;

    vector<vector<Distribution const *>> distEntries;

    vector<vector<Distribution>> uniques;

    vector<unsigned> counters;

    vector<unsigned> cumulSplits;


  public:

    DistMemory();

    void reset();

    void resize(
      const unsigned maxCardsIn,
      const bool fullFlag = true);

    // Thread-safe
    const Distribution& add(
      const unsigned cards,
      const unsigned holding);

    const Distribution& get(
      const unsigned cards,
      const unsigned holding) const;

    unsigned numUniques(const unsigned cards) const;

    unsigned numSplits(const unsigned cards) const;

    string str() const;
};

#endif

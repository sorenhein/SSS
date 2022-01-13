/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTMEMORY_H
#define SSS_DISTMEMORY_H

#include <vector>
#include <string>

#include "DistMap.h"
#include "DistCore.h"

using namespace std;


class DistMemory
{
  private:

    struct DistEntry
    {
      DistMap distMap;
      DistCore const * distCorePtr;
    };


    unsigned maxCards;

    bool fullFlag;

    vector<vector<DistEntry>> distEntries;

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

    const DistCore& getCore(
      const unsigned cards,
      const unsigned holding) const;

    unsigned numUniques(const unsigned cards) const;

    unsigned numSplits(const unsigned cards) const;

    string str() const;
};

#endif

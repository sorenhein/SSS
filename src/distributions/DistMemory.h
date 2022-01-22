/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTMEMORY_H
#define SSS_DISTMEMORY_H

#include <vector>
#include <set>
#include <string>

#include "Distribution.h"
#include "DistCore.h"
#include "CoverMemory.h"

using namespace std;


class DistMemory
{
  private:

    unsigned char maxCards;

    bool fullFlag;

    vector<vector<Distribution>> distributions;

    vector<vector<DistCore>> uniques;

    vector<unsigned> usedCounts;

    CoverMemory coverMemory;


  public:

    DistMemory();

    void reset();

    void resize(
      const unsigned char maxCardsIn,
      const bool fullFlag = true);

    void resizeSingle(const vector<set<unsigned>>& dependenciesCan);

    // Thread-safe.  Used for generating all distributions in a loop
    Distribution& addFullMT(
      const unsigned char cards,
      const unsigned holding);

    // Thread-safe.  Used for adding only certain distributions
    void addCanonicalMT(
      const unsigned char cards,
      const unsigned holding);

    // Thread-safe.  Used for adding only certain distributions
    void addNoncanonicalMT(
      const unsigned char cards,
      const unsigned holding);

    Distribution& get(
      const unsigned char cards,
      const unsigned holding);

    const Distribution& get(
      const unsigned char cards,
      const unsigned holding) const;

    unsigned size(const unsigned char cards) const;

    unsigned used(const unsigned char cards) const;

    string strDynamic() const;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBMEMORY_H
#define SSS_COMBMEMORY_H

#include <vector>
#include <string>

#include "CombEntry.h"
#include "Combination.h"

using namespace std;


class CombMemory
{
  private:


    unsigned maxCards;

    bool fullFlag;

    vector<vector<CombEntry>> combEntries;

    vector<vector<Combination>> uniques;

    vector<unsigned> counters;


  public:

    CombMemory();

    void reset();

    void resize(
      const unsigned maxCardsIn,
      const bool fullFlag = true);

    unsigned size(const unsigned cards) const;

    // Thread-safe
    Combination& add(
      const unsigned cards,
      const unsigned holding);

    const Combination& getComb(
      const unsigned cards,
      const unsigned holding) const;

    CombEntry& getEntry(
      const unsigned cards,
      const unsigned holding);

    const CombEntry& getEntry(
      const unsigned cards,
      const unsigned holding) const;

    string str() const;
};

#endif

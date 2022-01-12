/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBMEMORY_H
#define SSS_COMBMEMORY_H

#include <vector>
#include <string>
#include <atomic>

#include "CombEntry.h"
#include "Combination.h"

using namespace std;


class CombMemory
{
  private:


    unsigned maxCards;

    vector<vector<CombEntry>> combEntries;

    vector<vector<Combination>> uniques;

    atomic<unsigned> counter;


  public:

    CombMemory();

    void reset();

    void resize(const unsigned maxCardsIn);

    Combination& add(
      const unsigned cards,
      const unsigned holding);

    Combination& get(
      const unsigned cards,
      const unsigned holding);

};

#endif

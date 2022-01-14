/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTRIBUTIONS_H
#define SSS_DISTRIBUTIONS_H

#include <vector>
#include <string>

#include <atomic>
#include <mutex>

#include "DistMemory.h"

class Distribution;

using namespace std;


class Distributions
{
  private:

    unsigned char maxCards;

    // vector<vector<Distribution>> distributions;

    DistMemory distMemory;

    vector<unsigned> splitCounts;
    vector<unsigned> threadSplitCounts;

    mutex log; // Locked when a result is being logged
    atomic<unsigned> counter;

    void runUniqueThread(
      const unsigned char cards,
      const unsigned thid);

  public:

    Distributions();

    void reset();

    void resize(const unsigned char maxCardsIn);

    void add(
      const unsigned char cards,
      const unsigned holding);

    void runUniques(const unsigned char cards);

    void runUniquesMT(
      const unsigned char cards,
      const unsigned numThreads);

    const Distribution& get(
      const unsigned char cards,
      const unsigned holding2) const;

    string str(const unsigned char cards = 0) const;

};

#endif

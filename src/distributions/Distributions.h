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

#include "Distribution.h"

using namespace std;


class Distributions
{
  private:

    unsigned maxCards;

    vector<vector<Distribution>> distributions;

    vector<unsigned> counts;
    vector<unsigned> threadCounts;

    // Only canonical distributions are processed in detail.
    vector<unsigned> uniques;
    vector<unsigned> threadUniques;

    mutex log; // Locked when a result is being logged
    atomic<unsigned> counter;

    void runUniqueThread(
      const unsigned cards,
      const unsigned thid);

  public:

    Distributions();

    void reset();

    void resize(const unsigned maxCardsIn);

    void runUniques(const unsigned cards);

    void runUniquesMT(
      const unsigned cards,
      const unsigned numThreads);

    Distribution const * ptrCanonical(
      const unsigned cards,
      const unsigned holding2) const;

    Distribution const * ptrNoncanonical(
      const unsigned cards,
      const unsigned holding2) const;

    string str(const int unsigned = 0) const;

};

#endif

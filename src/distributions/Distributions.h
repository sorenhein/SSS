/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTRIBUTIONS_H
#define SSS_DISTRIBUTIONS_H

#include <vector>
#include <set>
#include <string>
#include <atomic>
#include <mutex>

#include "DistMemory.h"

#include "covers/ProductMemory.h"

class Distribution;

using namespace std;


class Distributions
{
  private:

    unsigned char maxCards;

    DistMemory distMemory;

    ProductMemory productMemory;

    vector<unsigned> splitCounts;
    vector<unsigned> threadSplitCounts;

    mutex log; // Locked when a result is being logged
    atomic<unsigned> counter;

    void runUniqueThread(
      const unsigned char cards,
      const unsigned thid);

    void strLimits(
      const unsigned char cards,
      unsigned char& cmin,
      unsigned char& cmax) const;

  public:

    Distributions();

    void reset();

    void resize(const unsigned char maxCardsIn);

    // void resizeStats(ExplStats& explStats) const;

    void runSingle(
      const vector<set<unsigned>>& dependenciesCan,
      const vector<set<unsigned>>& dependenciesNoncan);

    void runUniques(const unsigned char cards);

    void runUniquesMT(
      const unsigned char cards,
      const unsigned numThreads);

    Distribution& get(
      const unsigned char cards,
      const unsigned holding2);

    const Distribution& get(
      const unsigned char cards,
      const unsigned holding2) const;

    string strDynamic() const;

    string str(const unsigned char cards = 0) const;

    string strCovers(const unsigned char cards = 0) const;

    string strCoverCounts(const unsigned char cards = 0) const;

    string strProductEnterStats() const;
};

#endif

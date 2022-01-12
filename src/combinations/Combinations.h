/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBINATIONS_H
#define SSS_COMBINATIONS_H

#include <vector>
#include <set>
#include <string>
#include <atomic>

#include "CombMemory.h"
#include "Combination.h"
#include "CombEntry.h"
#include "CombHelp.h"

using namespace std;

class Distributions;


class Combinations
{
  private:

    CombMemory combMemory;

    // Indexed by cards and then by CombinationType
    vector<CombCountEntry> countStats;

    // Indexed by cards.  Non-canonical ones are not solved.
    vector<unsigned> countNonreference;

    // Indexed by thread ID and collapsed into countStats,
    // and then by CombinationType
    vector<CombCountEntry> threadCountStats;

    // Indexed by thread ID and collapsed into countNoncanonical.
    vector<unsigned> threadCountNonreference;

    atomic<unsigned> counterHolding; // Holding

    void dumpVS(
      const string& title,
      const unsigned cards,
      const vector<set<unsigned>>& vs) const;

    void getDependencies(
      const unsigned cards,
      const unsigned holding,
      vector<set<unsigned>>& dependenciesTrinary,
      vector<set<unsigned>>& dependendiesBinary);

    bool getMinimals(
      const Strategies& strategies,
      const Ranks& ranks,
      CombEntry& centry) const;

    CombinationType classify(
      const bool minimalFlag,
      const Strategies& strategies,
      const Ranks& ranks) const;

    void runUniqueThread(
      const unsigned cards,
      Distributions const * distributions,
      const unsigned thid);

    void fixMinimals(const unsigned cards);


  public:

    Combinations();

    void setTimerNames();

    void reset();

    void resize(
      const unsigned maxCardsIn,
      const bool fullFlag = false);

    void runSingle(
      const unsigned cards,
      const unsigned holding,
      Distributions& distributions);

    void runUniques(
      const unsigned cards,
      const Distributions& distributions);

    void runUniquesOld(
      const unsigned cards,
      const Distributions& distributions);

    void runUniquesMT(
      const unsigned cards,
      const Distributions& distributions,
      const unsigned numThreads);

    Combination const * getPtr(
      const unsigned cards,
      const unsigned holding3) const;

    string strUniques(const int unsigned = 0) const;

};

#endif

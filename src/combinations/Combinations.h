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
struct ExplStats;


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
      const unsigned char cards,
      const vector<set<unsigned>>& vs) const;

    bool getMinimals(
      const Strategies& strategies,
      const Ranks& ranks,
      CombEntry& centry) const;

    CombinationType classify(
      const bool minimalFlag,
      const Strategies& strategies,
      const Ranks& ranks) const;

    void runUniqueThread(
      const unsigned char cards,
      Distributions const * distributions,
      const unsigned thid);

    string strHeader() const;

    string strLine(const unsigned char cards) const;


  public:

    Combinations();

    void setTimerNames();

    void reset();

    void resize(
      const unsigned char maxCardsIn,
      const bool fullFlag = false);

    void getDependencies(
      const unsigned char cards,
      const unsigned holding,
      vector<set<unsigned>>& dependenciesTrinary,
      vector<set<unsigned>>& dependendiesBinaryCan,
      vector<set<unsigned>>& dependendiesBinaryNoncan);

    void runSingle(
      const unsigned char cards,
      const unsigned holding,
      const Distributions& distributions,
      const vector<set<unsigned>>& dependenciesTrinary);

    void runUniques(
      const unsigned char cards,
      const Distributions& distributions,
      ExplStats& explStats);

    void runUniquesOld(
      const unsigned char cards,
      const Distributions& distributions);

    void runUniquesMT(
      const unsigned char cards,
      const Distributions& distributions,
      const unsigned numThreads);

    void covers(
      const unsigned char cards,
      const unsigned holding,
      Distributions& distributions);

    Combination const * getPtr(
      const unsigned char cards,
      const unsigned holding3) const;

    string str(const unsigned char cards = 0) const;

};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBINATIONS_H
#define SSS_COMBINATIONS_H

#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <atomic>
#include <mutex>

#include "Combination.h"
#include "CombEntry.h"
#include "CombHelp.h"

using namespace std;

class Distributions;

enum CombMinimumMode
{
  COMB_MIN_IGNORE = 0,
  COMB_MIN_SINGLE = 1,
  COMB_MIN_FULL = 2
};


class Combinations
{
  private:


    unsigned maxCards;

    vector<vector<CombEntry>> combEntries;
    vector<vector<Combination>> uniques;

    // Indexed by cards and then by CombinationType
    vector<CombCountEntry> countStats;

    // Indexed by cards.  Non-canonical ones are not solved.
    vector<unsigned> countNonreference;

    // Indexed by thread ID and collapsed into countStats,
    // and then by CombinationType
    vector<CombCountEntry> threadCountStats;

    // Indexed by thread ID and collapsed into countNoncanonical.
    vector<unsigned> threadCountNonreference;

    mutex log; // Locked when a result is being logged
    atomic<unsigned> counterHolding; // Holding
    atomic<unsigned> counterUnique; // Unique index

    void dumpVS(
      const string& title,
      const unsigned cards,
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
      const unsigned cards,
      Distributions const * distributions,
      const unsigned thid);

    void fixMinimals(vector<CombEntry>& centries);


  public:

    Combinations();

    void setTimerNames();

    void reset();

    void resize(const unsigned maxCardsIn);

    void runSingle(
      const unsigned cards,
      const unsigned holding,
      const Distributions& distributions);

    /*
    void runSpecific(
      const unsigned cards,
      const unsigned holding,
      const Distributions& distributions);

    void runSpecificVoid(
      const unsigned cards,
      const unsigned holding,
      const Distributions& distributions);
      */

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
      const unsigned holding3,
      const CombMinimumMode mode,
      bool& rotateFlag) const;

    string strUniques(const int unsigned = 0) const;

};

#endif

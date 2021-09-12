#ifndef SSS_COMBINATIONS_H
#define SSS_COMBINATIONS_H

#include <vector>
#include <list>
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


class Combinations
{
  private:


    unsigned maxCards;

    vector<vector<CombEntry>> combEntries;
    vector<vector<Combination>> uniques;

    // Indexed by cards and then by CombinationType
    vector<CombCountEntry> countStats;

    // Indexed by cards.  Non-canonical ones are not solved.
    vector<unsigned> countNoncanonical;

    // Indexed by thread ID and collapsed into countStats,
    // and then by CombinationType
    vector<CombCountEntry> threadCountStats;

    // Indexed by thread ID and collapsed into countNoncanonical.
    vector<unsigned> threadCountNoncanonical;

    mutex log; // Locked when a result is being logged
    atomic<unsigned> counterHolding; // Holding
    atomic<unsigned> counterUnique; // Unique index

    bool getMinimals(
      const Strategies& strategies,
      const Ranks& ranks,
      list<CombReference>& minimals) const;

    CombinationType classify(
      const bool minimalFlag,
      const Strategies& strategies,
      const Ranks& ranks) const;

    void runUniqueThread(
      const unsigned cards,
      Distributions const * distributions,
      const unsigned thid);

    bool checkMinimals(
      const vector<CombEntry>& centries,
      const list<CombReference>& minimals) const;

    bool checkAndFixMinimals(
      const vector<CombEntry>& centries,
      list<CombReference>& minimals) const;

    void checkAllMinimals(const unsigned cards);


  public:

    Combinations();

    void reset();

    void resize(const unsigned maxCardsIn);

    void runSpecific(
      const unsigned cards,
      const unsigned holding,
      const Distributions& distributions);

    void runSpecificVoid(
      const unsigned cards,
      const unsigned holding,
      const Distributions& distributions);

    void runUniques(
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

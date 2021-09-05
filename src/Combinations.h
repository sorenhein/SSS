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

using namespace std;

class Distributions;
struct CombReference;


class Combinations
{
  private:

    struct CountEntry
    {
      unsigned total;
      unsigned count;

      void reset()
      {
        total = 0;
        count = 0;
      }

      void operator += (const CountEntry& c2)
      {
        total += c2.total;
        count += c2.count;
      }

      void operator += (const unsigned t)
      {
        total += t;
        count++;
      }

      string strAverage() const
      {
        if (count == 0)
          return "-";

        stringstream ss;
        ss << fixed << setprecision(2) << 
          total / static_cast<double>(count);
        return ss.str();
      }
    };

    struct CountPair
    {
      CountEntry unique;
      CountEntry minimal;
      CountEntry constant;
      CountEntry simple;

      void reset()
      {
        unique.reset();
        minimal.reset();
        constant.reset();
        simple.reset();
      }

      void operator += (const CountPair& cp)
      {
        unique += cp.unique;
        minimal += cp.minimal;
        constant += cp.constant;
        simple += cp.simple;
      }
    };

    struct CombCountEntry
    {
      CountPair plays;
      CountPair strats;

      void reset()
      {
        plays.reset();
        strats.reset();
      }

      void operator += (const CombCountEntry& cce)
      {
        plays += cce.plays;
        strats += cce.strats;
      }
    };

    unsigned maxCards;

    vector<vector<CombEntry>> combEntries;
    vector<vector<Combination>> uniques;

    // Indexed by cards
    vector<CombCountEntry> countStats;

    // Indexed by thread ID and collapsed into countStats
    vector<CombCountEntry> threadCountStats;

    mutex log; // Locked when a result is being logged
    atomic<unsigned> counterHolding; // Holding
    atomic<unsigned> counterUnique; // Unique index

    bool getMinimals(
      const Strategies& strategies,
      const Ranks& ranks,
      list<CombReference>& minimals) const;

    void runUniqueThread(
      const unsigned cards,
      Distributions const * distributions,
      const unsigned thid);

  public:

    Combinations();

    ~Combinations();

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

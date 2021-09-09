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

using namespace std;

class Distributions;


class Combinations
{
  private:

    struct CountEntry
    {
      unsigned count;
      unsigned totalPlays;
      unsigned totalStrats;

      void reset()
      {
        count = 0;
        totalPlays = 0;
        totalStrats = 0;
      }

      void operator += (const CountEntry& c2)
      {
        count += c2.count;
        totalPlays += c2.totalPlays;
        totalStrats += c2.totalStrats;
      }

      void incr(
        const unsigned plays,
        const unsigned strats)
      {
        count++;
        totalPlays += plays;
        totalStrats += strats;
      }

      string strAverage(const unsigned num) const
      {
        if (count == 0)
          return "-";

        stringstream ss;
        ss << fixed << setprecision(2) << num / static_cast<double>(count);
        return ss.str();
      }

      string strAveragePlays() const
      {
        return strAverage(totalPlays);
      }

      string strAverageStrats() const
      {
        return strAverage(totalStrats);
      }
    };

    struct CountPartition
    {
      vector<CountEntry> elements;

      void resize(const unsigned len)
      {
        elements.clear();
        elements.resize(len);
      }

      void operator += (const CountPartition& cp)
      {
        for (unsigned n = 0; n < elements.size(); n++)
          elements[n] += cp.elements[n];
      }
    };

    struct CombCountEntry
    {
      vector<CountEntry> data;

      void operator += (const CombCountEntry& cce)
      {
        assert(data.size() == cce.data.size());
        for (unsigned n = 0; n < data.size(); n++)
          data[n] += cce.data[n];
      }
    };

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

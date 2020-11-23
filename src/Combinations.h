#ifndef SSS_COMBINATIONS_H
#define SSS_COMBINATIONS_H

#include <vector>
#include <string>

#include <atomic>
#include <mutex>

#include "Combination.h"
#include "struct.h"

using namespace std;


class Combinations
{
  private:

    struct CountEntry
    {
      unsigned total;
      unsigned unique;

      void reset()
      {
        total = 0;
        unique = 0;
      }

      void operator +=(const CountEntry& c2)
      {
        total += c2.total;
        unique += c2.unique;
      }
    };

    unsigned maxCards;

    vector<vector<CombEntry>> combEntries;
    vector<vector<Combination>> uniques;

    vector<CountEntry> combCounts;
    vector<CountEntry> threadCombCounts;

    vector<CountEntry> playCounts;
    vector<CountEntry> threadPlayCounts;

    mutex log; // Locked when a result is being logged
    atomic<unsigned> counterHolding; // Holding
    atomic<unsigned> counterUnique; // Unique index

    void runUniqueThread(
      const unsigned cards,
      const unsigned thid);

  public:

    Combinations();

    ~Combinations();

    void reset();

    void resize(const unsigned maxCardsIn);

    void runUniques(const unsigned cards);

    void runUniquesMT(
      const unsigned cards,
      const unsigned numThreads);

    string strUniques(const int unsigned = 0) const;

};

#endif

#ifndef SSS_COMBINATIONS_H
#define SSS_COMBINATIONS_H

#include <vector>
#include <string>

#include <atomic>
#include <thread>
#include <mutex>

#include "const.h"

using namespace std;


class Combinations
{
  private:

    struct CountEntry
    {
      unsigned total;
      unsigned unique;
    };

    unsigned maxCards;

    vector<vector<CombEntry>> combinations;
    vector<vector<unsigned>> uniques; // Probably gets more structure

    vector<CountEntry> counts;
    vector<CountEntry> threadCounts;

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

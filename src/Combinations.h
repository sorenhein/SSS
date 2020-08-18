#ifndef SSS_COMBINATIONS_H
#define SSS_COMBINATIONS_H

#include <map>
#include <list>
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
      int total;
      int unique;
    };

    int maxCards;

    vector<vector<CombEntry>> combinations;
    vector<vector<int>> uniques; // Probably gets more structure

    vector<CountEntry> counts;
    vector<CountEntry> threadCounts;

    mutex log; // Locked when a result is being logged
    atomic<int> counterHolding; // Holding
    atomic<int> counterUnique; // Unique index

    void runUniqueThread(
      const int cards,
      const int thid);

  public:

    Combinations();

    ~Combinations();

    void reset();

    void resize(const int maxCardsIn);

    void runUniques(const int cards);

    void runUniquesMT(
      const int cards,
      const int numThreads);

    string strUniques(const int cards = 0) const;

};

#endif

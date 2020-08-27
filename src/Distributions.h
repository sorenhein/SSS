#ifndef SSS_DISTRIBUTIONS_H
#define SSS_DISTRIBUTIONS_H

#include <vector>
#include <string>

#include <atomic>
#include <mutex>

#include "Distribution.h"

using namespace std;


class Distributions
{
  private:

    unsigned maxCards;

    vector<vector<Distribution>> distributions;

    vector<unsigned> counts;
    vector<unsigned> threadCounts;

    mutex log; // Locked when a result is being logged
    atomic<unsigned> counter;

    void runUniqueThread(
      const unsigned cards,
      const unsigned thid);

  public:

    Distributions();

    ~Distributions();

    void reset();

    void resize(const unsigned maxCardsIn);

    void runUniques(const unsigned cards);

    void runUniquesMT(
      const unsigned cards,
      const unsigned numThreads);

    string str(const int unsigned = 0) const;

};

#endif

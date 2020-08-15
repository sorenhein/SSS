#ifndef SSS_TIMERs_H
#define SSS_TIMERs_H

#pragma warning(push)
#pragma warning(disable: 4365 4571 4625 4626 4774 5026 5027)
#include <chrono>
#pragma warning(pop)

#include <vector>
#include <string>

#include "Timer.h"

using namespace std;


enum TimerName
{
  TIMER_UNIQUE = 0,
  TIMER_FILL_TREE = 1,
  TIMER_DERIVE_STRAT = 2,
  TIMER_WRITE_CONST = 3,
  TIMER_WRITE_SIMPLE = 4,
  TIMER_TEST_PURE = 5,
  TIMER_DERIVE_MIXED = 6,
  TIMER_ALL_THREADS = 7,

  TIMER_SIZE = 8
};


class Timers
{
  private:

    vector<Timer> timers;
    vector<string> names;


  public:

    Timers();

    ~Timers();

    void reset();

    void start(const TimerName tname);

    void stop(const TimerName tname);

    void operator += (const Timers& timers2);

    string str(const int prec = 1) const;
};

#endif

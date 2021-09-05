/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

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
  TIMER_DISTRIBUTIONS = 0,
  TIMER_COMBINATIONS = 1,
  TIMER_RANKS = 2,
  TIMER_PLAYS = 3,
  TIMER_STRATEGIZE = 4,
  TIMER_WRITE_CONST = 5,
  TIMER_WRITE_SIMPLE = 6,
  TIMER_TEST_PURE = 7,
  TIMER_DERIVE_MIXED = 8,
  TIMER_ALL_THREADS = 9,

  TIMER_SIZE = 10
};


class Timers
{
  private:

    vector<Timer> timers;
    vector<string> names;


  public:

    Timers();

    void reset();

    void start(const TimerName tname);

    void stop(const TimerName tname);

    void operator += (const Timers& timers2);

    string str(const int prec = 1) const;
};

#endif

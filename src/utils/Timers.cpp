/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Timers.h"


Timers::Timers()
{
  Timers::reset();
}


void Timers::reset()
{
  timers.resize(TIMER_SIZE);
  names.resize(TIMER_SIZE);

  for (auto& t: timers)
    t.reset();
  
  names[TIMER_DISTRIBUTIONS] = "Distributions";
  names[TIMER_COMBINATIONS] = "Combinations";
  names[TIMER_RANKS] = "Ranks";
  names[TIMER_PLAYS] = "Plays";
  names[TIMER_STRATEGIZE] = "Strategize";
  names[TIMER_WRITE_CONST] = "Const";
  names[TIMER_WRITE_SIMPLE] = "Simple";
  names[TIMER_TEST_PURE] = "Pure";
  names[TIMER_DERIVE_MIXED] = "Mixed";
  names[TIMER_ALL_THREADS] = "WALL TIME";
}


void Timers::start(const TimerName tname)
{
  timers[tname].start();
}


void Timers::stop(const TimerName tname)
{
  timers[tname].stop();
}


void Timers::operator += (const Timers& timers2)
{
  for (unsigned i = 0; i < timers.size(); i++)
    timers[i] += timers2.timers[i];
}


string Timers::str(const int prec) const 
{
  stringstream ss;

  ss << "Stats: Timers\n";
  ss << string(13, '-') << "\n\n";

  ss << setw(24) << left << "Name" << 
    setw(13) << right << "Sum" << 
    setw(10) << right << "Count" << 
    setw(13) << right << "Average" << "\n";

  Timer timerSum;
  for (unsigned i = 0; i+1 < timers.size(); i++)
  {
    if (! timers[i].empty())
    {
      ss << setw(24) << left << names[i] << timers[i].str(prec);
      timerSum.accum(timers[i]);
    }
  }

  ss << string(60, '-') << "\n";
  ss << setw(24) << left << "THREAD SUM" << timerSum.str(prec) << "\n";

  // The last timer is presumed to be separate.
  ss << setw(24) << left << names.back() << timers.back().str(prec);

  ss << setw(24) << left << "RATIO" << 
    timers.back().strRatio(timerSum, prec) << "\n\n";

  return ss.str();
}


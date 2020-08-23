#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Timers.h"


Timers::Timers()
{
  Timers::reset();
}


Timers::~Timers()
{
}


void Timers::reset()
{
  timers.resize(TIMER_SIZE);
  names.resize(TIMER_SIZE);

  for (auto& t: timers)
    t.reset();
  
  names[TIMER_UNIQUE] = "Unique";
  names[TIMER_FILL_TREE] = "Fwd";
  names[TIMER_DERIVE_STRAT] = "Bwd";
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
    setw(10) << right << "Sum" << 
    setw(6) << right << "Count" << 
    setw(10) << right << "Average" << "\n";

  Timer timerSum;
  for (unsigned i = 0; i+1 < timers.size(); i++)
  {
    if (! timers[i].empty())
    {
      ss << setw(24) << left << names[i] << timers[i].str(prec);
      timerSum.accum(timers[i]);
    }
  }

  ss << string(50, '-') << "\n";
  ss << setw(24) << left << "THREAD SUM" << timerSum.str(prec) << "\n";

  // The last timer is presumed to be separate.
  ss << setw(24) << left << names.back() << timers.back().str(prec);

  ss << setw(24) << left << "RATIO" << 
    timers.back().strRatio(timerSum, prec) << endl << endl;

  return ss.str();
}


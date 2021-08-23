/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iomanip>
#include <sstream>

#include "Timer.h"


Timer::Timer()
{
  Timer::reset();
}


Timer::~Timer()
{
}


void Timer::reset()
{
  no = 0;
  sum = 0.;
  label = "";
}


void Timer::name(const string& nameIn)
{
  label = nameIn;
}


void Timer::start()
{
  begin = std::chrono::high_resolution_clock::now();
}


void Timer::stop()
{
  auto end = std::chrono::high_resolution_clock::now();
  auto delta = std::chrono::duration_cast<std::chrono::microseconds>
    (end - begin);

  no++;
  sum += static_cast<double>(delta.count());
}


void Timer::operator += (const Timer& timer2)
{
  no += timer2.no;
  sum += timer2.sum;
}


void Timer::accum(const Timer& timer2)
{
  if (no == 0)
    no += timer2.no;
  sum += timer2.sum;
}


bool Timer::empty() const
{
  return (no == 0);
}


string Timer::strNumber() const
{
  stringstream ss;
  ss << setw(10) << no;
  return ss.str();
}


string Timer::strRatio(
  const Timer& timerNum,
  const int prec) const
{
  stringstream ss;
  if (sum == 0.)
    ss << setw(10) << "-";
  else
    ss << setw(10) << fixed << setprecision(prec) <<
      timerNum.sum / sum << "  x";

  return ss.str();
}


string Timer::str(const int prec) const 
{
  if (no == 0)
    return "";

  stringstream ss;
  ss << setw(10) << fixed << setprecision(prec) << 
    sum / 1000. << " ms";

  ss << Timer::strNumber();

  if (no > 1)
    ss << setw(10) << sum / (1000. * no) << " ms";

  if (! label.empty())
    ss << "  " << left << label;

  return ss.str() + "\n";
}


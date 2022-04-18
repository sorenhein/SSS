/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "ExplStat.h"


ExplStat::ExplStat()
{
  ExplStat::reset();
}


void ExplStat::reset()
{
  lengths.clear();
}


void ExplStat::resize()
{
  lengths.resize(20, 0); // up to 20 explanations per strat
}


void ExplStat::incrLengths(const unsigned count)
{
  assert(count < lengths.size());
  lengths[count]++;
}


string ExplStat::strLengths() const
{
  stringstream ss;

  unsigned cumSum = 0;
  unsigned sum = 0;

  for (unsigned cno = 0; cno < lengths.size(); cno++)
  {
    const unsigned v = lengths[cno];
    if (v > 0)
    {
      ss << setw(3) << cno << setw(6) << v << "\n";
      sum += v;
      cumSum += v * cno;
    }
  }

  ss << string(9, '-') << "\n";

  ss << setw(3) << "Avg" <<
    setw(6) << fixed << setprecision(2) <<
    cumSum / static_cast<double>(sum) << "\n\n";
  
  return ss.str();
}


string ExplStat::str() const
{
  return ExplStat::strLengths();
}


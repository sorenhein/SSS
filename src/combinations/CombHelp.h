/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBHELP_H
#define SSS_COMBHELP_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

using namespace std;


struct CountEntry
{
  size_t count;
  size_t totalPlays;
  size_t totalStrats;

  void reset()
  {
    count = 0;
    totalPlays = 0;
    totalStrats = 0;
  }

  void operator += (const CountEntry& c2)
  {
    count += c2.count;
    totalPlays += c2.totalPlays;
    totalStrats += c2.totalStrats;
  }

  void incr(
    const size_t plays,
    const size_t strats)
  {
    count++;
    totalPlays += plays;
    totalStrats += strats;
  }

  string strAverage(const size_t num) const
  {
    if (count == 0)
      return "-";

    stringstream ss;
    ss << fixed << setprecision(2) << 
      static_cast<double>(num) / static_cast<double>(count);
    return ss.str();
  }

  string strAveragePlays() const
  {
    return strAverage(totalPlays);
  }

  string strAverageStrats() const
  {
    return strAverage(totalStrats);
  }
};


struct CombCountEntry
{
  vector<CountEntry> data;

  void operator += (const CombCountEntry& cce)
  {
    assert(data.size() == cce.data.size());
    for (unsigned n = 0; n < data.size(); n++)
      data[n] += cce.data[n];
  }
};

#endif

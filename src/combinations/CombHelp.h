/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBHELP_H
#define SSS_COMBHELP_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


struct CountEntry
{
  unsigned count;
  unsigned totalPlays;
  unsigned totalStrats;

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
    const unsigned plays,
    const unsigned strats)
  {
    count++;
    totalPlays += plays;
    totalStrats += strats;
  }

  string strAverage(const unsigned num) const
  {
    if (count == 0)
      return "-";

    stringstream ss;
    ss << fixed << setprecision(2) << num / static_cast<double>(count);
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


struct CountPartition
{
  vector<CountEntry> elements;

  void resize(const unsigned len)
  {
    elements.clear();
    elements.resize(len);
  }

  void operator += (const CountPartition& cp)
  {
    for (unsigned n = 0; n < elements.size(); n++)
      elements[n] += cp.elements[n];
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

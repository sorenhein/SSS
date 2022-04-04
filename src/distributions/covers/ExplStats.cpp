/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "ExplStats.h"
#include "CoverSpec.h"


ExplStats::ExplStats()
{
  ExplStats::reset();
}


void ExplStats::reset()
{
  explStats.clear();
}


void ExplStats::resize(const vector<vector<unsigned>>& counts)
{
  // specs is just used for its two dimensions

  const unsigned ssize = counts.size();
  explStats.resize(ssize);

  for (unsigned s = 0; s < ssize; s++)
  {
    const unsigned s2size = counts[s].size();
    explStats[s].resize(s2size);

    for (unsigned t = 0; t < s2size; t++)
    {
      ExplStat& explStat = explStats[s][t];
      explStat.resize(counts[s][t]);
    }
  }
}


ExplStat& ExplStats::getEntry(
  const unsigned char lengthIndex,
  const unsigned char tops1Index)
{
  assert(lengthIndex < explStats.size());
  assert(tops1Index < explStats[lengthIndex].size());
  return explStats[lengthIndex][tops1Index];
}


string ExplStats::str() const
{
  stringstream ss;

  for (unsigned length = 2; length < explStats.size(); length++)
  {
    for (unsigned tops1 = 1; tops1 < explStats[length].size(); tops1++)
    {
      const ExplStat& explStat = explStats[length][tops1];
      if (explStat.empty())
        continue;

      ss << "Cover counts " << length << "-" << tops1 << "\n";
      ss << explStat.str();
    }
  }

  return ss.str();
}


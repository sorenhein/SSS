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

#include "../../const.h"


// Not very scientific, but enough for the largest one in Manual.
#define EXPL_STATS_MAX 50


ExplStats::ExplStats()
{
  ExplStats::resize();
}


void ExplStats::resize()
{
  explStats.resize(MAX_CARDS);

  for (unsigned s = 0; s < MAX_CARDS; s++)
  {
    explStats[s].resize(MAX_CARDS);

    for (unsigned t = 0; t < MAX_CARDS; t++)
    {
      ExplStat& explStat = explStats[s][t];
      explStat.resize();
    }
  }

  lengthMax = 0;
  topsMax.resize(MAX_CARDS, 0);
}


ExplStat& ExplStats::getEntry(
  const unsigned char lengthIndex,
  const unsigned char tops1Index)
{
  assert(lengthIndex < explStats.size());
  assert(tops1Index < explStats[lengthIndex].size());

  if (lengthIndex > lengthMax)
    lengthMax = lengthIndex;

  if (tops1Index > topsMax[lengthIndex])
    topsMax[lengthIndex] = tops1Index;

  return explStats[lengthIndex][tops1Index];
}


string ExplStats::str() const
{
  stringstream ss;

  for (unsigned length = 2; length <= lengthMax; length++)
  {
    for (unsigned tops1 = 1; tops1 <= topsMax[length]; tops1++)
    {
      const ExplStat& explStat = explStats[length][tops1];
      // if (explStat.empty())
        // continue;

      ss << "Cover counts " << length << "-" << tops1 << "\n";
      ss << explStat.str();
    }
  }

  return ss.str();
}


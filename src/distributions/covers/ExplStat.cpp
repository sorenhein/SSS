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
  singles.clear();
  pairs.clear();
}


void ExplStat::resize(const unsigned pairSize)
{
  lengths.resize(20, 0); // up to 20 explanations per strat
  singles.resize(pairSize, 0);

  pairs.resize(pairSize);

  for (unsigned c = 0; c < pairSize; c++)
    pairs[c].resize(pairSize, 0);
}


bool ExplStat::empty() const
{
  return lengths.empty();
}


void ExplStat::incrLengths(const unsigned count)
{
  assert(count < lengths.size());
  lengths[count]++;
}


void ExplStat::incrSingles(const unsigned index)
{
  assert(index < singles.size());
  singles[index]++;
}


void ExplStat::incrPairs(
  const unsigned index1,
  const unsigned index2)
{
  assert(index1 < pairs.size());
  assert(index2 < pairs.size());

  pairs[index1][index2]++;
  pairs[index2][index1]++;
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


string ExplStat::strPairs() const
{
  stringstream ss;
  ss << "Pairs\n";

  ss << setw(3) << "#" << " | ";
  for (unsigned cno = 0; cno < pairs.size(); cno++)
    ss << setw(5) << cno;
  ss << " | " << setw(5) << "Sing" << "\n";
  ss << string(3 + 3 + 5 * pairs.size() + 3 + 5, '-') << "\n";

  for (unsigned cno = 0; cno < pairs.size(); cno++)
  {
    ss << setw(3) << cno << " | ";
    for (unsigned cno2 = 0; cno2 < pairs.size(); cno2++)
    {
      const unsigned v = pairs[cno][cno2];
      ss << setw(5) << (v == 0 ? "-" : to_string(v));
    }
    ss << " | " << setw(5) << singles[cno] << "\n";
  }

  return ss.str() + "\n";
}


string ExplStat::str() const
{
  return ExplStat::strLengths() + ExplStat::strPairs();
}


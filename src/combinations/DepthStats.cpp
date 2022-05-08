/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "DepthStats.h"

mutex mtxDepthStats;


DepthStats::DepthStats()
{
  stats.clear();
}


void DepthStats::resize(const unsigned len)
{
  // This is too large, as we only need the maximum strategy depth,
 // but it's a relatively small array.
 stats.resize(len);

 for (unsigned char cards = 0; cards < len; cards++)
   stats[cards].resize(len, 0);
}


void DepthStats::increment(
  const unsigned nominalDepth,
  const unsigned actualDepth)
{
  assert(nominalDepth < stats.size());
  assert(actualDepth < stats[nominalDepth].size());

  mtxDepthStats.lock();
  stats[nominalDepth][actualDepth]++;
  mtxDepthStats.unlock();
}


DepthStats& DepthStats::operator += (const DepthStats& ds2)
{
  for (unsigned nominal = 0; nominal < stats.size(); nominal++)
    for (unsigned actual = 0; actual < stats[nominal].size(); actual++)
      stats[nominal][actual] += ds2.stats[nominal][actual];
  
  return * this;
}


unsigned DepthStats::size() const
{
  return stats.size();
}


void DepthStats::makeMarginals(
  vector<unsigned>& sumNominal,
  vector<unsigned>& sumActual,
  unsigned& maxNominal,
  unsigned& maxActual) const
{
  // Make the marginal sums.
  sumNominal.resize(stats.size());
  sumActual.resize(stats.size());
  maxNominal = 0;
  maxActual = 0;

  for (unsigned nominal = 0; nominal < stats.size(); nominal++)
  {
    for (unsigned actual = 0; actual < stats[nominal].size(); actual++)
    {
assert(nominal < sumNominal.size());
assert(actual < sumActual.size());
      const unsigned v = stats[nominal][actual];
      sumNominal[nominal] += v;
      sumActual[actual] += v;

      if (v > 0)
      {
        if (nominal > maxNominal)
          maxNominal = nominal;
        if (actual > maxActual)
          maxActual = actual;
      }
    }
  }
}


string DepthStats::strHeader(const unsigned maxActual) const
{
  stringstream ss;

  ss << setw(10) << "nom | act";
  for (unsigned i = 0; i <= maxActual; i++)
    ss << setw(6) << i;
  ss << setw(8) << "Sum";
  ss << "\n";
  ss << string(18 + 6 * (maxActual+1), '-');

  return ss.str() + "\n";
}


string DepthStats::str() const
{
  // Make the marginal sums.
  vector<unsigned> sumNominal;
  vector<unsigned> sumActual;
  unsigned maxNominal;
  unsigned maxActual;
  DepthStats::makeMarginals(sumNominal, sumActual, maxNominal, maxActual);

  stringstream ss;
  ss << "Strategy depth statistics\n\n";

  ss << DepthStats::strHeader(maxActual);

  for (unsigned nominal = 0; nominal <= maxNominal; nominal++)
  {
    ss << setw(10) << nominal;
    for (unsigned actual = 0; actual <= maxActual; actual++)
    {
      const unsigned v = stats[nominal][actual];
      if (v > 0)
        ss << setw(6) << v;
      else
        ss << setw(6) << "-";

      ss << setw(8) << sumNominal[nominal] << "\n";
    }
    ss << string(18 + 6 * (maxActual+1), '-') << "\n";
  }

  ss << setw(10) << "Sum";
  for (unsigned actual = 0; actual <= maxActual; actual++)
  {
    const unsigned v = sumActual[actual];
    if (v > 0)
      ss << setw(6) << v;
    else
      ss << setw(6) << "-";

    ss << "\n";
  }

  return ss.str() + "\n\n";
}


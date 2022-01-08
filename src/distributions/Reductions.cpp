/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Reductions.h"
#include "DistHelp.h"

/*
   Unlike Survivors, Reductions operate within the same trick.
   They are used when there is a rank collapse below a certain
   declarer card, which will cover entire EW ranks (and not only
   parts of a rank).

   We pre-calculate a table that makes the lookup faster.

   All ranks in Reductions are minimal, canonical ranks.
 */


Reductions::Reductions()
{
}


bool Reductions::sameReduction(
  const DistInfo& dist0,
  const DistInfo& dist1,
  const unsigned reducedRankSize,
  const unsigned reducer) const
{
  for (unsigned rank = reducedRankSize; rank-- > 0; )
  {
    if (rank <= reducer)
    {
      // We have run out of ranks that still count.
      return true;
    }

    if (dist0.west.counts[rank] != dist1.west.counts[rank])
    {
      // If there is a difference in counts in the part of the
      // distribution that still counts (> reducer), it is a new
      // distribution in this sense.
      return false;
    }
  }
  return true;
}


void Reductions::set(
  const vector<DistInfo>& distributions,
  const unsigned reducedRankSize)
{
  reductions.clear();

  if (distributions.empty())
  {
    // Trivial version.
    reductions.resize(1);
    reductions[0].setTrivial();
    return;
  }

  reductions.resize(reducedRankSize);

  for (unsigned reducer = 0; reducer < reducedRankSize; reducer++)
  {
    auto& dist = reductions[reducer].full2reducedDist;
    dist.resize(distributions.size());

    vector<unsigned char> seen(distributions.size(), 0);
    unsigned char nextReducedDist = 0;

    for (unsigned d = 0; d < distributions.size(); d++)
    {
      if (seen[d])
        continue;

      const unsigned dlen = distributions[d].west.len;
      dist[d] = nextReducedDist;

      for (unsigned dnext = d+1; dnext < distributions.size(); dnext++)
      {
        if (seen[dnext])
          continue;
        else if (distributions[dnext].west.len != dlen)
          break;
        else if (Reductions::sameReduction(
            distributions[d], distributions[dnext], 
            reducedRankSize, reducer))
        {
          seen[dnext] = 1;
          dist[dnext] = nextReducedDist;
        }
      }
      nextReducedDist++;
    }
  }
}


const Reduction& Reductions::get(const unsigned rank) const
{
if (rank >= reductions.size())
{
  cout << "rank " << rank << endl;
  cout << "red.size " << reductions.size() << endl;
}
  assert(rank < reductions.size());
  return reductions[rank];
}


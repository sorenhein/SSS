/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

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


void Reductions::set(
  const vector<DistInfo>& distributions,
  const unsigned reducedRankSize)
{
  reductions.clear();
  reductions.resize(reducedRankSize);

  for (unsigned reducer = 0; reducer < reducedRankSize; reducer++)
  {
    bool firstFlag = true;
    unsigned char nextReducedDist = 0;
    auto& dist = reductions[reducer].full2reducedDist;
    dist.resize(distributions.size());

    for (unsigned d = 0; d < distributions.size(); d++)
    {
      if (firstFlag)
      {
        dist[d] = nextReducedDist;
        firstFlag = false;
        continue;
      }

      bool changeFlag = false;

      // It's enough to look at West.
      if (distributions[d].west.len != distributions[d-1].west.len)
        changeFlag = true;
      else
      {
        for (unsigned rank = reducedRankSize; rank-- > 0; )
        {
          if (rank <= reducer)
          {
            // We have run out of ranks that still count.
            break;
          }

          if (distributions[d].west.counts[rank] !=
              distributions[d-1].west.counts[rank])
          {
            // If there is a difference in counts in the part of the
            // distribution that still counts (> reducer), it is a new
            // distribution in this sense.
            changeFlag = true;
            break;
          }
        }
      }

      if (changeFlag)
        nextReducedDist++;

      dist[d] = nextReducedDist;
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


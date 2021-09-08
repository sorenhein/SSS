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

  for (unsigned reducer = 1; reducer < reducedRankSize; reducer++)
  {
    bool firstFlag = true;
    unsigned char nextReducedDist = 0;
    Reduction& reduction = reductions[reducer];
    reduction.resize(distributions.size());

    for (unsigned d = 0; d < distributions.size(); d++)
    {
      if (firstFlag)
      {
        reduction[d] = nextReducedDist++;
        firstFlag = false;
        continue;
      }

      bool changeFlag = false;
      for (unsigned rank = reducedRankSize; rank-- > 0; )
      {
        if (rank <= reducer)
        {
          // We have run out of ranks that still count.
          break;
        }

        if (distributions[d].west.counts[rank] !=
            distributions[d-1].west.counts[rank] ||
            distributions[d].east.counts[rank] !=
            distributions[d-1].east.counts[rank])
        {
          // If there is a difference in counts in the part of the
          // distribution that still counts (> reducer), it is a new
          // disitrbution in this sense.
          changeFlag = true;
          break;
        }
      }

      reduction[d] = nextReducedDist;

      if (changeFlag)
        nextReducedDist++;
    }
  }
}


const Reduction& Reductions::get(const unsigned rank) const
{
  assert(rank < reductions.size());
  return reductions[rank];
}


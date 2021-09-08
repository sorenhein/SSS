/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Survivors.h"

#include "../plays/Play.h"

/*
   There are several cases to consider:
   - West or East is void, and only one distribution survives.
   - No collapse occurs.
   - One collapse occurs.
   - Two collapses occur.

   We pre-calculate several tables that make the lookup faster.

   All ranks in Survivors are minimal, canonical ranks.
 */


Survivors::Survivors()
{
  rankSize = 0;
}


void Survivors::setGlobal(const unsigned rankSizeIn)
{
  rankSize = rankSizeIn;
}


void Survivors::setSurvivorsVoid(const vector<DistInfo>& distributions)
{
  assert(distributions.size() > 0);

  // West void.
  distSurvivorsWestVoid.setVoid(0);
  assert(distributions[0].west.len == 0);

  // East void.
  const unsigned char dlast =
    static_cast<unsigned char>(distributions.size() - 1);

  distSurvivorsEastVoid.setVoid(dlast);
  assert(distributions[dlast].east.len == 0);
}


void Survivors::setSurvivorsGeneral(const vector<DistInfo>& distributions)
{
  // Make the survivors in the absence of rank collapses.
  distSurvivors.setGeneral(distributions, rankSize);
}


void Survivors::precalcSurvivorsCollapse1(
  const vector<DistInfo>& distributions,
  vector<vector<SideInfo>>& distCollapses1)
{
  const unsigned dlast = distributions.size() - 1;
  distCollapses1.resize(rankSize);
  for (unsigned c1 = 1; c1 < rankSize; c1++)
    distCollapses1[c1].resize(dlast);

  for (unsigned d = 1; d < dlast; d++)
  {
    const DistInfo& dist = distributions[d];
    for (unsigned c1 = 1; c1 < rankSize; c1++)
    {
      distCollapses1[c1][d] = dist.west;
      distCollapses1[c1][d].collapse1(c1);
    }
  }
}


void Survivors::precalcSurvivorsCollapse2(
  const vector<DistInfo>& distributions,
  const vector<vector<SideInfo>>& distCollapses1,
  vector<vector<vector<SideInfo>>>& distCollapses2)
{
  const unsigned dlast = distributions.size() - 1;
  distCollapses2.resize(rankSize);
  for (unsigned c1 = 1; c1 < rankSize; c1++)
  {
    distCollapses2[c1].resize(rankSize);
    for (unsigned c2 = 1; c2 < rankSize; c2++)
      distCollapses2[c1][c2].resize(dlast);
  }

  for (unsigned c1 = 1; c1 < rankSize; c1++)
  {
    if (c1+1 < rankSize)
    {
      // Special case where two adjacent ranks are collapsed.
      for (unsigned d = 1; d < dlast; d++)
      {
        distCollapses2[c1][c1+1][d] = distCollapses1[c1][d];
        distCollapses2[c1][c1+1][d].collapse2(c1+1);
        distCollapses2[c1+1][c1][d] = distCollapses2[c1][c1+1][d];
      }
    }

    for (unsigned c2 = c1+2; c2 < rankSize; c2++)
    {
      // General case where the collapses are independent.
      for (unsigned d = 1; d < dlast; d++)
      {
        distCollapses2[c1][c2][d] = distCollapses1[c1][d];
        distCollapses2[c1][c2][d].collapse1(c2);
        distCollapses2[c2][c1][d] = distCollapses2[c1][c2][d];
      }
    }
  }
}


void Survivors::setSurvivors(const vector<DistInfo>& distributions)
{
  // The internal reduced ranks are 0-based.
  // The external full ranks are 1-based, and "0" means "void".
  // The survivors matrix is expressed in reduced ranks.
  // Therefore we need the voids separately.
  Survivors::setSurvivorsVoid(distributions);
  Survivors::setSurvivorsGeneral(distributions);

  // Pre-calculate the collapses of each distribution.
  vector<vector<SideInfo>> distCollapses1;
  Survivors::precalcSurvivorsCollapse1(distributions, distCollapses1);

  vector<vector<vector<SideInfo>>> distCollapses2;
  Survivors::precalcSurvivorsCollapse2(distributions, 
    distCollapses1, distCollapses2);

  // We collapse downward, so rank 1 always survives and rank 2
  // may collapse onto rank 1.
  distSurvivorsCollapse1.resize(rankSize);

  distSurvivorsCollapse2.resize(rankSize);
  for (unsigned c1 = 1; c1 < rankSize; c1++)
    distSurvivorsCollapse2[c1].resize(rankSize);

  // Make the rank collapses.
  for (unsigned w = 0; w < rankSize; w++)
  {
    for (unsigned e = 0; e < rankSize; e++)
    {
      for (unsigned c1 = 1; c1 < rankSize; c1++)
      {
        distSurvivorsCollapse1.matrix(c1).data[w][e].collapse(
          distCollapses1[c1],
          distSurvivors.data[w][e]);

        for (unsigned c2 = c1+1; c2 < rankSize; c2++)
        {
          distSurvivorsCollapse2[c1].matrix(c2).data[w][e].collapse(
            distCollapses2[c1][c2],
            distSurvivorsCollapse1.matrix(c1).data[w][e]);

          // Could perhaps order c1 and c2 when calling,
          // such that we only need half the matrix.
          distSurvivorsCollapse2[c2].matrix(c1).data[w][e] =
            distSurvivorsCollapse2[c1].matrix(c2).data[w][e];
        }
      }
    }
  }
}


const SurvivorList& Survivors::getSurvivors(const SurvivorControl& sc) const
{
  if (sc.westVoidFlag)
  {
    return distSurvivorsWestVoid;
  }
  else if (sc.eastVoidFlag)
  {
    return distSurvivorsEastVoid;
  }
  else if (sc.collapseLeadFlag && sc.collapsePardFlag)
  {
    assert(sc.collapseLead >= 1);
    assert(sc.collapsePard >= 1);

    const SurvivorMatrix& sm = 
      distSurvivorsCollapse2[sc.collapsePard].matrix(sc.collapseLead);

    return sm.data[sc.westRank][sc.eastRank];
  }
  else if (sc.collapseLeadFlag)
  {
    assert(sc.collapseLead >= 1);

    const SurvivorMatrix& sm = 
      distSurvivorsCollapse1.matrix(sc.collapseLead);

    return sm.data[sc.westRank][sc.eastRank];
  }
  else if (sc.collapsePardFlag)
  {
    assert(sc.collapsePard >= 1);

    const SurvivorMatrix& sm = 
      distSurvivorsCollapse1.matrix(sc.collapsePard);

    return sm.data[sc.westRank][sc.eastRank];
  }
  else
  {
    return distSurvivors.data[sc.westRank][sc.eastRank];
  }
}


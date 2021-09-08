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

// TODO Set rankSize once and for all, don't keep passing around


Survivors::Survivors()
{
  rankSize = 0;
  full2reducedPtr = nullptr;
}


void Survivors::setGlobal(
  const unsigned rankSizeIn,
  const vector<unsigned>& full2reduced)
{
  rankSize = rankSizeIn;
  full2reducedPtr = &full2reduced;
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
  // {
    distSurvivorsCollapse2[c1].resize(rankSize);
    // for (unsigned c2 = 1; c2 < rankSize; c2++)
      // distSurvivorsCollapse2[c1][c2].resize(rankSize);
  // }

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
          // distSurvivorsCollapse2[c1][c2].data[w][e].collapse(
          distSurvivorsCollapse2[c1].matrix(c2).data[w][e].collapse(
            distCollapses2[c1][c2],
            distSurvivorsCollapse1.matrix(c1).data[w][e]);

          // TODO Could perhaps order c1 and c2 when calling,
          // such that we only need half the matrix.
          distSurvivorsCollapse2[c2].matrix(c1).data[w][e] =
            distSurvivorsCollapse2[c1].matrix(c2).data[w][e];
        }
      }
    }
  }
}


const SurvivorList& Survivors::survivorsUncollapsed(
  const unsigned westRankReduced,
  const unsigned eastRankReduced) const
{
  // This method uses full (externally visible) ranks.
  // assert(westRank != 0 || eastRank != 0);
  // assert(westRank < full2reducedPtr->size());
  // assert(eastRank < full2reducedPtr->size());

  if (westRankReduced == 0)
    return Survivors::survivorsWestVoid();
  else if (eastRankReduced == 0)
    return Survivors::survivorsEastVoid();
  else
    return Survivors::survivorsReduced(
      westRankReduced,
      eastRankReduced);
      // (* full2reducedPtr)[westRank],
      // (* full2reducedPtr)[eastRank]);
}


const SurvivorList& Survivors::survivorsCollapse1(
  const unsigned westRankReduced,
  const unsigned eastRankReduced,
  const unsigned collapseReduced) const
{
  // This method uses full (externally visible) ranks.
  // assert(westRank != 0 || eastRank != 0);
  // assert(westRank < full2reducedPtr->size());
  // assert(eastRank < full2reducedPtr->size());

  if (westRankReduced == 0)
  {
    return Survivors::survivorsWestVoid();
  }
  else if (eastRankReduced == 0)
  {
    return Survivors::survivorsEastVoid();
  }
  // else if (collapse1 <= 1 || collapse1 >= full2reduced.size())
  else if (collapseReduced <= 1)
  {
    // Not really collapsed
    return Survivors::survivorsReduced(
      westRankReduced,
      eastRankReduced);
      // (* full2reducedPtr)[westRank],
      // (* full2reducedPtr)[eastRank]);
  }
  else
  {
    // Regular collapse
    return Survivors::survivorsReducedCollapse1(
      westRankReduced,
      eastRankReduced,
      collapseReduced);
      // (* full2reducedPtr)[westRank],
      // (* full2reducedPtr)[eastRank],
      // (* full2reducedPtr)[collapse1]);
  }
}

const SurvivorList& Survivors::survivorsCollapse2(
  const unsigned westRankReduced,
  const unsigned eastRankReduced,
  const unsigned collapsePardReduced,
  const unsigned collapseLeadReduced) const
  // const unsigned collapse1,
  // const unsigned collapse2) const
{
  // This method uses full (externally visible) ranks.
  // assert(westRank != 0 || eastRank != 0);
  // assert(westRank < full2reducedPtr->size());
  // assert(eastRank < full2reducedPtr->size());

  if (westRankReduced == 0)
    return Survivors::survivorsWestVoid();
  else if (eastRankReduced == 0)
    return Survivors::survivorsEastVoid();
  // else if (collapse1 <= 1)
  else if (collapsePardReduced <= 1)
  {
// TODO Do these discards ever happen?  Do we have to test for them?
// Can we avoid them in Ranks.cpp?
    if (collapseLeadReduced <= 1 || collapsePardReduced == collapseLeadReduced)
    {
      // Discarding collapse2
      return Survivors::survivorsReduced(
        westRankReduced,
        eastRankReduced);
        // (* full2reducedPtr)[westRank],
        // (* full2reducedPtr)[eastRank]);
    }
    else
      return Survivors::survivorsReducedCollapse1(
        westRankReduced,
        eastRankReduced,
        collapseLeadReduced);
        // (* full2reducedPtr)[westRank],
        // (* full2reducedPtr)[eastRank],
        // (* full2reducedPtr)[collapse2]);
  }
  else if (collapseLeadReduced <= 1)
  {
    // Discarding collapse2
      return Survivors::survivorsReducedCollapse1(
        westRankReduced,
        eastRankReduced,
        collapsePardReduced);
        // (* full2reducedPtr)[westRank],
        // (* full2reducedPtr)[eastRank],
        // (* full2reducedPtr)[collapse1]);
  }
  else
  {
    // Discarding nothing
    return Survivors::survivorsReducedCollapse2(
        westRankReduced,
        eastRankReduced,
        collapsePardReduced,
        collapseLeadReduced);
      // (* full2reducedPtr)[westRank],
      // (* full2reducedPtr)[eastRank],
      // (* full2reducedPtr)[collapse1],
      // (* full2reducedPtr)[collapse2]);
  }
}


const SurvivorList& Survivors::survivorsWestVoid() const
{
  return distSurvivorsWestVoid;
}


const SurvivorList& Survivors::survivorsEastVoid() const
{
  return distSurvivorsEastVoid;
}


const SurvivorList& Survivors::survivorsReduced(
  const unsigned westRank,
  const unsigned eastRank) const
{
  assert(westRank < rankSize);
  assert(eastRank < rankSize);

  return distSurvivors.data[westRank][eastRank];
}


const SurvivorList& Survivors::survivorsReducedCollapse1(
  const unsigned westRank,
  const unsigned eastRank,
  const unsigned collapse1) const
{
  assert(collapse1 >= 1 && collapse1 < rankSize);
  assert(westRank < rankSize);
  assert(eastRank < rankSize);

  return distSurvivorsCollapse1.matrix(collapse1).data[westRank][eastRank];
}


const SurvivorList& Survivors::survivorsReducedCollapse2(
  const unsigned westRank,
  const unsigned eastRank,
  const unsigned collapse1,
  const unsigned collapse2) const
{
  assert(collapse1 >= 1 && collapse1 < distSurvivorsCollapse2.size());
  const SurvivorMatrix& sm = 
    distSurvivorsCollapse2[collapse1].matrix(collapse2);

  assert(westRank < rankSize);
  assert(eastRank < rankSize);
  return sm.data[westRank][eastRank];
}


const SurvivorList& Survivors::getSurvivors(
  const Play& play,
  const unsigned westRankReduced,
  const unsigned eastRankReduced,
  const unsigned collapseLeadReduced,
  const unsigned collapsePardReduced) const
{
  /*
  unsigned westRank, eastRank;
  if (play.side == SIDE_NORTH)
  {
    westRank = play.rho();
    eastRank = play.lho();
  }
  else
  {
    westRank = play.lho();
    eastRank = play.rho();
  }
  */

  if (westRankReduced == 0 || eastRankReduced == 0)
    return Survivors::survivorsUncollapsed(westRankReduced, eastRankReduced);
  else if (play.leadCollapse && play.pardCollapse)
  {
    return Survivors::survivorsCollapse2(
      westRankReduced, eastRankReduced, 
      collapsePardReduced, collapseLeadReduced);
      // westRankReduced, eastRankReduced, play.pard()+1, play.lead()+1);
  }
  else if (play.leadCollapse)
  {
    return Survivors::survivorsCollapse1(
      westRankReduced, eastRankReduced, collapseLeadReduced);
      // westRank, eastRank, play.lead()+1);
  }
  else if (play.pardCollapse)
  {
    return Survivors::survivorsCollapse1(
      westRankReduced, eastRankReduced, collapsePardReduced);
      // westRank, eastRank, play.pard()+1);
  }
  else
    return Survivors::survivorsUncollapsed(westRankReduced, eastRankReduced);
}


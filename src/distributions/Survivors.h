/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SURVIVORS_H
#define SSS_SURVIVORS_H

#include <vector>

#include "DistHelp.h"

struct Play;


using namespace std;


struct SurvivorControl
{
  // The ranks are all reduced, as they control the survivors of a
  // canonical distribution.
  
  unsigned westRank;
  unsigned eastRank;
  bool westVoidFlag;
  bool eastVoidFlag;
  unsigned collapseLead;
  unsigned collapsePard;
  bool collapseLeadFlag;
  bool collapsePardFlag;
};


class Survivors
{
  private:

    unsigned rankSize;

    // vector<unsigned> const * full2reducedPtr;

    SurvivorMatrix distSurvivors;

    SurvivorsCollapsed distSurvivorsCollapse1;

    vector<SurvivorsCollapsed> distSurvivorsCollapse2;

    SurvivorList distSurvivorsWestVoid;

    SurvivorList distSurvivorsEastVoid;


    void setSurvivorsVoid(const vector<DistInfo>& distributions);

    void setSurvivorsGeneral(const vector<DistInfo>& distributions);

    void precalcSurvivorsCollapse1(
      const vector<DistInfo>& distributions,
      vector<vector<SideInfo>>& distCollapse1);

    void precalcSurvivorsCollapse2(
      const vector<DistInfo>& distributions,
      const vector<vector<SideInfo>>& distCollapse1,
      vector<vector<vector<SideInfo>>>& distCollapse2);

    const SurvivorList& survivorsWestVoid() const;

    const SurvivorList& survivorsEastVoid() const;

    const SurvivorList& survivorsReduced(
      const unsigned westRank,
      const unsigned eastRank) const;

    const SurvivorList& survivorsReducedCollapse1(
      const unsigned westRank,
      const unsigned eastRank,
      const unsigned collapse1) const;

    const SurvivorList& survivorsReducedCollapse2(
      const unsigned westRank,
      const unsigned eastRank,
      const unsigned collapse1,
      const unsigned collapse2) const;


  public:

    Survivors();

    void setGlobal(const unsigned rankSize);

    void setSurvivors(const vector<DistInfo>& distributions);

    const SurvivorList& getSurvivors(const SurvivorControl& sc) const;
};

#endif

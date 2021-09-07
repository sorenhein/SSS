/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTRIBUTION_H
#define SSS_DISTRIBUTION_H

#include <vector>
#include <string>

#include "SurvivorList.h"
#include "DistHelp.h"


struct Play;


using namespace std;


struct DistID
{
  unsigned cards; // Smallest number of NS+EW cards needed
  unsigned holding; // Canonical EW holding
};


class Distribution
{
  private:

    unsigned cards;

    vector<unsigned> full2reduced;
    vector<unsigned> reduced2full;
    unsigned rankSize; // Reduced ranks

    SideInfo opponents;

    vector<DistInfo> distributions;

    Distribution const * distCanonical;

    SurvivorMatrix distSurvivors;
    SurvivorsCollapsed distSurvivorsCollapse1;
    vector<vector<SurvivorMatrix>> distSurvivorsCollapse2;
    SurvivorList distSurvivorsWestVoid;
    SurvivorList distSurvivorsEastVoid;


    void setBinomial();

    void setNames();

    void shrink(
      const unsigned maxFullRank,
      const unsigned maxReducedRank);

    void mirror(unsigned& distIndex);

    void setSurvivorsVoid();

    void setSurvivorsGeneral();

    void precalcSurvivorsCollapse1(
      vector<vector<SideInfo>>& distCollapse1);

    void precalcSurvivorsCollapse2(
      const vector<vector<SideInfo>>& distCollapse1,
      vector<vector<vector<SideInfo>>>& distCollapse2);

    void collapseSurvivors(
      const vector<SideInfo>& distCollapses,
      const SurvivorList& survivorsUnreduced,
      SurvivorList& survivorsReduced);

    const SurvivorList& survivorsUncollapsed(
      const unsigned westRank,
      const unsigned eastRank) const;

    const SurvivorList& survivorsCollapse1(
      const unsigned westRank,
      const unsigned eastRank,
      const unsigned collapse1) const;

    const SurvivorList& survivorsCollapse2(
      const unsigned westRank,
      const unsigned eastRank,
      const unsigned collapse1,
      const unsigned collapse2) const;

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

    const SurvivorList& survivorsWestVoid() const;
    const SurvivorList& survivorsEastVoid() const;

    string strHeader() const;


  public:

    Distribution();

    void reset();

    void setRanks(
      const unsigned cards,
      const unsigned holding2); // Binary, not trinary format

    void split();
    void splitAlternative(); // Does the identical thing, maybe faster

    void setPtr(Distribution const * distCanonicalIn);

    Distribution const * getPtr() const;

    unsigned size() const;

    DistID getID() const;

    void setSurvivors();

    const SurvivorList& survivors(const Play& play) const;

    string str() const;

};

#endif

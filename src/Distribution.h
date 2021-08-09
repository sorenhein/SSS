#ifndef SSS_DISTRIBUTION_H
#define SSS_DISTRIBUTION_H

#include <vector>
#include <string>

#include "Survivor.h"

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

    #include "DistHelp.h"

    unsigned cards;

    vector<unsigned> full2reduced;
    vector<unsigned> reduced2full;
    unsigned rankSize; // Reduced ranks

    SideInfo opponents;

    vector<DistInfo> distributions;

    Distribution const * distCanonical;

    SurvivorMatrix distSurvivors;
    vector<SurvivorMatrix> distSurvivorsCollapse1;
    vector<vector<SurvivorMatrix>> distSurvivorsCollapse2;
    Survivors distSurvivorsWestVoid;
    Survivors distSurvivorsEastVoid;


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
      const Survivors& survivorsUnreduced,
      Survivors& survivorsReduced);

    const Survivors& survivorsReduced(
      const unsigned westRank,
      const unsigned eastRank) const;

    const Survivors& survivorsReducedCollapse1(
      const unsigned westRank,
      const unsigned eastRank,
      const unsigned collapse1) const;

    const Survivors& survivorsReducedCollapse2(
      const unsigned westRank,
      const unsigned eastRank,
      const unsigned collapse1,
      const unsigned collapse2) const;

    const Survivors& survivorsWestVoid() const;
    const Survivors& survivorsEastVoid() const;

    string strHeader() const;


  public:

    Distribution();

    ~Distribution();

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

    const Survivors& survivorsUncollapsed(
      const unsigned westRank,
      const unsigned eastRank) const;

    const Survivors& survivorsCollapse1(
      const unsigned westRank,
      const unsigned eastRank,
      const unsigned collapse1) const;

    const Survivors& survivorsCollapse2(
      const unsigned westRank,
      const unsigned eastRank,
      const unsigned collapse1,
      const unsigned collapse2) const;

    const Survivors& survivors(const Play& play) const;

    string str() const;

};

#endif

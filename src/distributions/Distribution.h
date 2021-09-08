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
#include "Survivors.h"
#include "Reductions.h"
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

    Survivors survivors;

    Reductions reductions;


    void setBinomial();

    void setNames();

    void shrink(
      const unsigned maxFullRank,
      const unsigned maxReducedRank);

    void mirror(unsigned& distIndex);

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

    void setLookups();

    const SurvivorList& getSurvivors(const Play& play) const;

    const Reduction& getReduction(const unsigned char rankNS) const;

    string str() const;

};

#endif

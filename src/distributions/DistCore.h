/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTCORE_H
#define SSS_DISTCORE_H

#include <vector>
#include <string>

#include "SurvivorList.h"
#include "Survivors.h"
#include "Reductions.h"
#include "DistHelp.h"

class DistMap;
struct Play;

using namespace std;


class DistCore
{
  private:

    unsigned cards;
    unsigned rankSize;

    vector<DistInfo> distributions;

    Survivors survivors;

    Reductions reductions;


    void setBinomial();

    void setNames();

    void mirror(
      const DistMap& distMap,
      unsigned& distIndex);

    string strHeader() const;


  public:

    DistCore();

    void reset();

    // The alternative does the identical thing, maybe faster
    void split(const DistMap& distMap);
    void splitAlternative(const DistMap& distMap); 

    unsigned size() const;

    void setLookups();

    const SurvivorList& getSurvivors(
      const DistMap& distMap,
      const Play& play) const;

    const Reduction& getReduction(
      const DistMap& distMap,
      const unsigned char rankNS) const;

    string str() const;

};

#endif

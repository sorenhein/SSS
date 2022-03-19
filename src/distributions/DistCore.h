/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTCORE_H
#define SSS_DISTCORE_H

#include <vector>
#include <string>

#include "DistHelp.h"
#include "Survivors.h"
#include "Reductions.h"
#include "covers/Covers.h"

class SurvivorList;
class DistMap;
class CoverMemory;
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

    Covers covers;


    void setBinomial();

    void setNames();

    void mirror(
      const DistMap& distMap,
      unsigned& distIndex);

    void getCoverData(
      vector<ProductProfile>& distProfiles,
      // vector<unsigned char>& lengths,
      // vector<unsigned char>& tops,
      vector<unsigned char>& cases,
      unsigned char& maxLength,
      unsigned char& maxTops) const;

    void getCoverDataNew(
      vector<ProductProfile>& distProfiles,
      // vector<unsigned char>& lengths,
      // vector<vector<unsigned> const *>& topPtrs,
      vector<unsigned char>& cases,
      ProductProfile& sumProfile) const;
      // unsigned char& maxLength,
      // vector<unsigned char>& topTotals) const;

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
    
    void prepareCovers(const CoverMemory& coverMemory);

    // TODO const once specs are separate
    Covers& getCovers();

    void getCoverCounts(
      unsigned& numTableaux,
      unsigned& numUses) const;

    string str() const;

    string strCovers() const;
};

#endif

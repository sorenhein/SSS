/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERS_H
#define SSS_COVERS_H

#include <list>
#include <vector>
#include <string>

#include "product/Profile.h"

#include "CoverStore.h"
#include "TableauCache.h"
#include "CoverRow.h"


class ProductMemory;
class CoverTableau;
class Tricks;
class Result;


using namespace std;


class Covers
{
  private:

    Profile sumProfile;

    vector<unsigned char> cases;

    CoverStore store;

    TableauCache tableauCache;

    // These are the manually set rows.  They are currently inactive.
    list<CoverRow> rows;


    void setup(
      const list<Result>& results,
      Tricks& tricks,
      unsigned char& tricksMin) const;


  public:

    Covers();

    void reset();

    void prepare(
      ProductMemory& productMemory,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& casesIn,
      const Profile& sumProfileIn);

    CoverRow& addRow();

    void sortRows();

    const Cover& lookup(const Cover& cover) const;

    void explain(
      const list<Result>& results,
      const unsigned numStrategyTops,
      CoverTableau& tableau,
      bool& newTableauFlag);

    CoverState explainManually(
      const list<Result>& results,
      CoverTableau& tableau) const;

    void storeTableau(
      const Tricks& excessTricks,
      const CoverTableau& tableau);

    bool lookupTableau(
      const Tricks& excessTricks,
      CoverTableau const * tableauPtr);

    const Profile& getSumProfile() const;

    void getCoverCounts(
      unsigned& numTableaux,
      unsigned& numUses) const;

    string strCached() const;
};

#endif

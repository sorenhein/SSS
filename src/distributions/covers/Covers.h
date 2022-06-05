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

#include "CoverStore.h"
#include "RowStore.h"
#include "Explain.h"

#include "tableau/TableauCache.h"

#include "product/Profile.h"


class ProductMemory;
class Explain;
class CoverTableau;
class Tricks;
class Result;
template<typename T> class CoverStack;


using namespace std;


class Covers
{
  private:

    vector<unsigned char> cases;

    CoverStore coverStore;

    // This is the cache corresponding to the algorithmic covers.
    TableauCache tableauCache;

    // These are the manually set rows
    RowStore rowStore;

    // This is the separate cache corresponding to the rows.
    TableauCache tableauRowCache;

    Profile sumProfile;


    void fillStore(
      ProductMemory& productMemory,
      const vector<Profile>& distProfiles,
      ProfilePair& running);

    template<class C, class T>
    void explainTemplate(
      const Tricks& tricks,
      // const unsigned char tmin,
      const Explain& explain,
      // const unsigned numStrategyTops,
      const C& candidates,
      const size_t pruneTrigger,
      const size_t pruneSize,
      CoverStack<T>& stack,
      CoverTableau& solution);

    void partitionResults(
      const list<Result>& results,
      list<Result>& resultsSymm,
      list<Result>& resultsAsymm,
      // unsigned char& tmin,
      Explain& explain) const;


  public:

    Covers();

    void reset();

    void prepare(
      ProductMemory& productMemory,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& casesIn,
      const Profile& sumProfileIn);

    void addDirectly(list<Cover const *>& coverPtrs);

    const Cover& lookup(const Cover& cover) const;

    void explain(
      const list<Result>& results,
      const unsigned numStrategyTops,
      CoverTableau& solution,
      bool& newTableauFlag);

    void explainManually(
      const list<Result>& results,
      CoverTableau& solution);

    const Profile& getSumProfile() const;

    void getCoverCounts(
      size_t& numTableaux,
      size_t& numUses) const;

    string strCache() const;

    string strSignature() const;
};

#endif

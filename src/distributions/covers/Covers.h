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
class RanksNames;
class Tricks;
class Result;
struct HeavyData;
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
      const Explain& explain,
      const C& candidates,
      const bool partialTableauFlag,
      const size_t pruneTrigger,
      const size_t pruneSize,
      CoverStack<T>& stack,
      CoverTableau& solution);

    void explainByCategory(
      const Tricks& tricks,
      const Explain& explain,
      const bool partialTableauFlag,
      CoverTableau& solution,
      bool& newTableauFlag);

    void guessStart(
      const Tricks& tricks,
      CoverTableau& partialSolution,
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

    string strCache(const RanksNames& ranksNames) const;

    string strSignature() const;
};

#endif

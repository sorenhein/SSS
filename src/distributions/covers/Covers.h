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
#include "CoverRowOld.h"
#include "ProfilePair.h"
#include "TableauCache.h"

class CoverMemory;
class ProductMemory;
class CoverTableau;
class ResExpl;
class Tricks;
class Result;


using namespace std;


class Covers
{
  private:

    Profile sumProfile;

    list<CoverRowOld> rowsOld;

    CoverStore store;

    TableauCache tableauCache;


    void setup(
      const list<Result>& results,
      Tricks& tricks,
      unsigned char& tricksMin) const;

    void prune();

    string strDebug(
      const string& title,
      const Tricks& tricks) const;


  public:

    Covers();

    void reset();

    void prepareRow(
      const CoverSpec& coverSpec,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    void prepareRowNew(
      const list<Cover>& coverLits,
      const Profile& sumProfile,
      const unsigned indexIn,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    void prepareNew(
      ProductMemory& productMemory,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases,
      const Profile& sumProfileIn);

    void sortRows();

    CoverState explain(
      const list<Result>& results,
      ResExpl& resExpl) const;

    void explainGreedy(
      const list<Result>& results,
      const unsigned numStrategyTops,
      CoverTableau& tableau) const;

    void explainExhaustive(
      const list<Result>& results,
      const unsigned numStrategyTops,
      CoverTableau& tableau);

    void storeTableau(
      const Tricks& excessTricks,
      const CoverTableau& tableau);

    bool lookupTableau(
      const Tricks& excessTricks,
      CoverTableau const * tableauPtr);

    void getCoverCounts(
      unsigned& numTableaux,
      unsigned& numUses) const;

    string strCached() const;
};

#endif

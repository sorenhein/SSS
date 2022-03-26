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

#include "Cover.h"
#include "ProfilePair.h"
#include "TableauCache.h"

class Distribution;
class CoverMemory;
class CoverTableau;
class Result;
class ResExpl;


using namespace std;


class Covers
{
  private:

    Profile sumProfile;

    list<Cover> covers;

    list<CoverNew> coversNew;

    TableauCache tableauCache;


    void setup(
      const list<Result>& results,
      vector<unsigned char>& tricks,
      unsigned char& tricksMin) const;

    void prune();

    string strDebug(
      const string& title,
      const vector<unsigned char>& tricks) const;


  public:

    Covers();

    void reset();

    void prepare(
      const CoverMemory& coverMemory,
      const unsigned char maxLength,
      const unsigned char maxTops,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    void prepareNew(
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases,
      const Profile& sumProfileIn);

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
      const vector<unsigned char>& excessTricks,
      const CoverTableau& tableau);

    bool lookupTableau(
      const vector<unsigned char>& excessTricks,
      CoverTableau const * tableauPtr);

    void getCoverCounts(
      unsigned& numTableaux,
      unsigned& numUses) const;

    string strCached() const;
};

#endif

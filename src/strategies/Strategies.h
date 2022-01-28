/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_STRATEGIES_H
#define SSS_STRATEGIES_H

// These vectors represent partial declarer strategies, so they
// always have the same lengths and distribution numbers.

#include <list>
#include <string>

#include "Slist.h"
#include "result/Ranges.h"

#include "../distributions/Reductions.h"

struct Play;
class SurvivorList;
class Distribution;

using namespace std;

typedef bool (Strategy::*ComparatorType)(const Strategy& strat) const;


class Strategies
{
  private:
    
    Slist slist;

    Ranges ranges;


    void scrutinize(const Ranges& rangesIn);


  public:

    Strategies();

    void reset();

    const Strategy& front() const;

    unsigned size() const;

    bool empty() const;

    void setTrivial(
      const Result& trivial,
      const unsigned char len);

    void adapt(
      const Play& play,
      const SurvivorList& survivors);

    void forgetDetails();

    void reactivate(
      const Strategy& simpleStrat,
      const Strategy& constants);

    void symmetrize();

    // Does each Strategy separately using its own critical rank.
    bool reduceByResults(const Distribution& dist);

    void expand(
      const Reduction& reduction,
      const char rankAdder,
      const bool rotateFlag);

    bool operator == (const Strategies& strats2) const;

    bool equalPrimary(
      const Strategies& strats2,
      const bool enforceSizeFlag = true) const;

    void operator += (Strategies& strats2);

    void operator *= (Strategies& strats2);

    void makeRanges();

    void propagateRanges(const Strategies& child);

    bool purgeRanges(
      Strategy& constants,
      const Ranges& rangesParent,
      const bool debugFlag);

    const Ranges& getRanges() const;

    void studyOppsVoid();

    // TODO Do we need these long-term, all of them?
    void getResultLowest(Result& result) const;
    void getResultList(list<Result>& resultList) const;
    unsigned char winRankLow() const;

    void covers(
      const Covers& coversIn,
      list<ResExpl>& resExplanations,
      const unsigned char maxRank = 0) const;

    bool ordered() const;

    bool minimal() const;

    bool constantTricks() const;

    bool sameTricks() const;

    string strRanges(
      const string& title = "",
      const bool rankFlag = false) const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

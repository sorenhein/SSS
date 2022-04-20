/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SLIST_H
#define SSS_SLIST_H

#include <list>
#include <string>

#include "Strategy.h"
#include "result/Ranges.h"

#include "../distributions/Reductions.h"

struct StratData;
class Ranges;
struct Play;
class SurvivorList;
class Distribution;
class ProductStats;

using namespace std;

typedef bool (Strategy::*ComparatorType)(const Strategy& strat) const;




class Slist
{
  private:

    struct Addition
    {
      Strategy const * ptr;
      list<Strategy>::const_iterator iter;
    };

    list<Strategy> strategies;

    Result resultWestVoid;
    Result resultEastVoid;


    void collapseOnVoid();

    // Uses compareComplateBasic if needed.
    unsigned consolidateAnyTwo(
      list<Strategy>::iterator& iter1,
      list<Strategy>::iterator& iter2,
      ComparatorType lessEqualMethod);

    // Uses compareComplateBasic if needed.
    void consolidateTwo(ComparatorType lessEqualMethod);

    // Uses compareSecondary if needed.
    void consolidateGeneral(ComparatorType lessEqualMethod);

    void consolidate(ComparatorType lessEqualMethod);

    bool sameOrdered(
      const Slist& slist2,
      ComparatorType equalMethod) const;

    bool sameUnordered(
      const Slist& slist2,
      ComparatorType equalMethod) const;

    bool addendDominatedHeavier(
      list<Strategy>::iterator& iter,
      ComparatorType lessEqualMethod,
      const Strategy& addend) const;

    // Uses compareSecondary if needed.
    bool processSameWeights(
      list<Strategy>::iterator& iter,
      list<Strategy>::iterator& iterEnd,
      ComparatorType lessEqualMethod,
      const Strategy& addend);

    void eraseDominatedLighter(
      list<Strategy>::iterator& iter,
      ComparatorType lessEqualMethod,
      const Strategy& addend);

    // Uses compareSecondary if needed.
    void addStrategy(
      const Strategy& strategy,
      ComparatorType lessEqualMethod);

    // Uses lessEqualPrimaryScrutinized and then if needed,
    // compareSecondary or lessEqualCompleteBasic.
    void markChanges(
      const Slist& slist2,
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions,
      ComparatorType lessEqualMethod) const;

    void processChanges(
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions);

    string strHeader(
      const string& title,
      const bool rankFlag) const;

    string strSumLine(const bool rankFlag) const;


  public:

    Slist();

    // Simple methods.  The ones with direct access to strategies 
    // should be used sparingly.  begin() and end() are only used in
    // Strategies::study and Strategies::scrutinized in order to
    // avoid more calls of Strategy::study() and 
    // Strategy::scrutinize() in Slist.  front() is only used
    // in Node::removePlay and as a const.

    list<Strategy>::iterator begin() { return strategies.begin(); };
    list<Strategy>::iterator end() { return strategies.end(); }

    void clear();

    void resize(const unsigned len);

    const Strategy& front() const;

    void push_back(const Strategy& strat);

    unsigned size() const;

    bool empty() const;


    // Ways in which a new Slist arises.

    void setTrivial(
      const Result& trivial,
      const unsigned char len);

    void adapt(
      const Play& play,
      const SurvivorList& survivors);

    void forgetDetails();


    // Consolidate and symmetrize.

    void symmetrize();

    
    // Rank-check help methods.

    bool reduceByResults(const Distribution& dist);

    void expand(
      const Reduction& reduction,
      const char adder,
      const bool rotateFlag);


    // Comparator (full Result equality in the first case).

    bool operator == (const Slist& slist2) const;

    bool equalByMethod(
      const Slist& slist2,
      ComparatorType equalMethod,
      const bool enforceSizeFlag = true) const;


    // Addition.  If lessEqualMethod matches, then compareSecondary.

    // Uses compareSecondary if needed.
    void addStrategyInplace(ComparatorType lessEqualMethod);


    // Addition.

    // Uses compareSecondary if needed.
    void addStrategies(
      const Slist& slist2,
      ComparatorType lessEqualMethod);

    // Uses compareCompleteStudied.
    void plusOneByOne(const Slist& slist2);


    // Multiply strategy.

    // Uses strat *= strat, so probably implicitly study.
    void multiply(
      const Strategy& strat,
      ComparatorType lessEqualMethod,
      const bool consolidateFlag = true);


    // Multiply strategies.

    // Uses strat * strat, so probably implicitly study.
    void multiplyOneByOne(const Slist& slist2);

    // Uses addStrategyInplace which uses compareSecondary if needed.
    void multiply(
      const Slist& slist2,
      const Ranges& ranges,
      ComparatorType lessEqualMethod);


    // Splits.

    void splitDistributions(
      const Strategy& counterpart,
      Slist& own,
      Slist& shared);


    // Ranges.

    void makeRanges(Ranges& ranges) const;

    bool purgeRanges(
      Strategy& constants,
      const Ranges& rangesOwn,
      const Ranges& rangesParent,
      const bool debugFlag);


    // Results and winners.

    void studyOppsVoid();    

    void getResultLowest(Result& result) const;
    void getResultList(list<Result>& resultList) const;


    // Debug utilities.

    void covers(
      Covers& coversIn,
      const unsigned char maxRank,
      list<CoverTableau>& tableaux,
      ProductStats& productStats) const;

    bool ordered() const;

    // Uses compareCompleteBasic.
    bool minimal() const;

    bool constantTricks() const;

    bool sameTricks() const;


    // String methods.

    string strRanges(const string& title = "") const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

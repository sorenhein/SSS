/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SLIST_H
#define SSS_SLIST_H

#include <list>
#include <string>

#include "Strategy.h"
#include "result/Ranges.h"

struct StratData;
class Ranges;
struct Play;
struct Survivors;

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


    void collapseOnVoid();

    unsigned consolidateAnyTwo(
      list<Strategy>::iterator& iter1,
      list<Strategy>::iterator& iter2,
      ComparatorType lessEqualMethod);

    bool sameOrdered(const Slist& slist2) const;
    bool sameUnordered(const Slist& slist2) const;

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

    // Uses lessEqualPrimaryScrutinized and then if needed,
    // compareSecondary or lessEqualCompleteBasic.
    void markChanges(
      const Slist& slist2,
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions) const;

    void processChanges(
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions);

    string strHeader(
      const string& title,
      const bool rankFlag) const;

    string strWeights(const bool rankFlag) const;

    string strWinners() const;


  public:

    Slist();

    ~Slist();

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
      const Survivors& survivors);


    // Consolidate.

    // Uses compareComplateBasic if needed.
    void consolidateTwo(ComparatorType lessEqualMethod);

    // Uses compareSecondary if needed.
    void consolidate(ComparatorType lessEqualMethod);


    // Comparator (full Result equality).

    bool operator == (const Slist& slist2) const;


    // Addition.  If lessEqualMethod matches, then compareSecondary.

    // Uses compareSecondary if needed.
    void addStrategy(
      const Strategy& strategy,
      ComparatorType lessEqualMethod);

    // Uses compareSecondary if needed.
    void addStrategyInplace(ComparatorType lessEqualMethod);


    // Addition.

    // Uses compareSecondary if needed.
    void addStrategies(
      const Slist& slist2,
      ComparatorType lessEqualMethod);

    // Uses lessEqualPrimaryScrutinized and then if needed,
    // compareSecondary.
    void addStrategiesScrutinized(const Slist& slist2);

    // Uses compareCompleteStudied.
    void plusOneByOne(const Slist& slist2);


    // Multiply strategy.

    // Uses strat *= strat, so probably implicitly study.
    void operator *= (const Strategy& strat);


    // Multiply strategies.

    // Uses addStrategyInplace which uses compareSecondary if needed.
    void multiply(
      const Slist& slist2,
      const Ranges& ranges,
      ComparatorType lessEqualMethod);

    // Uses strat * strat, so probably implicitly study.
    void multiplyOneByOne(const Slist& slist2);


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


    // Winners.

    const Result resultLowest() const;


    // Debug utilities.

    bool ordered() const;

    // Uses compareCompleteBasic.
    bool minimal() const;


    // String methods.

    string strRanges(const string& title = "") const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

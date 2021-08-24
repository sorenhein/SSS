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


struct Addition
{
  Strategy const * ptr;
  list<Strategy>::const_iterator iter;
};


class Slist
{
  private:

    list<Strategy> strategies;


    // Full Result level
    bool sameOrdered(const Slist& slist2) const;
    bool sameUnordered(const Slist& slist2) const;

    bool addendDominatedHeavier(
      list<Strategy>::iterator& iter,
      ComparatorType lessEqualMethod,
      const Strategy& addend) const;

    bool processSameWeights(
      list<Strategy>::iterator& iter,
      list<Strategy>::iterator& iterEnd,
      ComparatorType lessEqualMethod,
      const Strategy& addend);

    void eraseDominatedLighter(
      list<Strategy>::iterator& iter,
      ComparatorType lessEqualMethod,
      const Strategy& addend);


    string strHeader(
      const string& title,
      const bool rankFlag) const;

    string strWeights(const bool rankFlag) const;

    string strWinners() const;

    void operator += (const Strategy& strat);


  public:

    Slist();

    ~Slist();

    // These as well as front() and back() should be used sparingly,
    // as we don't really want to expose strategies.
    list<Strategy>::iterator begin() 
      { return strategies.begin(); };
    list<Strategy>::iterator end() 
      { return strategies.end(); }
    list<Strategy>::const_iterator begin() const 
      { return strategies.begin(); };
    list<Strategy>::const_iterator end() const 
      { return strategies.end(); }

    void clear();

    void resize(const unsigned len);

    void setTrivial(
      const Result& trivial,
      const unsigned char len);

    void adapt(
      const Play& play,
      const Survivors& survivors);

    void collapseOnVoid();

    void consolidateTwo(ComparatorType lessEqualMethod);

    void consolidate(ComparatorType lessEqualMethod);

    void plusOneByOne(const Slist& slist2);

    void markChanges(
      const Slist& slist2,
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions) const;

    void processChanges(
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions);

    // Full Result level
    bool operator == (const Slist& slist2) const;

    void addStrategy(
      const Strategy& strategy,
      ComparatorType lessEqualMethod);

    void add(
      const Slist& slist2,
      ComparatorType lessEqualMethod);

    void multiply(
      const Slist& slist2,
      const Ranges& ranges,
      ComparatorType lessEqualMethod);

    void operator *= (const Strategy& strat);

    void multiplyAddStrategy(ComparatorType lessEqualMethod);

    void multiply(
      const Slist& slist2,
      ComparatorType lessEqualMethod);

    void multiplyOneByOne(const Slist& slist2);

    const Strategy& front() const;
    Strategy& back();

    void push_back(const Strategy& strat);

    unsigned size() const;
    bool empty() const;
    bool ordered() const;
    bool minimal() const;

    void getLoopData(StratData& stratData);

    const Result resultLowest() const;

    string strRanges(const string& title = "") const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

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

struct Play;
struct Survivors;

using namespace std;

typedef bool (Strategy::*ComparatorType)(const Strategy& strat) const;


class Strategies
{
  friend class Splits;

  private:
    
    Slist slist;

    Ranges ranges;

    bool scrutinizedFlag;


    void consolidateTwo(ComparatorType lessEqualMethod);

    void restudy();

    void scrutinize(const Ranges& rangesIn);

    void consolidate();


  public:

    Strategies();

    ~Strategies();

    void reset();

    void setTrivial(
      const Result& trivial,
      const unsigned char len);

    void adapt(
      const Play& play,
      const Survivors& survivors);

    bool operator == (const Strategies& strats2) const;

    void operator += (Strategies& strats2);

    void operator *= (const Strategy& strat);
    void operator *= (Strategies& strats2);

    const Strategy& front() const;
    unsigned size() const;
    bool empty() const;
    bool ordered() const;
    bool minimal() const;

    void splitDistributions(
      const Strategy& counterpart,
      Strategies& own,
      Strategies& shared);

    void reactivate(
      Strategy& simpleStrat,
      const Strategy& constants);

    void makeRanges();

    void propagateRanges(const Strategies& child);

    bool purgeRanges(
      Strategy& constants,
      const Ranges& rangesParent,
      const bool debugFlag);

    const Ranges& getRanges() const;

    const Result resultLowest() const;

    string strRanges(const string& title = "") const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

#ifndef SSS_STRATEGY_H
#define SSS_STRATEGY_H

#include <vector>
#include <list>
#include <string>

#include "result/Result.h"
#include "optim/Study.h"

class Ranges;
struct Play;
struct Survivors;

using namespace std;


typedef CompareDetail (Result::*CumulateType)(const Result& res2) const;


class Strategy
{
  private:

    list<Result> results;
    unsigned weightInt;

    // Used for faster comparisons on average.
    Study studied;


    void setConstants();

    // Returns true if <= is still possible at the trick level.
    // Terminates as soon as this becomes impossible if flag is set.

    bool cumulateCommon(
      const Strategy& strat2,
      const bool earlyStopFlag,
      CumulateType methodPtr,
      unsigned& cumul) const;

    bool cumulateCommon(
      const Strategy& strat2,
      const bool earlyStopFlag,
      unsigned& cumul) const;

    bool cumulate(
      const Strategy& strat2,
      const bool earlyStopFlag,
      unsigned& cumul) const;

    bool cumulatePrimary(
      const Strategy& strat2,
      const bool earlyStopFlag,
      unsigned& cumul) const;

    void cumulateSecondary(
      const Strategy& strat2,
      unsigned& cumul) const;

    void updateSingle(
      const Play& play,
      const unsigned char fullNo);

    void updateSameLength(
      const Play& play,
      const Survivors& survivors);

    void updateAndGrow(
      const Play& play,
      const Survivors& survivors);

    void adaptResults(
      const Play& play,
      const Survivors& survivors);

  public:

    Strategy();

    ~Strategy();

    list<Result>::iterator begin()
      { return results.begin(); };
    list<Result>::iterator end()
      { return results.end(); }
    list<Result>::const_iterator begin() const 
      { return results.begin(); };
    list<Result>::const_iterator end() const 
      { return results.end(); }

    void reset();

    void resize(const unsigned len);

    unsigned size() const;

    bool empty() const;

    bool constantTricks() const;

    unsigned weight() const;

    void push_back(const Result& result);

    list<Result>::iterator erase(list<Result>::iterator& iter);
    list<Result>::const_iterator erase(list<Result>::const_iterator& iter);

    void eraseRest(list<Result>::iterator iter);

    void logTrivial(
      const Result& trivial,
      const unsigned char len);

    void study();

    void scrutinize(const Ranges& ranges);


    // ***************** Comparisons ****************

    // Basic, unaccelerated comparators

    bool operator == (const Strategy& strat2) const;

    bool lessEqualCompleteBasic(const Strategy& strat2) const;

    CompareType compareCompleteBasic(const Strategy& strat2) const;

    Compare compareSecondary(const Strategy& strat2) const;

    // Study-level comparators

    bool lessEqualCompleteStudied(const Strategy& strat2) const;

    CompareType compareCompleteStudied(const Strategy& strat2) const;

    bool lessEqualPrimaryStudied(const Strategy& strat2) const;

    // Scrutiny-level comparators

    bool lessEqualPrimaryScrutinized(const Strategy& strat2) const;

    Compare comparePrimaryScrutinized(const Strategy& strat2) const;


    // ******************* Multiply *****************

    void operator *= (const Strategy& strat2);

    void multiply(
      const Strategy& strat1,
      const Strategy& strat2);


    // ******************** Ranges ******************

    void initRanges(Ranges& ranges) const;

    void extendRanges(Ranges& ranges) const;


    // ******************** Adapt *******************

    void adapt(
      const Play& play,
      const Survivors& survivors);

    void addComponentwise(const Strategy& strat2);


    // ******************* Winners ******************

    const Result resultLowest() const;


    // ******************* Strings ******************

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_STRATEGY_H
#define SSS_STRATEGY_H

#include <vector>
#include <list>
#include <string>

#include "result/Result.h"
#include "optim/Study.h"

class Ranges;
struct Play;
class Distribution;
class SurvivorList;
class Covers;
class CoverTableau;
struct Reduction;

using namespace std;


typedef CompareDetail (Result::*CumulateType)(const Result& res2) const;


class Strategy
{
  private:

    list<Result> results;
    unsigned weightInt;

    // Used for faster comparisons on average.
    Study studied;

/*
    // The first one is the product of all Result's currently in results.
    // The second one covers all Result's used to arrive at the 
    // current results, including perhaps some that were since
    // eliminated.  It's a subtle difference, and perhaps there really
    // should be a result history for each Result in results, but we
    // try it this way.
    // It is impractical to update them with every operation, so they
    // are updated at the same time as studied is.
    Result resultUsed;
    Result resultSeen;
*/


    void setConstants();

    bool detectChangedResults(
      const Reduction& reduction,
      vector<Result>& reducedResultList,
      vector<bool>& changeList) const;

    bool reduceByResults(const Reduction& reduction);

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
      const SurvivorList& survivors);

    void updateAndGrow(
      const Play& play,
      const SurvivorList& survivors);

    void adaptResults(
      const Play& play,
      const SurvivorList& survivors);

  public:

    Strategy();

    list<Result>::iterator begin()
      { return results.begin(); };
    list<Result>::iterator end()
      { return results.end(); }
    list<Result>::const_iterator begin() const 
      { return results.begin(); };
    list<Result>::const_iterator end() const 
      { return results.end(); }

    void reset();

    void resize(const size_t len);

    size_t size() const;

    bool empty() const;

    bool constantTricks() const;

    bool symmetric() const;

    bool symmetricPrimary() const;

    void symmetrize(const size_t distSize);

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

    // ************* Rank-check methods  ************

    bool reduceByResults(const Distribution& dist);

    void expand(
      const Reduction& reduction,
      const char rankAdder,
      const bool rotateFlag);

    // ***************** Comparisons ****************

    // Basic, unaccelerated comparators

    bool operator == (const Strategy& strat2) const;
    bool equalCompleteBasic(const Strategy& strat2) const;

    bool equalPrimaryBasic(const Strategy& strat2) const;

    bool lessEqualCompleteBasic(const Strategy& strat2) const;

    Compare compareCompleteBasic(const Strategy& strat2) const;

    Compare compareSecondary(const Strategy& strat2) const;

    bool lessEqualSecondaryOverallResult(const Strategy& strat2) const;

    Compare compareSecondaryOverallResult(
      const Strategy& strat2) const;

    // Study-level comparators

    bool equalCompleteStudied(const Strategy& strat2) const;

    bool equalPrimaryStudied(const Strategy& strat2) const;

    bool lessEqualCompleteStudied(const Strategy& strat2) const;

    Compare compareCompleteStudied(const Strategy& strat2) const;

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

    bool adapt(
      const Play& play,
      const SurvivorList& survivors,
      const size_t distSize,
      const bool symmOnlyFlag);

    void setAndAdaptVoid(
      const Play& play,
      const Result& resultWestVoid,
      const Result& resultEastVoid,
      const bool westVoidFlag,
      const bool eastVoidFlag,
      const unsigned char fullDistNo,
      const size_t distSize,
      const bool symmOnlyFlag);


    // ******************* Winners ******************

    void coversManual(
      Covers& coversIn,
      CoverTableau& tableau) const;

    void covers(
      Covers& coversIn,
      const unsigned char numStrategyTops,
      CoverTableau& tableau,
      bool& newTableauFlag) const;

    void studyOppsVoid(
      Result& resultWestVoid,
      Result& resultEastVoid) const;

    const Result resultLowest() const;


    // ******************* Strings ******************

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

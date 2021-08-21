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



class Strategy
{
  private:

    list<Result> results;
    unsigned weightInt;

    // Used for faster comparisons on average.
    Study study;


    void setConstants();

    // Returns true if <= is still possible at the trick level.
    // Terminates as soon as this becomes impossible if flag is set.

    bool cumulate(
      const Strategy& strat2,
      const bool earlyStopFlag,
      unsigned& cumul) const;

    void updateSingle(
      const unsigned char fullNo,
      const unsigned char trickNS);

    void updateSameLength(
      const Survivors& survivors,
      const unsigned char trickNS);

    void updateAndGrow(
      const Survivors& survivors,
      const unsigned char trickNS);

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

    unsigned weight() const;

    void push_back(const Result& result);

    list<Result>::iterator erase(list<Result>::iterator& iter);

    void eraseRest(list<Result>::iterator iter);

    void logTrivial(
      const Result& trivial,
      const unsigned char len);

    void restudy();

    void scrutinize(const Ranges& ranges);


    // ***************** Comparisons ****************

    // Full Result level (basic, unoptimized)
    bool operator == (const Strategy& strat2) const;
    bool lessEqualCompleteBasic(const Strategy& strat2) const;
    CompareType compareCompleteBasic(const Strategy& strat2) const;

    // Full Result level
    bool operator >= (const Strategy& strat2) const;

    // Trick-level
    bool greaterEqualByProfile(const Strategy& strat2) const;
    bool greaterEqualByStudy(const Strategy& strat2) const;
    bool greaterEqualByTricks(const Strategy& strat2) const;

    bool lessEqualPrimaryScrutinized(const Strategy& strat2) const;

    Compare comparePrimaryScrutinized(const Strategy& strat2) const;
    Compare compareSecondary(const Strategy& strat2) const;


    // ******************* Multiply *****************

    void operator *= (const Strategy& strat2);

    void multiply(
      const Strategy& strat1,
      const Strategy& strat2);


    // ******************** Ranges ******************

    void initRanges(Ranges& ranges);

    void extendRanges(Ranges& ranges);


    // ******************** Adapt *******************

    void adapt(
      const Play& play,
      const Survivors& survivors);


    // ******************* Winners ******************

    const Result resultLowest() const;


    // ******************* Strings ******************

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

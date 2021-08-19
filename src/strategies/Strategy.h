#ifndef SSS_STRATEGY_H
#define SSS_STRATEGY_H

#include <vector>
#include <list>
#include <string>

#include "optim/Study.h"

#include "result/Result.h"
#include "result/Ranges.h"

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

    // Trick-level, but returns a full-level cumulator.
    bool greaterEqualCumulator(
      const Strategy& strat2,
      unsigned& cum) const;

    unsigned makeCumulator(const Strategy& strat2) const;

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

    void push_back(const Result& result);

    unsigned weight() const;

    list<Result>::iterator erase(list<Result>::iterator& iter);

    void eraseRest(list<Result>::iterator iter);

    void logTrivial(
      const Result& trivial,
      const unsigned char len);

    void restudy();

    void scrutinize(const Ranges& ranges);

    // Full Result level
    bool operator == (const Strategy& strat2) const;

    // Full Result level
    bool greaterEqual(const Strategy& strat2) const;
    bool operator >= (const Strategy& strat2) const;

    // Full Result level
    CompareType compare(const Strategy& strat2) const;
    CompareDetail compareDetail(const Strategy& strat2) const;

    // Trick-level
    bool greaterEqualByProfile(const Strategy& strat2) const;
    bool greaterEqualByStudy(const Strategy& strat2) const;
    bool greaterEqualByTricks(const Strategy& strat2) const;
    Compare compareByProfile(const Strategy& strat2) const;

    void operator *= (const Strategy& strat2);

    void multiply(
      const Strategy& strat1,
      const Strategy& strat2);

    void initRanges(Ranges& ranges);

    void extendRanges(Ranges& ranges);

    void adapt(
      const Play& play,
      const Survivors& survivors);

    const Result resultLowest() const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

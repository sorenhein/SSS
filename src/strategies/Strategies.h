#ifndef SSS_STRATEGIES_H
#define SSS_STRATEGIES_H

// These vectors represent partial declarer strategies, so they
// always have the same lengths and distribution numbers.

#include <list>
#include <string>

#include "Strategy.h"
#include "Range.h"

struct StratData;
struct Play;
struct Survivors;
struct SplitStrategies;
struct ExtendedStrategy;

using namespace std;

typedef bool (Strategy::*ComparatorType)(const Strategy& strat) const;


class Strategies
{
  private:
    
    struct Addition
    {
      Strategy const * ptr;
      list<Strategy>::const_iterator iter;
    };


    list<Strategy> strategies;

    Ranges ranges;

    bool scrutinizedFlag;


    void restudy();

    bool sameOrdered(const Strategies& strats2) const;
    bool sameUnordered(const Strategies& strats2) const;

    void addStrategy(
      const Strategy& strategy,
      ComparatorType comparator);

    unsigned numDists() const;

    void plusOneByOne(const Strategies& strats2);

    void multiplyAdd(
      const Strategy& strat1,
      const Strategy& strat2);

    void multiplyAddNew(
      const Strategy& strat1,
      const Strategy& strat2,
      const Ranges& minima);

    void multiplyAddNewer(
      const Strategy& strat1,
      const Strategy& strat2,
      const Ranges& minima,
      const SplitStrategies& splitOwn,
      const SplitStrategies& splitOther,
      const unsigned indexOwn,
      const unsigned indexOther,
      list<ExtendedStrategy>& extendedStrategies);

    void markChanges(
      const Strategies& strats2,
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions) const;

    // TMP If this works, should perhaps go in some class?
    void setSplit(
      Strategies& stratsToSplit,
      const Strategy& strat2,
      SplitStrategies& split) const;

    // TMP If this works, should perhaps go in some class?
    bool greaterEqual(
      const ExtendedStrategy& es1,
      const ExtendedStrategy& es2,
      const SplitStrategies& split1,
      const SplitStrategies& split2) const;

    void collapseOnVoid();

    void combinedLower(
      const Ranges& ranges1,
      const Ranges& ranges2,
      const bool keepConstantsFlag,
      Ranges& minima) const;

    string strHeader(
      const string& title,
      const bool rankFlag) const;

    string strWeights(const bool rankFlag) const;

    void operator += (const Strategy& strat);


  public:

    Strategies();

    ~Strategies();

    void reset();

    void setTrivial(
      const Result& trivial,
      const unsigned char len);

    bool operator == (const Strategies& strats2) const;

    void operator += (Strategies& strats2); // TODO const if no ...

    void operator *= (const Strategy& strat);
    void operator *= (Strategies& strats2); // TODO const if no scrutinize

    const Strategy& front() const;

    unsigned size() const;
    bool empty() const;

    void getLoopData(StratData& stratData);

    void makeRanges();

    void propagateRanges(const Strategies& child);

    const Ranges& getRanges() const;

    void adapt(
      const Play& play,
      const Survivors& survivors);

    void consolidate();

    void scrutinize(const Ranges& rangesIn);

    string strRanges(const string& title = "") const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};


struct ExtendedStrategy
{
  Strategy overlap;
  unsigned indexOwn;
  unsigned indexOther;
  unsigned weight;
};

struct SplitStrategies
{
  Strategies own;
  Strategies shared;
  vector<Strategy *> ownPtrs;
  vector<vector<Compare>> matrix;
};


#endif

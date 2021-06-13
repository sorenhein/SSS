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

using namespace std;

typedef bool (Strategy::*ComparatorType)(const Strategy& strat) const;


class Strategies
{
  friend class Splits;

  private:
    
    struct Addition
    {
      Strategy const * ptr;
      list<Strategy>::const_iterator iter;
    };

    struct ExtendedStrategy
    {
      Strategy overlap;
      unsigned indexOwn;
      unsigned indexOther;
      unsigned weight;
    };


    list<Strategy> strategies;

    Ranges ranges;

    bool scrutinizedFlag;


    void collapseOnVoid();

    void consolidateTwo();

    void restudy();

    bool sameOrdered(const Strategies& strats2) const;
    bool sameUnordered(const Strategies& strats2) const;

    void addStrategy(
      const Strategy& strategy,
      ComparatorType comparator);

    void plusOneByOne(const Strategies& strats2);

    void markChanges(
      const Strategies& strats2,
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions) const;




    void multiplyAddStrategy(
      const Strategy& strat1,
      const Strategy& strat2,
      ComparatorType comparator);



    void multiplyAddNewer(
      const Strategy& strat1,
      const Strategy& strat2,
      const SplitStrategies& splitOwn,
      const SplitStrategies& splitOther,
      const unsigned indexOwn,
      const unsigned indexOther,
      list<ExtendedStrategy>& extendedStrategies);

    // TMP If this works, should perhaps go in some class?
    void setSplit(
      const Strategy& strat2,
      SplitStrategies& split);

    // TMP If this works, should perhaps go in some class?
    bool greaterEqual(
      const ExtendedStrategy& es1,
      const ExtendedStrategy& es2,
      const SplitStrategies& split1,
      const SplitStrategies& split2) const;

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

    void adapt(
      const Play& play,
      const Survivors& survivors);

    void consolidate();

    void scrutinize(const Ranges& rangesIn);

    bool operator == (const Strategies& strats2) const;

    void operator += (Strategies& strats2);

    void operator *= (const Strategy& strat);
    void operator *= (Strategies& strats2);

    const Strategy& front() const;
    unsigned size() const;
    bool empty() const;

    void getLoopData(StratData& stratData);

    void makeRanges();

    void propagateRanges(const Strategies& child);

    const Ranges& getRanges() const;

    string strRanges(const string& title = "") const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};


struct SplitStrategies
{
  Strategies own;
  Strategies shared;
  vector<Strategy *> ownPtrs;
  vector<vector<Compare>> matrix;
};

#endif

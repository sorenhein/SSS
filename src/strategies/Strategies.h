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


    bool sameOrdered(const Strategies& strats2);
    bool sameUnordered(const Strategies& strats2);

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


  public:

    Strategies();

    ~Strategies();

    void reset();

    Strategy& front();

    void setTrivial(
      const Result& trivial,
      const unsigned char len);

    void restudy();

    bool operator == (const Strategies& strats2);

    void operator += (const Strategy& strat);
    void operator += (Strategies& strats2); // TODO const if no ...

    void operator *= (const Strategy& strat);
    void operator *= (Strategies& strats2); // TODO const if no scrutinize

    unsigned size() const;
    unsigned numDists() const;

    void getLoopData(StratData& stratData);

    void makeRanges();

    void propagateRanges(const Strategies& child);

    const Ranges& getRanges() const;

    void consolidate();

    void adapt(
      const Play& play,
      const Survivors& survivors);

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

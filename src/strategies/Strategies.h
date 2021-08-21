#ifndef SSS_STRATEGIES_H
#define SSS_STRATEGIES_H

// These vectors represent partial declarer strategies, so they
// always have the same lengths and distribution numbers.

#include <list>
#include <string>

#include "Strategy.h"
#include "result/Ranges.h"

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


    list<Strategy> strategies;

    Ranges ranges;

    bool scrutinizedFlag;


    void collapseOnVoid();

    void consolidateTwo(ComparatorType lessEqualMethod);

    void restudy();

    // Full Result level
    bool sameOrdered(const Strategies& strats2) const;
    bool sameUnordered(const Strategies& strats2) const;

    bool addendDominatedHeavier(
      list<Strategy>::iterator& iter,
      ComparatorType lessEqualMethod,
      const Strategy& addend) const;

    void addStrategy(
      const Strategy& strategy,
      ComparatorType lessEqualMethod);

    void plusOneByOne(const Strategies& strats2);

    void markChanges(
      const Strategies& strats2,
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions) const;

    void multiplyAddStrategy(
      const Strategy& strat1,
      const Strategy& strat2,
      ComparatorType lessEqualMethod);


    string strHeader(
      const string& title,
      const bool rankFlag) const;

    string strWeights(const bool rankFlag) const;

    string strWinners() const;

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

    // Full Result level
    bool operator == (const Strategies& strats2) const;

    void operator += (Strategies& strats2);

    void operator *= (const Strategy& strat);
    void operator *= (Strategies& strats2);

    const Strategy& front() const;
    unsigned size() const;
    bool empty() const;
    bool ordered() const;
    bool minimal() const;

    void getLoopData(StratData& stratData);

    void makeRanges();

    void propagateRanges(const Strategies& child);

    const Ranges& getRanges() const;

    const Result resultLowest() const;

    string strRanges(const string& title = "") const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

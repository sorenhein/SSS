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

    void markChanges(
      const Strategies& strats2,
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions) const;

    void collapseOnVoid();

    string strHeader(
      const string& title,
      const bool rankFlag) const;

    string strWeights(const bool rankFlag) const;


  public:

    Strategies();

    ~Strategies();

    void reset();

    void setTrivial(
      const Result& trivial,
      const unsigned len);

    void restudy();

    bool operator == (const Strategies& strats2);

    void operator += (const Strategy& strat);
    void operator += (const Strategies& strats2);

    void operator *= (const Strategy& strat);
    void operator *= (const Strategies& strats2);

    unsigned size() const;
    unsigned numDists() const;

    void getLoopData(StratData& stratData);

    void makeRanges();

    void propagateRanges(const Strategies& child);

    void purgeRanges(const Strategies& parent);

    void getConstants(Strategy& constantsIn) const;

    const Ranges& getRanges() const;

    void consolidate();

    void adapt(
      const Play& play,
      const Survivors& survivors);

    void checkWeights() const;

    string strRanges(const string& title = "") const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

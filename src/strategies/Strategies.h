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

    void multiplyAdd(
      const Strategy& strat1,
      const Strategy& strat2);

    void multiplyAddNew(
      const Strategy& strat1,
      const Strategy& strat2,
      const Ranges& minima);

    void markChanges(
      const Strategies& strats2,
      list<Addition>& additions,
      list<list<Strategy>::const_iterator>& deletions) const;

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

#endif

#ifndef SSS_STRATEGIES_H
#define SSS_STRATEGIES_H

// These vectors represent partial declarer strategies, so they
// always have the same lengths and distribution numbers.

#include <list>

#include "Strategy.h"
// #include "StratData.h"

struct StratData;
struct Play;
struct Survivors;

using namespace std;


class Strategies
{
  private:

    list<Strategy> results;

    Ranges ranges;

    Strategy constants;


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
      const Result& trivialEntry,
      const unsigned len);

    bool operator == (const Strategies& tvs);

    void operator +=(const Strategy& tv);

    void operator +=(const Strategies& tvs);

    void operator *=(const Strategies& tvs);
    void operator *=(const Strategy& tv);

    void operator |=(const Strategies& tvs);

    unsigned size() const;
    unsigned numDists() const;

    void getLoopData(StratData& stratData);

    unsigned purge(const Strategy& constants);

    void consolidate();

    void makeRanges();

    void propagateRanges(const Strategies& child);

    void purgeRanges(const Strategies& parent);

    void getConstants(Strategy& constantsIn) const;

    const Ranges& getRanges() const;

    void addConstantWinners(Strategy& constants) const;

    void adapt(
      const Play& play,
      const Survivors& survivors);

    string strRanges(const string& title = "") const;

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

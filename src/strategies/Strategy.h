#ifndef SSS_STRATEGY_H
#define SSS_STRATEGY_H

#include <list>
#include <string>

#include "Result.h"
#include "Range.h"

struct Play;
struct Survivors;

using namespace std;


enum Compare
{
  COMPARE_LESS_THAN = 0,
  COMPARE_EQUAL = 1,
  COMPARE_GREATER_THAN = 2,
  COMPARE_INCOMMENSURATE = 3
};


class Strategy
{
  private:

    list<Result> results;
    unsigned weightInt;


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

    void eraseRest(list<Result>::iterator iter);

    void logTrivial(
      const Result& trivialEntry,
      const unsigned len);

    void log(
      const vector<unsigned>& distributions,
      const vector<unsigned>& tricks);

    bool operator == (const Strategy& tv2) const;
    bool operator >= (const Strategy& tv2) const;
    bool operator > (const Strategy& tv2) const;

    Compare compare(const Strategy& tv2) const;

    void operator *=(const Strategy& tv2);

    void bound(
      Strategy& constants,
      Strategy& lower) const;

    void constrain(Strategy& constants) const;

    unsigned purge(const Strategy& constants);

    void initRanges(Ranges& ranges);

    void extendRanges(Ranges& ranges);

    void purgeRanges(
      Ranges& ranges,
      const Ranges& parentRanges);

    void erase(list<Result>::iterator iter);

    void addConstantWinners(Strategy& constants) const;

    void updateSingle(
      const unsigned fullNo,
      const unsigned trickNS);

    void updateSameLength(
      const Survivors& survivors,
      const unsigned trickNS);

    void updateAndGrow(
      const Survivors& survivors,
      const unsigned trickNS);

    void adapt(
      const Play& play,
      const Survivors& survivors);

    unsigned size() const;

    unsigned weight() const;

    string str(const string& title = "") const;
};

#endif

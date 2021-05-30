#ifndef SSS_STRATEGY_H
#define SSS_STRATEGY_H

#include <vector>
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

    // Used for faster comparisons on average.
    vector<unsigned> summary;
    bool studiedFlag;

    list<unsigned> profiles;


    void setConstants();

    unsigned numGroups() const;

    bool greaterEqual(const Strategy& strat2) const;

    void updateSingle(
      const unsigned fullNo,
      const unsigned trickNS);

    void updateSameLength(
      const Survivors& survivors,
      const unsigned trickNS);

    void updateAndGrow(
      const Survivors& survivors,
      const unsigned trickNS);

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

    void logTrivial(
      const Result& trivialEntry,
      const unsigned len);

    void log(
      const vector<unsigned>& distributions,
      const vector<unsigned>& tricks);

    void study();

    void scrutinize(const Ranges& minima);

    bool operator == (const Strategy& strat2) const;

    bool operator >= (const Strategy& tv2) const;

    bool greaterEqualByProfile(const Strategy& strat2) const;

    void operator *= (const Strategy& strat2);

    void multiply(
      const Strategy& strat1,
      const Strategy& strat2);

    void initRanges(Ranges& ranges);

    void extendRanges(Ranges& ranges);

    void erase(list<Result>::iterator iter);
    void eraseRest(list<Result>::iterator iter);

    void adapt(
      const Play& play,
      const Survivors& survivors);

    unsigned size() const;

    unsigned weight() const;

    string str(const string& title = "") const;
};

#endif

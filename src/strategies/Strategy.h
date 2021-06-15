#ifndef SSS_STRATEGY_H
#define SSS_STRATEGY_H

#include <vector>
#include <list>
#include <string>

#include "Study.h"
#include "Result.h"
#include "Range.h"

#include "../const.h"

struct Play;
struct Survivors;

using namespace std;



class Strategy
{
  private:

    list<Result> results;
    unsigned weightInt;

    // Used for faster comparisons on average.
    vector<unsigned> summary;
    bool studiedFlag;

    list<unsigned> profiles;

    Study study2;


    void setConstants();

    unsigned numGroups() const;

    bool greaterEqual(const Strategy& strat2) const;

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
      const Result& trivialEntry,
      const unsigned char len);

    void log(
      const vector<unsigned char>& distributions,
      const vector<unsigned char>& tricks);

    void study();

    void scrutinize(const Ranges& minima);

    bool operator == (const Strategy& strat2) const;

    bool operator >= (const Strategy& tv2) const;

    bool greaterEqualByProfile(const Strategy& strat2) const;
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

    string str(const string& title = "") const;
};

#endif

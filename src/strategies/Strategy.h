#ifndef SSS_STRATEGY_H
#define SSS_STRATEGY_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <list>
#include <cassert>

#include "../Survivor.h"

#include "Winners.h"

struct Play;

using namespace std;


enum Compare
{
  COMPARE_LESS_THAN = 0,
  COMPARE_EQUAL = 1,
  COMPARE_GREATER_THAN = 2,
  COMPARE_INCOMMENSURATE = 3
};


struct TrickEntry
{
  unsigned dist;
  unsigned tricks;
  Winners winners;

  bool operator < (const TrickEntry& te2) const
  {
    assert(dist == te2.dist);
    return(tricks < te2.tricks);
  }

  bool operator != (const TrickEntry& te2) const
  {
    assert(dist == te2.dist);
    return(tricks != te2.tricks);
  }

  bool operator > (const TrickEntry& te2) const
  {
    assert(dist == te2.dist);
    return(tricks > te2.tricks);
  }

  void set(
    const unsigned tricksIn,
    const WinningSide side,
    const Card& card)
  {
    tricks = tricksIn;
    winners.set(side, card);
  }

  void setEmpty(const unsigned tricksIn)
  {
    tricks = tricksIn;
    winners.setEmpty();
  }

  string strEntry(const bool rankFlag) const
  {
    stringstream ss;
    ss << setw(4) << tricks;
    if (rankFlag)
      ss << setw(8) << winners.strEntry();
    return ss.str();
  }
};

struct RangeEntry
{
  unsigned dist;
  unsigned lower;
  unsigned upper;

  void operator *= (const RangeEntry& range2)
  {
    if (range2.upper < upper ||
        (range2.upper == upper && range2.lower < lower))
    {
      * this = range2;
    }
  };

  bool operator < (const RangeEntry& range2) const
  {
    return (upper <= range2.lower &&
        (range2.lower < range2.upper || lower < upper));
  };

  string str() const
  {
    stringstream ss;
    ss << 
      setw(2) << dist <<
      setw(4) << lower <<
      setw(4) << upper << endl;
    return ss.str();
  };
};

typedef list<RangeEntry> Ranges;


class Strategy
{
  private:

    list<TrickEntry> results;
    unsigned weightInt;


  public:

    Strategy();

    ~Strategy();

    list<TrickEntry>::const_iterator begin() const 
      { return results.begin(); };
    list<TrickEntry>::const_iterator end() const 
      { return results.end(); }

    void reset();

    void logTrivial(
      const TrickEntry& trivialEntry,
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
      // Strategy& upper) const;

    void constrain(Strategy& constants) const;

    unsigned purge(const Strategy& constants);

    void initRanges(Ranges& ranges);

    void extendRanges(Ranges& ranges);

    void purgeRanges(
      Ranges& ranges,
      const Ranges& parentRanges);

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

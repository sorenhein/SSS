#ifndef SSS_TVECTOR_H
#define SSS_TVECTOR_H

#include <vector>
#include <list>
#include <cassert>

#include "../struct.h"

#include "Winner.h"

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
  Winner winner;

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
    const unsigned rank,
    const unsigned depth,
    const unsigned number)
  {
    tricks = tricksIn;
    winner.set(side, rank, depth, number);
  }

  string strEntry(const bool rankFlag) const
  {
    stringstream ss;
    ss << setw(4) << tricks;
    if (rankFlag)
      ss << setw(4) << winner.strEntry();
    return ss.str();
  }
};


class Tvector
{
  private:

    list<TrickEntry> results;
    unsigned weightInt;


  public:

    Tvector();

    ~Tvector();

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

    bool operator == (const Tvector& tv2) const;
    bool operator >= (const Tvector& tv2) const;
    bool operator > (const Tvector& tv2) const;

    Compare compare(const Tvector& tv2) const;

    void operator *=(const Tvector& tv2);

    void bound(
      Tvector& constants,
      Tvector& lower,
      Tvector& upper) const;

    void constrict(Tvector& constants) const;

    unsigned purge(const Tvector& constants);

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
      const Survivors& survivors,
      const unsigned trickNS,
      const vector<Winner>& northOrder,
      const vector<Winner>& southOrder,
      const Winner& currBest,
      const bool lhoVoidFlag,
      const bool rhoVoidFlag,
      const bool rotateFlag);

    unsigned size() const;

    unsigned weight() const;

    string str(const string& title = "") const;
};

#endif

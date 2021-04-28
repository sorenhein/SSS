#ifndef SSS_STRATEGIES_H
#define SSS_STRATEGIES_H

// These vectors represent partial declarer strategies, so they
// always have the same lengths and distribution numbers.

#include <list>

#include "Strategy.h"
#include "Winner.h"
#include "Card.h"

struct Play;
struct Survivors;

using namespace std;


struct Bounds
{
  // Actual minima.
  Strategy minima;

  // The lowest maxima (so not the actual maxima).
  Strategy maxima;

  // The lowest constant results.
  Strategy constants;

  string str(const string& title = "") const
  {
    stringstream ss;
    if (title != "")
      ss << title << endl;
    ss <<
      minima.str("Minima") << endl <<
      maxima.str("Maxima") << endl <<
      constants.str("Constants") << endl;
    return ss.str();
  };
};


class Strategies
{
  private:

    list<Strategy> results;

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
      const TrickEntry& trivialEntry,
      const unsigned len);

    bool operator == (const Strategies& tvs);

    void operator +=(const Strategy& tv);

    void operator +=(const Strategies& tvs);

    void operator *=(const Strategies& tvs);
    void operator *=(const Strategy& tv);

    void operator |=(const Strategies& tvs);

    unsigned size() const;
    unsigned numDists() const;

    void bound(
      Strategy& constants,
      Strategy& lower,
      Strategy& upper) const;

    void bound(Bounds& bounds) const;

    unsigned purge(const Strategy& constants);

    void adapt(
      const Play& play,
      const Survivors& survivors);

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

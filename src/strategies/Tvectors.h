#ifndef SSS_TVECTORS_H
#define SSS_TVECTORS_H

// These vectors represent partial declarer strategies, so they
// always have the same lengths and distribution numbers.

#include <list>

#include "Tvector.h"
#include "Winner.h"
#include "Card.h"

struct Play;
struct Survivors;

using namespace std;


struct Bounds
{
  Tvector minima;
  Tvector maxima;
  Tvector constants;

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


class Tvectors
{
  private:

    list<Tvector> results;

    void collapseOnVoid();

    string strHeader(
      const string& title,
      const bool rankFlag) const;

    string strWeights(const bool rankFlag) const;


  public:

    Tvectors();

    ~Tvectors();

    void reset();

    void setTrivial(
      const TrickEntry& trivialEntry,
      const unsigned len);

    bool operator == (const Tvectors& tvs);

    void operator +=(const Tvector& tv);

    void operator +=(const Tvectors& tvs);

    void operator *=(const Tvectors& tvs);
    void operator *=(const Tvector& tv);

    void operator |=(const Tvectors& tvs);

    unsigned size() const;
    unsigned numDists() const;

    void bound(
      Tvector& constants,
      Tvector& lower,
      Tvector& upper) const;

    void bound(Bounds& bounds) const;

    unsigned purge(const Tvector& constants);

    void adapt(
      const Play& play,
      const Survivors& survivors);

    string str(
      const string& title = "",
      const bool rankFlag = false) const;
};

#endif

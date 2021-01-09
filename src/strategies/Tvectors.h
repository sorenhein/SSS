#ifndef SSS_TVECTORS_H
#define SSS_TVECTORS_H

// These vectors represent partial declarer strategies, so they
// always have the same lengths and distribution numbers.

#include <list>

#include "Tvector.h"
#include "../struct.h"


using namespace std;


class Tvectors
{
  private:

    list<Tvector> results;

    void collapseOnVoid();

    string strHeader(const string& title) const;

    string strWeights() const;


  public:

    Tvectors();

    ~Tvectors();

    void reset();

    void setTrivial(
      const unsigned value,
      const unsigned len);

    void operator +=(const Tvector& tv);

    void operator +=(const Tvectors& tvs);

    void operator *=(const Tvectors& tvs);

    Tvector constants() const; // TODO Return reference?

    Tvector lower() const; // TODO Return reference?

    void purge(const Tvector& constants);

    void adapt(
      const Survivors& survivors,
      const unsigned trickNS,
      const bool lhoVoidFlag,
      const bool rhoVoidFlag,
      const bool rotateFlag);

    string str(const string& title = "") const;
};

#endif

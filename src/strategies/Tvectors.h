#ifndef SSS_TVECTORS_H
#define SSS_TVECTORS_H

// These vectors represent partial declarer strategies, so they
// always have the same lengths and distribution numbers.

#include "Tvector.h"

#include <list>

using namespace std;


class Tvectors
{
  private:

    list<Tvector> results;

    string strHeader(const string& title) const;

    string strWeights() const;


  public:

    Tvectors();

    ~Tvectors();

    void reset();

    void log(const Tvector& tv);

    void log(const Tvectors& tvs);

    string str(const string& title = "") const;
};

#endif

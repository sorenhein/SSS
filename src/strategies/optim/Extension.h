/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXTENSION_H
#define SSS_EXTENSION_H

#include <list>

#include "../Strategy.h"

using namespace std;

class Slist;
struct Split;
class Ranges;


class Extension
{
  private:

    // An extension is created from two Strategy's as they are
    // multiplied.  The overlap in terms of distribution is stored
    // in a Strategy.  The unique distributions of the factors are
    // stored in a different place (Extensions), but the extension
    // knows a pointer to the unique Strategy's.  The weight of the
    // Extension is the weight of the Strategy plus the weights of
    // the unique components.

    Strategy overlap;

    Strategy const * own1ptr;
    Strategy const * own2ptr;

    unsigned weightOverall;
    unsigned weightOverlap;
    unsigned weightOwn1;
    unsigned weightOwn2;

    bool weightLessEqual(const Extension& ext2) const;


  public:

    Extension();

    void reset();

    void multiply(
      const Split& strat1,
      const Split& strat2,
      const Ranges& ranges);

    void flatten(Slist& slist);

    bool lessEqualPrimary(const Extension& ext2) const;

    Compare compareSecondary(const Extension& ext2) const;

    unsigned weight() const;
};

#endif

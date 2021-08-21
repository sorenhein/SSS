/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXTENSION_H
#define SSS_EXTENSION_H

#include <list>

#include "Split.h"
#include "../Strategy.h"

using namespace std;

class Ranges;


class Extension
{
  private:

    // An extension is created from two Strategy's as they are
    // multiplied.  The overlap in terms of distribution is stored
    // in a Strategy.  The unique distributions of the factors are
    // stored in a different place (Extensions), but the extension
    // knows its index within that place.  The weight of the
    // Extension is the weight of the Strategy plus the weights of
    // the unique components.

    Strategy overlap;

    Strategy const * own1ptr;
    Strategy const * own2ptr;

    unsigned index1Int;
    unsigned index2Int;

    unsigned weightInt;


  public:

    Extension();

    ~Extension();

    void reset();

    void multiplyNew(
      const Split& strat1,
      const Split& strat2,
      const Ranges& ranges);

    /* */
    void multiply(
      const Strategy& strat1,
      const Strategy& strat2,
      const Ranges& ranges);

    void finish(
      const unsigned index1,
      const unsigned index2,
      const unsigned weightUniques);

    void flatten(
      list<Strategy>& strategies,
      const Strategy& strat1,
      const Strategy& strat2);
      /* */

    void flattenNew(list<Strategy>& strategies);

    bool lessEqualPrimary(const Extension& ext2) const;
    bool lessEqualPrimaryNew(const Extension& ext2) const;

    Compare compareSecondary(const Extension& ext2) const;
    Compare compareSecondaryNew(const Extension& ext2) const;

    unsigned index1() const;

    unsigned index2() const;

    unsigned weight() const;

    void strOld(const Strategy& own1, const Strategy& own2) const;

    void strNew() const;
};

#endif

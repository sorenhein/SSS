/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SPLITS_H
#define SSS_SPLITS_H

#include <list>
#include <string>

#include "Split.h"
#include "../Slist.h"

using namespace std;


class Splits
{
  private:

    // Contains the splits of a Strategies into overlapping and
    // unique components, relative to another Strategies.
    // Also contains numbered pointers to the Strategy's whence
    // they came, and a cross matrix of pre-calculated comparisons
    // of the own Strategy's.

    Slist own;

    Slist shared;

    list<Split> splits;

    unsigned count;


    void setPointers();


  public:

    Splits();

    ~Splits();

    void reset();

    void split(
      Slist& strategies,
      const Strategy& counterpart,
      const Ranges& ranges);

    const list<Split>& splitStrategies() const;
};

#endif

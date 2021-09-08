/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_REDUCTIONS_H
#define SSS_REDUCTIONS_H

#include <vector>


using namespace std;

struct DistInfo;


// A Reduction is a mapping from unreduced to reduced distributions.
typedef vector<unsigned char> Reduction;


class Reductions
{
  private:

    // Indexed by the maximal, reduced EW rank to be reduced
    // (including that EW rank).  If it is the maximal EW rank,
    // then all ranks are to be reduced into a single one.
    // The index is >= 1 as there is no reason to reduce the 0 index.
    vector<Reduction> reductions;


  public:

    Reductions();

    void set(
      const vector<DistInfo>& distributions,
      const unsigned reducedRankSize);

    const Reduction& get(const unsigned rank) const;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_REDUCTION_H
#define SSS_REDUCTION_H

#include <vector>


using namespace std;


// A Reduction contains a mapping from unreduced to reduced distributions.
// The rank mapping is actually the same for all Reduction's within
// a Reductions, but we put it here for simplicity.

struct Reduction
{
  vector<unsigned char> full2reducedDist;
  vector<unsigned> const * reduced2fullRankPtr;
};

#endif

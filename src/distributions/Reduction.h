/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_REDUCTION_H
#define SSS_REDUCTION_H

#include <vector>


using namespace std;

struct DistInfo;
struct Reduction;


// A Reduction is a mapping from unreduced to reduced distributions.
struct Reduction
{
  vector<unsigned char> dist;
  vector<unsigned> reduced2fullRanks;
};

#endif

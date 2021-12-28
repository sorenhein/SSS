/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_REDUCTION_H
#define SSS_REDUCTION_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>


using namespace std;


// A Reduction contains a mapping from unreduced to reduced distributions.
// The rank mapping is actually the same for all Reduction's within
// a Reductions, but we put it here for simplicity.

struct Reduction
{
  vector<unsigned char> full2reducedDist;

  void setTrivial()
  {
    full2reducedDist.resize(1);
    full2reducedDist[0] = 0;
  }

  string str() const
  {
    stringstream ss;

    ss << "Distribution map\n";
    for (unsigned dfull = 0; dfull < full2reducedDist.size(); dfull++)
      ss << 
        setw(2) << dfull << 
        setw(4) << +full2reducedDist[dfull]<< "\n";

    return ss.str();
  }
};

#endif

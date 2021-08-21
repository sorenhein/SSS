/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SPLIT_H
#define SSS_SPLIT_H

using namespace std;

class Strategy;


struct Split
{
  Strategy const * ownPtr;
  Strategy const * sharedPtr;
};

#endif

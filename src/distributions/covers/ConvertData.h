/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CONVERTDATA_H
#define SSS_CONVERTDATA_H

using namespace std;

struct ConvertData
{
  unsigned counter;
  unsigned accum;
  unsigned position;

  ConvertData()
  {
    counter = 0;
    accum = 0;
    position = 0;
  };

  // The implementations of these two methods are in TrickConvert!
  // They share some defined constants.

  void increment(
    const unsigned char value,
    vector<unsigned>& result);

  void finish(vector<unsigned>& result);
};

#endif

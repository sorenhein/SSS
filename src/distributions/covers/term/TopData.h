/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TOPDATA_H
#define SSS_TOPDATA_H

using namespace std;

class RankNames;


struct TopData
{
  unsigned char value;
  RankNames const * rankNamesPtr;

  bool used()
  {
    return (value > 0);
  };
};

#endif

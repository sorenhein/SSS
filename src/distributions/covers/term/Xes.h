
/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_XES_H
#define SSS_XES_H

#include <string>

using namespace std;

enum Opponent: unsigned;


struct Xes
{
  unsigned char westMax, westMin;
  unsigned char eastMax, eastMin;
  string strWest, strEast;

  void set(
    const unsigned char distLengthLower,
    const unsigned char distLengthUpper,
    const unsigned char topsExact,
    const unsigned char oppsLength,
    const unsigned char oppsTops);

  void getRange(
    const Opponent opponent,
    unsigned char& oppMin,
    unsigned char& oppMax) const;

  string str() const;
};

#endif

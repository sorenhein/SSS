/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Xes.h"

#include "../../../utils/table.h"


void Xes::set(
  const unsigned char distLengthLower,
  const unsigned char distLengthUpper,
  const unsigned char topsExact,
  const unsigned char oppsLength,
  const unsigned char oppsTops)
{
  const unsigned char totalXes = oppsLength - oppsTops;

  westMax = (distLengthUpper < topsExact ? 0 : 
    distLengthUpper - topsExact);

  westMin = (distLengthLower < topsExact ? 0 :
    distLengthLower - topsExact);

  if (westMax > totalXes)
    westMax = totalXes;
  if (westMin > totalXes)
    westMin = totalXes;

  eastMax = totalXes - westMin;
  eastMin = totalXes - westMax;

  strWest = string(westMin, 'x') +
    "(" + string(westMax - westMin, 'x') + ")";
  strEast = string(eastMin, 'x') +
    "(" + string(eastMax - eastMin, 'x') + ")";
}


void Xes::getRange(
  const Opponent opponent,
  unsigned char& oppMin,
  unsigned char& oppMax) const
{
  if (opponent == OPP_EAST)
  {
    oppMin = eastMin;
    oppMax = eastMax;
  }
  else
  {
    oppMin = westMin;
    oppMax = westMax;
  }
}

 
string Xes::str() const
{
  stringstream ss;

  ss << "coverXes: " <<
    westMin << "-" << westMax << ", " <<
    eastMin << "-" << eastMax << ", " <<
    strWest << ", " << strEast << "\n";

  return ss.str();
}

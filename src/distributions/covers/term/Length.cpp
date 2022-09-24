/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Length.h"
#include "CoverOperator.h"

#include "../../../utils/table.h"


bool Length::notVoid() const
{
  // This is a special case in Product.
  return (
    Length::lower() == 1 && Length::getOperator() == COVER_GREATER_EQUAL);
}


Opponent Length::simplestOpponent(const unsigned char maximum) const
{
  if (! Length::used())
    return OPP_EITHER;

  // Choose the shorter side, as this tends to be more intuitive.
  const unsigned char lsum = Length::lower() + Length::upper();

  if (lsum > maximum)
    return OPP_EAST;
  else if (lsum < maximum)
    return OPP_WEST;
  else
    return OPP_EITHER;
}


string Length::strLengthBare(
  const unsigned char oppsLength,
  const Opponent simplestOpponent) const
{
  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    vLower = Length::lower();
    vUpper = (Length::getOperator() == COVER_GREATER_EQUAL ?  oppsLength : 
      Length::upper());
  }
  else
  {
    vLower = (
      Length::getOperator() == COVER_GREATER_EQUAL ? 
      0 : oppsLength - Length::upper());
    vUpper = oppsLength - Length::lower();
  }

  stringstream ss;

  if (vLower == 0)
  {
    if (vUpper == 1)
      ss << "at most singleton";
    else if (vUpper == 2)
      ss << "at most doubleton";
    else if (vUpper == 3)
      ss << "at most tripleton";
    else
    {
cout << "\nHIT1" << +vUpper << "\n";
      ss << "with at most " << +vUpper << " cards";
    }
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
  {
cout << "\nHIT2" << "\n";
    ss << "with no void on either side";
  }
  else
  {
cout << "\nHIT2" << +vLower << " to " << +vUpper << "\n";
    ss << "with " << +vLower << "-" << +vUpper << " cards";
  }

  return ss.str();
}


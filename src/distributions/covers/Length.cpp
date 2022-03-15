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


string Length::strEqual(const unsigned char lenActual) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (value1 == 0)
    ss << side << " is void";
  else if (value1 == lenActual)
  {
    assert(! symmFlag);
    ss << "East is void";
  }
  else if (value1 == 1)
    ss << side << " has a singleton";
  else if (value1 == lenActual-1)
  {
    assert(! symmFlag);
    ss << "East has a singleton";
  }
  else if (value1 == 2)
  {
    if (lenActual > 4)
      ss << side << " has a doubleton";
    else
      ss << "The suit splits 2=2";
  }
  else
    ss << "The suit splits " << +value1 << "=" << +(lenActual - value1);

  return ss.str();
}



string Length::strInside(const unsigned char lenActual) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (value1 == 0)
  {
    if (value2 == 1)
      ss << side << " has at most a singleton";
    else if (value2 == 2)
      ss << side << " has at most a doubleton";
    else
      ss << side << " has at most " << +value2 << " cards";
  }
  else if (value2 == lenActual)
  {
    ss << side << " has at least " << +value1 << " cards";
  }
  else if (value1 == 1 && value2 == lenActual-1)
  {
    ss << "Neither opponent is void";
  }
  else if (value1 + value2 == lenActual)
  {
    if (value1 + 1 == value2)
    {
      ss << "The suit splits " << +value1 << "-" << +value2 << 
        " either way";
    }
    else
    {
      ss << "The suit splits " << +value1 << "-" << +value2 <<
        " or better either way";
    }
  }
  else
  {
    ss << "The suit splits between " <<
      +value1 << "=" << +(lenActual - value1) << " and " <<
      +value2 << "=" << +(lenActual - value2);
  }

  return ss.str();
}


string Length::str(const unsigned char lenActual) const
{
  if (oper == COVER_EQUAL)
    return Length::strEqual(lenActual);
  else if (oper == COVER_INSIDE_RANGE)
    return Length::strInside(lenActual);
  else
  {
    assert(false);
    return "";
  }
}


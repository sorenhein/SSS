/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverSpec.h"


CoverSpec::CoverSpec()
{
}


// TODO Goes away longer-term
string CoverSpec::strLength() const
{
  if (westLength.oper == COVER_EQUAL)
  {
    if (westLength.value1 == 0)
    {
      if (invertFlag)
        return "West is not void";
      else
        return "West is void";
    }
    else if (westLength.value1 == oppsLength)
    {
      if (invertFlag)
        return "East is not void";
      else
        return "East is void";
    }
    else if (westLength.value1 == 1)
    {
      if (invertFlag)
        return "West does not have a singleton";
      else
        return "West has a singleton";
    }
    else if (westLength.value1 == oppsLength-1)
    {
      if (invertFlag)
        return "East does not have a singleton";
      else
        return "East has a singleton";
    }
    else if (westLength.value1 == 2)
    {
      if (oppsLength > 4)
      {
        if (invertFlag)
          return "West does not have a doubleton";
        else
          return "West has a doubleton";
      }
      else
      {
        if (invertFlag)
          return "The suit does not split 2-2";
        else
          return "The suit splits 2-2";
      }
    }
    else
    {
      if (invertFlag)
        return "The suit does not split " + to_string(westLength.value1) +
          "=" + to_string(oppsLength - westLength.value1);
      else
        return "The suit splits " + to_string(westLength.value1) +
          "=" + to_string(oppsLength - westLength.value1);
    }
  }
  else if (westLength.oper == COVER_INSIDE_RANGE)
  {
    if (westLength.value1 == 0)
    {
      if (invertFlag)
        return "West has " + to_string(westLength.value2+1) +
          " or more cards";
      else
        return "West has at most " + to_string(westLength.value2) +
          " cards";
    }
    else if (westLength.value2 == oppsLength)
    {
      if (invertFlag)
        return "West has at most " + to_string(westLength.value1-1) +
          " cards";
      else
        return "West has at least " + to_string(westLength.value1) +
          " cards";
    }
    else if (westLength.value1 == 1 && westLength.value2 == oppsLength-1)
    {
      if (invertFlag)
        return "Either opponent is void";
      else
        return "Neither opponent is void";
    }
    else if (westLength.value1 + westLength.value2 == oppsLength)
    {
      if (westLength.value1 + 1 == westLength.value2)
      {
        if (invertFlag)
          return "The suit does not split " + to_string(westLength.value1) +
            "-" + to_string(westLength.value2) + " either way";
        else
          return "The suit splits " + to_string(westLength.value1) +
            "-" + to_string(westLength.value2) + " either way";
      }
      else
      {
        if (invertFlag)
          return "The suit splits less evenly than " +
            to_string(westLength.value1) + "-" +
            to_string(westLength.value2) + " either way";
        else
          return "The suit splits " +
            to_string(westLength.value1) + "-" +
            to_string(westLength.value2) + " or better either way";
      }
    }
    else
    {
      if (invertFlag)
        return "West has less than " + to_string(westLength.value1) +
          " or more than " + to_string(westLength.value2) + " cards";
      else
        return "The suit splits between " + to_string(westLength.value1) +
          "=" + to_string(oppsLength-westLength.value1) + " and " +
          to_string(westLength.value2) + "=" +
          to_string(oppsLength-westLength.value2);
    }
  }
  else
    return westLength.str("cards");
}


// TODO Goes away longer-term
string CoverSpec::strTop1() const
{
  return westTop1.str("tops");
}


string CoverSpec::str() const
{
  if (mode == COVER_LENGTHS_ONLY)
    return CoverSpec::strLength();
  else if (mode == COVER_TOPS_ONLY)
    return CoverSpec::strTop1();
  else if (mode == COVER_LENGTHS_OR_TOPS)
    return CoverSpec::strLength() + ", or " + CoverSpec::strTop1();
  else if (mode == COVER_LENGTHS_AND_TOPS)
    return CoverSpec::strLength() + ", and " + CoverSpec::strTop1();
  else
    return "";
}


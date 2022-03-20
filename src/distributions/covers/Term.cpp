/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Term.h"
#include "TermCompare.h"
// #include "Length.h"

#include "../../const.h"

extern TermCompare termCompare;


/*
struct CoverXes
{
  unsigned char westMax, westMin;
  unsigned char eastMax, eastMin;
  string strWest, strEast;

  string str() const
  {
    stringstream ss;

    ss << "coverXes: " <<
      +westMin << "-" << +westMax << ", " <<
      +eastMin << "-" << +eastMax << ", " <<
      strWest << ", " << strEast << "\n";

    return ss.str();
  };
};
*/



Term::Term()
{
  Term::reset();
}


void Term::reset()
{
  lower = UCHAR_NOT_SET;
  upper = UCHAR_NOT_SET;
  oper = COVER_OPERATOR_SIZE;

  // Choose a large index value and hope to break the index 
  // if we try to use it unset...
  index = numeric_limits<unsigned short>::max();
  data = termCompare.getData(false, 0, 0);
}


void Term::set(
  const unsigned char valueIn,
  const CoverOperator operIn)
{
  lower = valueIn;
  upper = valueIn; // Just to have something
  oper = operIn;

  index = termCompare.getIndex(lower, upper, oper);
  data = termCompare.getData(true, upper-lower, 0);
}

void Term::set(
  const unsigned char lowerIn,
  const unsigned char upperIn,
  const CoverOperator operIn)
{
  lower = lowerIn;
  upper = upperIn;
  oper = operIn;

  index = termCompare.getIndex(lower, upper, oper);
  data = termCompare.getData(true, upper-lower, 0);
}


void Term::setNew(
  const unsigned char oppSize,
  const unsigned char lowerIn,
  const unsigned char upperIn)
{
  // oppSize is the maximum value, so the total length in case of
  // a length, or the number of tops in case of a top.

  if (lowerIn == 0 && upperIn == oppSize)
  {
    // Not set
    data = termCompare.getData(false, 0, 0);
    return;
  }

  lower = lowerIn;
  upper = upperIn;
  unsigned char complexity;

  if (lower == upper)
  {
    oper = COVER_EQUAL;
    complexity = 1;
  }
  else if (lowerIn == 0)
  {
    oper = COVER_LESS_EQUAL;
    complexity = 1;
  }
  else if (upperIn == oppSize)
  {
    oper = COVER_GREATER_EQUAL;
    complexity = 1;
  }
  else
  {
    oper = COVER_INSIDE_RANGE;
    complexity = 2;
  }

  index = termCompare.getIndex(lower, upper, oper);
  data = termCompare.getData(true, upper-lower, complexity);
}


bool Term::includes(const unsigned char valueIn) const
{
  return termCompare.includes(index, valueIn);
}


bool Term::used() const
{
  return termCompare.used(data);
}


unsigned char Term::getComplexity() const
{
  return termCompare.complexity(data);
}


unsigned char Term::getRange() const
{
  return termCompare.range(data);
}


string Term::strGeneral() const
{
  stringstream ss;

  if (Term::used())
  {
    string s;
    if (oper == COVER_EQUAL)
    {
      s = "== " + to_string(+lower);
    }
    else if (oper == COVER_INSIDE_RANGE)
    {
      s = to_string(+lower) + "-" + to_string(+upper);
    }
    else if (oper == COVER_GREATER_EQUAL)
    {
      s = ">= " + to_string(+lower);
    }
    else if (oper == COVER_LESS_EQUAL)
    {
      s = "<= " + to_string(+upper);
    }
    else
    {
      assert(false);
    }
    
    ss << setw(8) << s;
  }
  else
  {
    ss << setw(8) << "unused";
  }

  return ss.str();
}


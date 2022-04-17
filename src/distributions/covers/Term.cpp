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

#include "../../const.h"

extern TermCompare termCompare;


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
  data = termCompare.getData(false, 0);
}


void Term::set(
  const unsigned char oppSize,
  const unsigned char lowerIn,
  const unsigned char upperIn)
{
  // oppSize is the maximum value, so the total length in case of
  // a length, or the total number of tops in case of a top.

  if (lowerIn == 0 && upperIn == oppSize)
  {
    // Not set
    data = termCompare.getData(false, 0);
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
    upper = 0xf; // For consistency
    complexity = 1;
  }
  else
  {
    oper = COVER_INSIDE_RANGE;
    complexity = 2;
  }

  index = termCompare.getIndex(lower, upper, oper);
  data = termCompare.getData(true, complexity);
}


bool Term::includes(const unsigned char value) const
{
  if (Term::used())
    return termCompare.includes(index, value);
  else
    return true;
}


bool Term::symmetrizable(const unsigned char maximum) const
{
  // True if the term occupies the lower half of its possible interval,
  // not including any middle value.
  assert(Term::used());
  return (upper <= (maximum-1)/2);
}


Opponent Term::simplestOpponent(const unsigned char maximum) const
{
  if (! Term::used())
    return OPP_EITHER;

  const unsigned char lsum = lower + upper;
  if (lsum > maximum)
    return OPP_EAST;
  else if (lsum < maximum)
    return OPP_WEST;
  else
    return OPP_EITHER;
}


CoverOperator Term::getOperator() const
{
  return oper;
}


bool Term::used() const
{
  return termCompare.used(data);
}


unsigned char Term::complexity() const
{
  return termCompare.complexity(data);
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


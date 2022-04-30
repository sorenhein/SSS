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

#include "../../../const.h"

extern TermCompare termCompare;


Term::Term()
{
  Term::reset();
}


void Term::reset()
{
  // Choose a large index value and hope to break the index 
  // if we try to use it unset...
  index = numeric_limits<unsigned short>::max();
  data = termCompare.getData(false, 0);
}


void Term::set(
  const unsigned char oppSize,
  const unsigned char lower,
  const unsigned char upper)
{
  // oppSize is the maximum value, so the total length in case of
  // a length, or the total number of tops in case of a top.

  if (lower == 0 && upper == oppSize)
  {
    // Not set
    data = termCompare.getData(false, 0);
    return;
  }

  unsigned char lowerInt = lower;
  unsigned char upperInt = upper;
  CoverOperator oper;
  unsigned char complexity;

  if (lowerInt == upperInt)
  {
    oper = COVER_EQUAL;
    complexity = 2;
  }
  else if (lower == 0)
  {
    oper = COVER_LESS_EQUAL;
    complexity = 2;
  }
  else if (upper == oppSize)
  {
    oper = COVER_GREATER_EQUAL;
    upperInt = 0xf; // For consistency
    complexity = 2;
  }
  else
  {
    oper = COVER_INSIDE_RANGE;
    complexity = 3;
  }

  index = termCompare.getIndex(lowerInt, upperInt, oper);
  data = termCompare.getData(true, complexity);
}


bool Term::includes(const unsigned char value) const
{
  if (Term::used())
    return termCompare.includes(index, value);
  else
    return true;
}


SymmTerm Term::symmetrizable(const unsigned char maximum) const
{
  // True if the term occupies the lower half of its possible interval,
  // not including any middle value.
  assert(Term::used());

  const unsigned char lowerInt = Term::lower();
  const unsigned char upperInt = Term::upper();

  if (maximum & 1)
  {
    // When the maximum is odd, e.g. 5, there is no midpoint.
    const unsigned char critical = maximum/2;
    if (upperInt < critical)
      return TERM_SYMMETRIZABLE;
    else if (lowerInt > critical+1)
      return TERM_SYMMETRIZABLE;
    else if (upperInt == critical)
      return TERM_OPEN_CONSECUTIVE;
    else if (lowerInt == critical+1)
      return TERM_OPEN_CONSECUTIVE;
    else
      return TERM_NOT_SYMMETRIZABLE;
  }
  else
  {
    // When the maximum is even, e.g. 4, it is also OK for
    // lower and upper both to equal the midpoint, but then something
    // else must break the symmetry.
    const unsigned char midpoint = maximum/2;
    if (upperInt < midpoint)
      return TERM_SYMMETRIZABLE;
    else if (lowerInt > midpoint)
      return TERM_SYMMETRIZABLE;
    else if (lowerInt == upperInt && upperInt == midpoint)
      return TERM_OPEN_CENTERED;
    else
      return TERM_NOT_SYMMETRIZABLE;
  }
}


Opponent Term::simplestOpponent(const unsigned char maximum) const
{
  if (! Term::used())
    return OPP_EITHER;

  const unsigned char lsum = Term::lower() + Term::upper();

  if (lsum > maximum)
    return OPP_EAST;
  else if (lsum < maximum)
    return OPP_WEST;
  else
    return OPP_EITHER;
}


CoverOperator Term::getOperator() const
{
  return termCompare.getOperator(index);
}


unsigned char Term::lower() const
{
  return termCompare.getLower(index);
}


unsigned char Term::upper() const
{
  return termCompare.getUpper(index);
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
    const CoverOperator oper = Term::getOperator();

    string s;
    if (oper == COVER_EQUAL)
      s = "== " + to_string(+Term::lower());
    else if (oper == COVER_INSIDE_RANGE)
      s = to_string(+Term::lower()) + "-" + to_string(+Term::upper());
    else if (oper == COVER_GREATER_EQUAL)
      s = ">= " + to_string(+Term::lower());
    else if (oper == COVER_LESS_EQUAL)
      s = "<= " + to_string(+Term::upper());
    else
      assert(false);
    
    ss << setw(8) << s;
  }
  else
  {
    ss << setw(8) << "unused";
  }

  return ss.str();
}


/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERHELP_H
#define SSS_COVERHELP_H

enum CoverMode
{
  COVER_LENGTHS_ONLY = 0,
  COVER_TOPS_ONLY = 1,
  COVER_LENGTHS_OR_TOPS = 2,
  COVER_LENGTHS_AND_TOPS = 3,
  COVER_MODE_SIZE = 4
};

enum CoverOperator
{
  COVER_LESS_EQUAL = 0,
  COVER_EQUAL = 1,
  COVER_GREATER_EQUAL = 2,
  COVER_INSIDE_RANGE = 3,
  COVER_OPERATOR_SIZE = 4,
  COVER_OUTSIDE_RANGE = 5
  // TODO Reorder later
};

enum CoverState
{
  COVER_DONE = 0,
  COVER_OPEN = 1,
  COVER_IMPOSSIBLE = 2,
  COVER_STATE_SIZE = 3
};

struct CoverElement
{
  typedef unsigned char 
    (CoverElement::*ComparePtr)(const unsigned char valueIn) const;

  unsigned char value1;
  unsigned char value2;
  CoverOperator oper;
  ComparePtr ptr;

  unsigned char lessEqual(const unsigned char valueIn) const
  {
    return (valueIn <= value1 ? 1 : 0);
  };

  unsigned char equal(const unsigned char valueIn) const
  {
    return (valueIn == value1 ? 1 : 0);
  };

  unsigned char greaterEqual(const unsigned char valueIn) const
  {
    return (valueIn >= value1 ? 1 : 0);
  };

  unsigned char insideRange(const unsigned char valueIn) const
  {
    return (valueIn >= value1 && valueIn <= value2 ? 1 : 0);
  };

  unsigned char outsideRange(const unsigned char valueIn) const
  {
    return (valueIn <= value1 && valueIn >= value2 ? 1 : 0);
  };

  void setOperator(const CoverOperator operIn)
  {
    oper = operIn;
    if (oper == COVER_LESS_EQUAL)
      ptr = &CoverElement::lessEqual;
    else if (oper == COVER_EQUAL)
      ptr = &CoverElement::equal;
    else if (oper == COVER_GREATER_EQUAL)
      ptr = &CoverElement::greaterEqual;
    else if (oper == COVER_INSIDE_RANGE)
      ptr = &CoverElement::insideRange;
    else if (oper == COVER_OUTSIDE_RANGE)
      ptr = &CoverElement::outsideRange;
    else
      assert(false);
  };

  unsigned char includes(const unsigned char valueIn) const
  {
    assert(ptr != nullptr);
    return (this->*ptr)(valueIn);
  }
};

struct CoverSpec
{
  CoverMode mode;
  CoverElement westLength;
  CoverElement westTop1;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverElement.h"

#include "../../const.h"


CoverElement::CoverElement()
{
  CoverElement::reset();
}


void CoverElement::reset()
{
  value1 = UCHAR_NOT_SET;
  value2 = UCHAR_NOT_SET;
  oper = COVER_OUTSIDE_RANGE;
  ptr = nullptr;
}


bool CoverElement::equal(const unsigned char valueIn) const
{
  return (valueIn == value1 ? 1 : 0);
}


bool CoverElement::insideRange(const unsigned char valueIn) const
{
  return (valueIn >= value1 && valueIn <= value2 ? 1 : 0);
}


// TODO Should be private once Covers is cleaner
void CoverElement::setOperator(const CoverOperator operIn)
{
  oper = operIn;
  if (oper == COVER_EQUAL)
    ptr = &CoverElement::equal;
  else if (oper == COVER_INSIDE_RANGE)
    ptr = &CoverElement::insideRange;
  else
    ptr = nullptr;
}


void CoverElement::setValue(const unsigned char valueIn)
{
  value1 = valueIn;
  value2 = UCHAR_NOT_SET;
}


void CoverElement::setValues(
  const unsigned char value1In,
  const unsigned char value2In)
{
  value1 = value1In;
  value2 = value2In;
}


void CoverElement::set(
  const unsigned char valueIn,
  const CoverOperator operIn)
{
  value1 = valueIn;
  setOperator(operIn);
}

void CoverElement::set(
  const unsigned char value1In,
  const unsigned char value2In,
  const CoverOperator operIn)
{
  value1 = value1In;
  value2 = value2In;
  setOperator(operIn);
}


bool CoverElement::includes(const unsigned char valueIn) const
{
  assert(ptr != nullptr);
  return (this->*ptr)(valueIn);
}


string CoverElement::strRaw() const
{
  stringstream ss;

  ss << +value1 << " to " << +value2 << ", oper ";
  if (oper == COVER_EQUAL)
    ss << "EQUAL";
  else if (oper == COVER_INSIDE_RANGE)
    ss << "INSIDE";
  else
    ss << "UNKNOWN";
  ss << "\n";

  return ss.str();
}


string CoverElement::str(const string& word) const
{
  stringstream ss;

  if (oper == COVER_LESS_EQUAL)
    ss << "West has at most " << +value1 << " " << word;
  else if (oper == COVER_EQUAL)
    ss << "West has exactly " << +value1 << " " << word;
  else if (oper == COVER_GREATER_EQUAL)
    ss << "West has at least " << +value1 << " " << word;
  else if (oper == COVER_INSIDE_RANGE)
    ss << "West has " << word <<  " in range " << 
      +value1 << " to " << +value2 << " " << word << " inclusive";
  else if (oper == COVER_OUTSIDE_RANGE)
  {
    assert(value1 < value2);
    ss << "West has up to " << +value1 << " " <<
    " or at least " << +value2 << word << " inclusive";
  }
  else
    assert(false);

  return ss.str();
}


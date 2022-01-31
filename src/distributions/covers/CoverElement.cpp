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
  oper = COVER_OPERATOR_SIZE;
  ptr = nullptr;
}


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


bool CoverElement::equal(const unsigned char valueIn) const
{
  return (valueIn == value1 ? 1 : 0);
}


bool CoverElement::insideRange(const unsigned char valueIn) const
{
  return (valueIn >= value1 && valueIn <= value2 ? 1 : 0);
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

  if (oper == COVER_EQUAL)
    ss << "West has exactly " << +value1 << " " << word;
  else if (oper == COVER_INSIDE_RANGE)
    ss << "West has " << word <<  " in range " << 
      +value1 << " to " << +value2 << " " << word << " inclusive";
  else
    assert(false);

  return ss.str();
}


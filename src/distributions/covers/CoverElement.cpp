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
  usedFlag = false;
  complexity = 0;
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
  CoverElement::setOperator(operIn);
  usedFlag = true;
}

void CoverElement::set(
  const unsigned char value1In,
  const unsigned char value2In,
  const CoverOperator operIn)
{
  value1 = value1In;
  value2 = value2In;
  CoverElement::setOperator(operIn);
  usedFlag = true;
}


void CoverElement::setNew(
  const unsigned char lenActual,
  const unsigned char value1In,
  const unsigned char value2In)
{
  // Returns true if actually in non-trivial use
  if (value1In == 0 && value2In == lenActual)
  {
    usedFlag = false;
    complexity = 0;
    return;
  }

  CoverElement::setOperator(value1In == value2In ?
    COVER_EQUAL : COVER_INSIDE_RANGE);

  value1 = value1In;
  value2 = value2In;
  usedFlag = true;

  if (value1In == 0 || value2 == lenActual ||value1In == value2In)
    complexity = 1;
  else
    complexity = 2;
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


bool CoverElement::used() const
{
  return usedFlag;
}


unsigned char CoverElement::getComplexity() const
{
  return complexity;
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


string CoverElement::strShort(const unsigned char lenActual) const
{
  stringstream ss;

  if (usedFlag)
  {
    string s;
    if (value1 == value2)
      s = "== " + to_string(+value1);
    else if (value1 == 0)
      s = "<= " + to_string(+value2);
    else if (value2 == lenActual)
      s = ">= " + to_string(+value1);
    else
      s = to_string(+value1) + "-" + to_string(+value2);
    
    ss << setw(8) << s;
  }
  else
  {
    ss << setw(8) << "unused";
  }

  return ss.str();
}


string CoverElement::strShort() const
{
  stringstream ss;

  if (usedFlag)
  {
    string s;
    if (value1 == value2)
      s = "== " + to_string(+value1);
    else if (value1 == 0)
      s = "<= " + to_string(+value2);
    else
      s = to_string(+value1) + "-" + to_string(+value2);
    
    ss << setw(8) << s;
  }
  else
  {
    ss << setw(8) << "unused";
  }

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


/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Cover.h"


Cover::Cover()
{
  Cover::reset();
}


void Cover::reset()
{
  profile.clear();

  spec.mode = COVER_MODE_SIZE;
  spec.westLength.oper = COVER_OPERATOR_SIZE;
  spec.westTop1.oper = COVER_OPERATOR_SIZE;

  weight = 0;
}


unsigned char Cover::lessEqual(
  const unsigned char value,
  const unsigned char ref,
  [[maybe_unused]] const unsigned char dummy) const
{
  return (value <= ref ? 1 : 0);
}


unsigned char Cover::equal(
  const unsigned char value,
  const unsigned char ref,
  [[maybe_unused]] const unsigned char dummy) const
{
  return (value == ref ? 1 : 0);
}


unsigned char Cover::greaterEqual(
  const unsigned char value,
  const unsigned char ref,
  [[maybe_unused]] const unsigned char dummy) const
{
  return (value >= ref ? 1 : 0);
}


unsigned char Cover::withinRange(
  const unsigned char value,
  const unsigned char ref1,
  const unsigned char ref2) const
{
  return (value >= ref1 && value <= ref2-ref1 ? 1 : 0);
}


void Cover::prepare(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const vector<unsigned char>& cases,
  const CoverSpec& specIn)
{
  assert(lengths.size() == tops.size());
  const unsigned len = lengths.size();
  profile.resize(len);

  spec = specIn;

  CoverComparePtr lengthFncPtr = nullptr;
  if (spec.westLength.oper == COVER_LESS_EQUAL)
    lengthFncPtr = &Cover::lessEqual;
  else if (spec.westLength.oper == COVER_EQUAL)
    lengthFncPtr = &Cover::equal;
  else if (spec.westLength.oper == COVER_GREATER_EQUAL)
    lengthFncPtr = &Cover::greaterEqual;
  else if (spec.westLength.oper == COVER_INSIDE_RANGE)
    lengthFncPtr = &Cover::withinRange;
  else
    assert(false);

  CoverComparePtr topFncPtr = nullptr;
  if (spec.westTop1.oper == COVER_LESS_EQUAL)
    topFncPtr = &Cover::lessEqual;
  else if (spec.westTop1.oper == COVER_EQUAL)
    topFncPtr = &Cover::equal;
  else if (spec.westTop1.oper == COVER_GREATER_EQUAL)
    topFncPtr = &Cover::greaterEqual;
  else
    assert(false);

  for (unsigned dno = 0; dno < len; dno++)
  {
    if (spec.mode == COVER_LENGTHS_ONLY)
    {
      profile[dno] = (this->*lengthFncPtr)(
        lengths[dno], spec.westLength.value1, spec.westLength.value2);
    }
    else if (spec.mode == COVER_TOPS_ONLY)
    {
      profile[dno] = (this->*topFncPtr)(tops[dno], spec.westTop1.value1, 
        spec.westLength.value2);
    }
    else if (spec.mode == COVER_LENGTHS_OR_TOPS)
    {
      profile[dno] = (this->*lengthFncPtr)(
          lengths[dno], spec.westLength.value1, spec.westLength.value2) |
          (this->*topFncPtr)(tops[dno], spec.westTop1.value1, spec.westLength.value1);
    }
    else if (spec.mode == COVER_LENGTHS_AND_TOPS)
    {
      profile[dno] = (this->*lengthFncPtr)(
          lengths[dno], spec.westLength.value1, spec.westLength.value2) &
          (this->*topFncPtr)(tops[dno], spec.westTop1.value1, spec.westLength.value1);
    }

    if (profile[dno])
      weight += cases[dno];
  }
}


CoverState Cover::explain(vector<unsigned char>& tricks) const
{
  assert(tricks.size() == profile.size());

  CoverState state = COVER_DONE;

  for (unsigned i = 0; i < tricks.size(); i++)
  {
    if (profile[i] > tricks[i])
      return COVER_IMPOSSIBLE;
    else if (profile[i] < tricks[i])
      state = COVER_OPEN;
  }

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= profile[i];

  return state;
}


unsigned char Cover::getWeight() const
{
  return weight;
}


string Cover::strLength() const
{
  stringstream ss;

  if (spec.westLength.oper == COVER_LESS_EQUAL)
    ss << "West has at most " << +spec.westLength.value1 << " cards";
  else if (spec.westLength.oper == COVER_EQUAL)
    ss << "West has exactly " << +spec.westLength.value1 << " cards";
  else if (spec.westLength.oper == COVER_GREATER_EQUAL)
    ss << "West has at least " << +spec.westLength.value1 << " cards";
  else if (spec.westLength.oper == COVER_GREATER_EQUAL)
    ss << "West has cards in range " << +spec.westLength.value1 << 
      "  to " << +spec.westLength.value2;
  
  return ss.str();
}


string Cover::strTop() const
{
  stringstream ss;

  if (spec.westTop1.oper == COVER_LESS_EQUAL)
    ss << "West has at most " << +spec.westTop1.value1 << " tops";
  else if (spec.westTop1.oper == COVER_EQUAL)
    ss << "West has exactly " << +spec.westTop1.value1 << " tops";
  else if (spec.westTop1.oper == COVER_GREATER_EQUAL)
    ss << "West has at least " << +spec.westTop1.value1 << " tops";
  
  return ss.str();
}


string Cover::str() const
{
  if (spec.mode == COVER_LENGTHS_ONLY)
    return Cover::strLength();
  else if (spec.mode == COVER_TOPS_ONLY)
    return Cover::strTop();
  else if (spec.mode == COVER_LENGTHS_OR_TOPS)
    return Cover::strLength() + ", or " + Cover::strTop();
  else if (spec.mode == COVER_LENGTHS_AND_TOPS)
    return Cover::strLength() + ", and " + Cover::strTop();
  else
    return "";
}


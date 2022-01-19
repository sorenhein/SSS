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
  spec.lengthOper = COVER_OPERATOR_SIZE;
  spec.topOper = COVER_OPERATOR_SIZE;

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
  if (spec.lengthOper == COVER_LESS_EQUAL)
    lengthFncPtr = &Cover::lessEqual;
  else if (spec.lengthOper == COVER_EQUAL)
    lengthFncPtr = &Cover::equal;
  else if (spec.lengthOper == COVER_GREATER_EQUAL)
    lengthFncPtr = &Cover::greaterEqual;
  else if (spec.lengthOper == COVER_WITHIN_RANGE)
    lengthFncPtr = &Cover::withinRange;
  else
    assert(false);

  CoverComparePtr topFncPtr = nullptr;
  if (spec.topOper == COVER_LESS_EQUAL)
    topFncPtr = &Cover::lessEqual;
  else if (spec.topOper == COVER_EQUAL)
    topFncPtr = &Cover::equal;
  else if (spec.topOper == COVER_GREATER_EQUAL)
    topFncPtr = &Cover::greaterEqual;
  else
    assert(false);

  for (unsigned i = 0; i < len; i++)
  {
    if (spec.mode == COVER_LENGTHS_ONLY)
    {
      profile[i] = (this->*lengthFncPtr)(
        lengths[i], spec.length, spec.length);
    }
    else if (spec.mode == COVER_TOPS_ONLY)
    {
      profile[i] = (this->*topFncPtr)(tops[i], spec.top, spec.length);
    }
    else if (spec.mode == COVER_LENGTHS_OR_TOPS)
    {
      profile[i] = (this->*lengthFncPtr)(
          lengths[i], spec.length, spec.length) |
          (this->*topFncPtr)(tops[i], spec.top, spec.length);
    }
    else if (spec.mode == COVER_LENGTHS_AND_TOPS)
    {
      profile[i] = (this->*lengthFncPtr)(
          lengths[i], spec.length, spec.length) &
          (this->*topFncPtr)(tops[i], spec.top, spec.length);
    }

    if (profile[i])
      weight += cases[i];
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

  if (spec.lengthOper == COVER_LESS_EQUAL)
    ss << "West has at most " << spec.length << " cards";
  else if (spec.lengthOper == COVER_EQUAL)
    ss << "West has exactly " << spec.length << " cards";
  else if (spec.lengthOper == COVER_GREATER_EQUAL)
    ss << "West has at least " << spec.length << " cards";
  
  return ss.str();
}


string Cover::strTop() const
{
  stringstream ss;

  if (spec.topOper == COVER_LESS_EQUAL)
    ss << "West has at most " << spec.top << " tops";
  else if (spec.topOper == COVER_EQUAL)
    ss << "West has exactly " << spec.top << " tops";
  else if (spec.topOper == COVER_GREATER_EQUAL)
    ss << "West has at least " << spec.top << " tops";
  
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


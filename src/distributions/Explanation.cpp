/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Explanation.h"


Explanation::Explanation()
{
  Explanation::reset();
}


void Explanation::reset()
{
  profile.clear();

  spec.mode = EXPL_MODE_SIZE;
  spec.lengthOper = EXPL_OPERATOR_SIZE;
  spec.topOper = EXPL_OPERATOR_SIZE;

  weight = 0;
}


unsigned char Explanation::lessEqual(
  const unsigned char value,
  const unsigned char ref) const
{
  return (value <= ref ? 1 : 0);
}


unsigned char Explanation::equal(
  const unsigned char value,
  const unsigned char ref) const
{
  return (value == ref ? 1 : 0);
}


unsigned char Explanation::greaterEqual(
  const unsigned char value,
  const unsigned char ref) const
{
  return (value >= ref ? 1 : 0);
}


void Explanation::prepare(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const ExplanationSpec& specIn)
{
  assert(lengths.size() == tops.size());
  const unsigned len = lengths.size();
  profile.resize(len);

  spec = specIn;

  ExplComparePtr lengthFncPtr = nullptr;
  if (spec.lengthOper == EXPL_LESS_EQUAL)
    lengthFncPtr = &Explanation::lessEqual;
  else if (spec.lengthOper == EXPL_EQUAL)
    lengthFncPtr = &Explanation::equal;
  else if (spec.lengthOper == EXPL_GREATER_EQUAL)
    lengthFncPtr = &Explanation::greaterEqual;
  else
    assert(false);

  ExplComparePtr topFncPtr = nullptr;
  if (spec.topOper == EXPL_LESS_EQUAL)
    topFncPtr = &Explanation::lessEqual;
  else if (spec.topOper == EXPL_EQUAL)
    topFncPtr = &Explanation::equal;
  else if (spec.topOper == EXPL_GREATER_EQUAL)
    topFncPtr = &Explanation::greaterEqual;
  else
    assert(false);

  for (unsigned i = 0; i < len; i++)
  {
    if (spec.mode == EXPL_LENGTHS_ONLY)
    {
      profile[i] = (this->*lengthFncPtr)(lengths[i], spec.length);
    }
    else if (spec.mode == EXPL_TOPS_ONLY)
    {
      profile[i] = (this->*topFncPtr)(tops[i], spec.top);
    }
    else if (spec.mode == EXPL_LENGTHS_OR_TOPS)
    {
      profile[i] = (this->*lengthFncPtr)(lengths[i], spec.length) |
          (this->*topFncPtr)(tops[i], spec.top);
    }
    else if (spec.mode == EXPL_LENGTHS_AND_TOPS)
    {
      profile[i] = (this->*lengthFncPtr)(lengths[i], spec.length) &
          (this->*topFncPtr)(tops[i], spec.top);
    }
  }
}


ExplanationState Explanation::explain(vector<unsigned char>& tricks) const
{
  assert(tricks.size() == profile.size());

  ExplanationState state = EXPL_DONE;

  for (unsigned i = 0; i < tricks.size(); i++)
  {
    if (profile[i] > tricks[i])
      return EXPL_IMPOSSIBLE;
    else if (profile[i] < tricks[i])
      state = EXPL_OPEN;
  }

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= profile[i];

  return state;
}


unsigned char Explanation::getWeight() const
{
  return weight;
}


string Explanation::strLength() const
{
  stringstream ss;

  if (spec.lengthOper == EXPL_LESS_EQUAL)
    ss << "West has at most " << spec.length << " cards";
  else if (spec.lengthOper == EXPL_EQUAL)
    ss << "West has exactly " << spec.length << " cards";
  else if (spec.lengthOper == EXPL_GREATER_EQUAL)
    ss << "West has at least " << spec.length << " cards";
  
  return ss.str();
}


string Explanation::strTop() const
{
  stringstream ss;

  if (spec.topOper == EXPL_LESS_EQUAL)
    ss << "West has at most " << spec.top << " tops";
  else if (spec.topOper == EXPL_EQUAL)
    ss << "West has exactly " << spec.top << " tops";
  else if (spec.topOper == EXPL_GREATER_EQUAL)
    ss << "West has at least " << spec.top << " tops";
  
  return ss.str();
}


string Explanation::str() const
{
  if (spec.mode == EXPL_LENGTHS_ONLY)
    return Explanation::strLength();
  else if (spec.mode == EXPL_TOPS_ONLY)
    return Explanation::strTop();
  else if (spec.mode == EXPL_LENGTHS_OR_TOPS)
    return Explanation::strLength() + ", or " + Explanation::strTop();
  else if (spec.mode == EXPL_LENGTHS_AND_TOPS)
    return Explanation::strLength() + ", and " + Explanation::strTop();
  else
    return "";
}


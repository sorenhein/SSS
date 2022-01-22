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
  spec.westLength.setOperator(COVER_OPERATOR_SIZE);
  spec.westTop1.setOperator(COVER_OPERATOR_SIZE);

  weight = 0;
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

  for (unsigned dno = 0; dno < len; dno++)
  {
    if (spec.mode == COVER_LENGTHS_ONLY)
    {
      profile[dno] = spec.westLength.includes(lengths[dno]);
    }
    else if (spec.mode == COVER_TOPS_ONLY)
    {
      profile[dno] = spec.westTop1.includes(tops[dno]);
    }
    else if (spec.mode == COVER_LENGTHS_OR_TOPS)
    {
      profile[dno] = 
        spec.westLength.includes(lengths[dno]) |
        spec.westTop1.includes(tops[dno]);
    }
    else if (spec.mode == COVER_LENGTHS_AND_TOPS)
    {
      profile[dno] = 
        spec.westLength.includes(lengths[dno]) &
        spec.westTop1.includes(tops[dno]);
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
  return spec.strLength();
}


string Cover::strTop() const
{
  return spec.strTop1();
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


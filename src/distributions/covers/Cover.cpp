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

  spec.setsWest[0].mode = COVER_MODE_NONE;
  spec.setsWest[1].mode = COVER_MODE_NONE;

  spec.setsWest[0].length.setOperator(COVER_OPERATOR_SIZE);
  spec.setsWest[1].length.setOperator(COVER_OPERATOR_SIZE);

  spec.setsWest[0].top1.setOperator(COVER_OPERATOR_SIZE);
  spec.setsWest[1].top1.setOperator(COVER_OPERATOR_SIZE);

  weight = 0;
  numDist = 0;
}


bool Cover::includes(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const unsigned dno,
  const unsigned specNumber)
{
  if (spec.setsWest[specNumber].mode == COVER_MODE_NONE)
  {
    return false;
  }
  else if (spec.setsWest[specNumber].mode == COVER_LENGTHS_ONLY)
  {
    return spec.includesLength(specNumber, lengths[dno]);
  }
  else if (spec.setsWest[specNumber].mode == COVER_TOPS_ONLY)
  {
    if (spec.setsWest[specNumber].symmFlag)
      return spec.setsWest[specNumber].top1.includes(tops[dno]) ||
        spec.setsWest[specNumber].top1.includes(spec.oppsTops1-tops[dno]);
    else
      return spec.setsWest[specNumber].top1.includes(tops[dno]);
  }
  else if (spec.setsWest[specNumber].mode == COVER_LENGTHS_AND_TOPS)
  {
    const bool caseWest =
      spec.setsWest[specNumber].length.includes(lengths[dno]) &&
      spec.setsWest[specNumber].top1.includes(tops[dno]);

    if (caseWest)
      return true;
    else if (spec.setsWest[specNumber].symmFlag)
      return
        spec.setsWest[specNumber].length.includes(spec.oppsLength-lengths[dno]) &&
        spec.setsWest[specNumber].top1.includes(spec.oppsTops1-tops[dno]);
    else
      return false;
  }
  else
  {
    assert(false);
    return false;
  }
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
    const bool p =
      Cover::includes(lengths, tops, dno, 0) ||
      Cover::includes(lengths, tops, dno, 1);

    if (p)
    {
      profile[dno] = 1;
      weight += cases[dno];
      numDist++;
    }
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


bool Cover::operator <= (const Cover& cover2) const
{
  assert(profile.size() == cover2.profile.size());
  for (unsigned i = 0; i < profile.size(); i++)
  {
    if (profile[i] > cover2.profile[i])
      return false;
  }

  return true;
}


void Cover::getIndices(
  unsigned char& length,
  unsigned char& tops1) const
{
  spec.getIndices(length, tops1);
}


unsigned Cover::index() const
{
  return spec.index;
}


unsigned char Cover::getWeight() const
{
  return weight;
}


unsigned char Cover::getNumDist() const
{
  return numDist;
}


string Cover::str() const
{
  return spec.str();
}


string Cover::strProfile() const
{
  stringstream ss;

  cout << "cover index " << spec.index << ", weight " << +weight << "\n";

  for (unsigned i = 0; i < profile.size(); i++)
    ss << i << ": " << +profile[i] << "\n";
  
  return ss.str();
}

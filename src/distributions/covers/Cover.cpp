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
  spec.reset();
  weight = 0;
  numDist = 0;
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
    if (spec.includes(lengths[dno], tops[dno]))
    {
      profile[dno] = 1;
      weight += static_cast<unsigned>(cases[dno]);
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
  spec.getID(length, tops1);
}


unsigned Cover::index() const
{
  return spec.getIndex();
}


unsigned Cover::getWeight() const
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

  cout << 
    "cover index " << spec.getIndex() << 
    ", weight " << weight << "\n";

  for (unsigned i = 0; i < profile.size(); i++)
    ss << i << ": " << +profile[i] << "\n";
  
  return ss.str();
}

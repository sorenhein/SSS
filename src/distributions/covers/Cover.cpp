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
#include "CoverSpec.h"
#include "Profile.h"


Cover::Cover()
{
  Cover::reset();
}


void Cover::reset()
{
  tricks.clear();
  specPtr = nullptr;
  weight = 0;
  numDist = 0;
}


void Cover::prepare(
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases,
  const CoverSpec& specIn)
{
  tricks.prepare(specIn, distProfiles, cases, weight, numDist);

  specPtr = &specIn;
}


CoverState Cover::explain(Tricks& tricksSeen) const
{
  return tricks.explain(tricksSeen);
}


bool Cover::operator <= (const Cover& cover2) const
{
  return (tricks <= cover2.tricks);
}


void Cover::getID(
  unsigned char& length,
  unsigned char& tops1) const
{
  assert(specPtr != nullptr);
  specPtr->getID(length, tops1);
}


unsigned Cover::index() const
{
  assert(specPtr != nullptr);
  return specPtr->getIndex();
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
  assert(specPtr != nullptr);
  return specPtr->str();
}


string Cover::strProfile() const
{
  assert(specPtr != nullptr);
  stringstream ss;

  cout << 
    "cover index " << specPtr->getIndex() << 
    ", weight " << weight << "\n";

  ss << tricks.strList();
  
  return ss.str();
}

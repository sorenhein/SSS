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
#include "Profile.h"


Cover::Cover()
{
  Cover::reset();
}


void Cover::reset()
{
  tricks.clear();
  weight = 0;
  numDist = 0;
}


void Cover::prepare(
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases,
  const CoverSpec& specIn)
{
  indexInternal = specIn.index;
  sumProfile = specIn.sumProfile;
  setsWest = specIn.setsWest;

  tricks.prepare(* this, distProfiles, cases, weight, numDist);
}


bool Cover::includes(const Profile& distProfile) const
{
  assert(distProfile.size() == 2);
  assert(sumProfile.size() == 2);

  for (auto& set: setsWest)
  {
    if (set.product.includes(distProfile))
      return true;
    else if (! set.symmFlag)
      continue;
    else
    {
      Profile mirror = distProfile;
      mirror.mirrorAround(sumProfile);

      if (set.product.includes(mirror))
        return true;
    }
  }

  return false;
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
  length = sumProfile.getLength();
  tops1 = sumProfile.count(1);
}


unsigned Cover::index() const
{
  return indexInternal;
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
  string s = setsWest.front().strVerbal(sumProfile);

  for (auto iter = next(setsWest.begin()); iter != setsWest.end(); iter++)
    s += "; or\n  " + iter->strVerbal(sumProfile);

  return s;
}


string Cover::strProfile() const
{
  stringstream ss;

  cout << 
    "cover index " << indexInternal << ", weight " << weight << "\n";

  ss << tricks.strList();
  
  return ss.str();
}

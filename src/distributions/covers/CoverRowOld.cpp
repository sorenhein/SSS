/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverRowOld.h"
#include "Profile.h"


CoverRowOld::CoverRowOld()
{
  CoverRowOld::reset();
}


void CoverRowOld::reset()
{
  tricks.clear();
  weight = 0;
  numDist = 0;
}


void CoverRowOld::prepare(
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases,
  const CoverSpec& specIn)
{
  indexInternal = specIn.index;
  sumProfile = specIn.sumProfile;
  covers = specIn.setsWest;

  // Individual covers
  for (auto& cover: covers)
    cover.prepare(distProfiles, cases);

  // The whole row
  // TODO Can't we just OR together the cover's?
  tricks.prepare(* this, distProfiles, cases, weight, numDist);
}


bool CoverRowOld::includes(const Profile& distProfile) const
{
  assert(distProfile.size() == sumProfile.size());

  for (auto& cover: covers)
  {
    if (cover.includes(distProfile))
      return true;
    else if (! cover.symmetric())
      continue;
    else
    {
      Profile mirror = distProfile;
      mirror.mirrorAround(sumProfile);

      if (cover.includes(mirror))
        return true;
    }
  }

  return false;
}


CoverState CoverRowOld::explain(Tricks& tricksSeen) const
{
  return tricks.explain(tricksSeen);
}


bool CoverRowOld::operator <= (const CoverRowOld& cover2) const
{
  return (tricks <= cover2.tricks);
}


void CoverRowOld::getID(
  unsigned char& length,
  unsigned char& tops1) const
{
  length = sumProfile.getLength();
  tops1 = sumProfile.count(
    static_cast<unsigned char>(sumProfile.size()-1));
}


unsigned CoverRowOld::index() const
{
  return indexInternal;
}


unsigned CoverRowOld::getWeight() const
{
  return weight;
}


unsigned char CoverRowOld::getNumDist() const
{
  return numDist;
}


string CoverRowOld::str() const
{
  string s = covers.front().str(sumProfile);

  for (auto iter = next(covers.begin()); iter != covers.end(); iter++)
    s += "; or\n  " + iter->str(sumProfile);

  return s;
}


string CoverRowOld::strProfile() const
{
  stringstream ss;

  cout << 
    "cover index " << indexInternal << ", weight " << weight << "\n";

  ss << tricks.strList();
  
  return ss.str();
}

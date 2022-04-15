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
#include "Manual.h"


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


void CoverRowOld::resize(const unsigned len)
{
  tricks.resize(len);
}


void CoverRowOld::add(
  const Cover& cover,
  const bool symmFlag, // Unnecessary later -- already in cover
  const Profile& sumProfileIn,
  const unsigned indexIn)
{
  indexInternal = indexIn;
  sumProfile = sumProfileIn; // Both are duplicative

  covers.emplace_back(cover);
  covers.back().setSymmetric(symmFlag);
  covers.back().tricksOr(tricks);
}


void CoverRowOld::weigh(const vector<unsigned char>& cases)
{
  tricks.weigh(cases, weight, numDist);
  assert(weight != 0);
}


CoverState CoverRowOld::explain(Tricks& tricksSeen) const
{
  return tricks.explain(tricksSeen);
}


bool CoverRowOld::operator <= (const CoverRowOld& cover2) const
{
  return (tricks <= cover2.tricks);
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

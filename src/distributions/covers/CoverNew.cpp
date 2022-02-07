/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverNew.h"


CoverNew::CoverNew()
{
  CoverNew::reset();
}


void CoverNew::reset()
{
  profile.clear();
  weight = 0;
  numDist = 0;
}


void CoverNew::prepare(
  const vector<unsigned char>& lengths,
  const vector<vector<unsigned char> const *>& topPtrs,
  const vector<unsigned char>& cases)
{
  const unsigned len = lengths.size();
  assert(len == topPtrs.size());
  assert(len == cases.size());
  profile.resize(len);

  for (unsigned dno = 0; dno < len; dno++)
  {
    if (coverSet.includes(lengths[dno], * (topPtrs[dno])))
    {
      profile[dno] = 1;
      weight += static_cast<unsigned>(cases[dno]);
      numDist++;
    }
  }
}


CoverState CoverNew::explain(vector<unsigned char>& tricks) const
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


bool CoverNew::operator <= (const CoverNew& cover2) const
{
  assert(profile.size() == cover2.profile.size());
  for (unsigned i = 0; i < profile.size(); i++)
  {
    if (profile[i] > cover2.profile[i])
      return false;
  }

  return true;
}


unsigned CoverNew::getWeight() const
{
  return weight;
}


unsigned char CoverNew::getNumDist() const
{
  return numDist;
}


string CoverNew::str(const unsigned char lengthActual) const
{
  return coverSet.strLine(lengthActual);
}


string CoverNew::strProfile() const
{
  stringstream ss;

  cout << "weight " << weight << "\n";

  for (unsigned i = 0; i < profile.size(); i++)
    ss << i << ": " << +profile[i] << "\n";
  
  return ss.str();
}

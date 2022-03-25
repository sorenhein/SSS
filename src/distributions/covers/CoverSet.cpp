/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverSet.h"
#include "Profile.h"


void CoverSet::reset()
{
  symmFlag = false;
  product.reset();
}


void CoverSet::set(
  const Profile& sumProfile,
  const Profile& lowerProfile,
  const Profile& upperProfile,
  const bool symmFlagIn)
{
  symmFlag = symmFlagIn;

  assert(lowerProfile.tops.size() == 2);
  assert(upperProfile.tops.size() == 2);

  product.resize(2);
  product.set(sumProfile, lowerProfile, upperProfile);
}


bool CoverSet::includes(
  const Profile& distProfile,
  const Profile& sumProfile) const
{
  if (product.includes(distProfile))
  {
    return true;
  }
  else if (! symmFlag)
  {
    return false;
  }
  else
  {
    Profile mirror = distProfile;
    mirror.tops.resize(sumProfile.tops.size()); // 2
    mirror.mirror(sumProfile);

    return product.includes(mirror);
  }
}


string CoverSet::str(const Profile& sumProfile) const
{
  const Opponent simplestOpponent = product.simplestOpponent(sumProfile);

  return product.strVerbal(sumProfile, simplestOpponent, symmFlag);
}


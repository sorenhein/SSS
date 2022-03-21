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
#include "ProductProfile.h"


void CoverSet::reset()
{
  symmFlag = false;
  product.reset();
}


void CoverSet::set(
  const ProductProfile& sumProfile,
  const ProductProfile& lowerProfile,
  const ProductProfile& upperProfile,
  const bool symmFlagIn)
{
  symmFlag = symmFlagIn;

  assert(lowerProfile.tops.size() == 1);
  assert(upperProfile.tops.size() == 1);

  ProductProfile sum2, lower2, upper2;
  sum2.tops.resize(2);
  lower2.tops.resize(2);
  upper2.tops.resize(2);
  
  sum2.length = sumProfile.length;
  lower2.length = lowerProfile.length;
  upper2.length = upperProfile.length;

  sum2.tops[1] = sumProfile.tops[0];
  lower2.tops[1] = lowerProfile.tops[0];
  upper2.tops[1] = upperProfile.tops[0];

  product.resize(2);
  product.set(sum2, lower2, upper2);
}


bool CoverSet::includes(
  const ProductProfile& distProfile,
  const ProductProfile& sumProfile) const
{
  ProductProfile dist2;
  dist2.tops.resize(2);
  dist2.length = distProfile.length;
  dist2.tops[1] = distProfile.tops[0];

  if (product.includes(dist2))
  {
    return true;
  }
  else if (! symmFlag)
  {
    return false;
  }
  else
  {
    ProductProfile mirror = distProfile;
    mirror.mirror(sumProfile);

    ProductProfile mirror2;
    mirror2.tops.resize(2);
    mirror2.length = mirror.length;
    mirror2.tops[1] = mirror.tops[0];

    return product.includes(mirror2);
  }
}


string CoverSet::str(const ProductProfile& sumProfile) const
{
  // Figure out simplestOpponent analogously to ProductProfile.

  ProductProfile sum2;
  sum2.tops.resize(2);
  sum2.length = sumProfile.length;
  sum2.tops[1] = sumProfile.tops[0];

  const Opponent simplestOpponent = product.simplestOpponent(sum2);

  return product.strVerbal(sum2, simplestOpponent, symmFlag);
}


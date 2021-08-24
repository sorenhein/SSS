/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include "Extension.h"
#include "Split.h"
#include "../Slist.h"


Extension::Extension()
{
  Extension::reset();
}


Extension::~Extension()
{
}


void Extension::reset()
{
  overlap.reset();
  own1ptr = nullptr;
  own2ptr = nullptr;
  weightOverall = 0;
}


void Extension::multiply(
  const Split& split1,
  const Split& split2,
  const Ranges& ranges)
{
  overlap.multiply(* split1.sharedPtr, * split2.sharedPtr);
  overlap.scrutinize(ranges);

  own1ptr = split1.ownPtr;
  own2ptr = split2.ownPtr;

  weightOverlap = overlap.weight();
  weightOwn1 = own1ptr->weight();
  weightOwn2 = own2ptr->weight();

  weightOverall = weightOverlap + weightOwn1 + weightOwn2;
}


void Extension::flatten(Slist& slist)
{
  overlap *= * own1ptr;
  overlap *= * own2ptr;
  slist.push_back(move(overlap));
}


bool Extension::weightLessEqual(const Extension& ext2) const
{
  return (weightOwn1 <= ext2.weightOwn1 &&
      weightOwn2 <= ext2.weightOwn2 &&
      weightOverlap <= ext2.weightOverlap);
}


bool Extension::lessEqualPrimary(const Extension& ext2) const
{
  if (! Extension::weightLessEqual(ext2))
    return false;
  else if (! own1ptr->lessEqualPrimaryScrutinized(* ext2.own1ptr))
    return false;
  else if (! own2ptr->lessEqualPrimaryScrutinized(* ext2.own2ptr))
    return false;
  else
    return overlap.lessEqualPrimaryScrutinized(ext2.overlap);
}


Compare Extension::compareSecondary(const Extension& ext2) const
{
  Compare c = own1ptr->compareSecondary(* ext2.own1ptr);
  c *= own2ptr->compareSecondary(* ext2.own2ptr);
  c *= overlap.compareSecondary(ext2.overlap);
  
  return c;
}


unsigned Extension::weight() const
{
  return weightOverall;
}


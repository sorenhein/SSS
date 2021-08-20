/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include "Extension.h"


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
  weightInt = 0;
}


void Extension::multiply(
  const Strategy& strat1,
  const Strategy& strat2,
  const Ranges& ranges)
{
  overlap.multiply(strat1, strat2);
  overlap.scrutinize(ranges);
}


void Extension::finish(
  const unsigned index1,
  const unsigned index2,
  const unsigned weightUniques)
{
  index1Int = index1;
  index2Int = index2;

  weightInt = overlap.weight() + weightUniques;
}


void Extension::flatten(
  list<Strategy>& strategies,
  const Strategy& strat1,
  const Strategy& strat2)
{
  overlap *= strat1;
  overlap *= strat2;
  strategies.push_back(move(overlap));
}


Compare Extension::compareSecondary(const Extension& ext2) const
{
  return overlap.compareSecondary(ext2.overlap);
}


bool Extension::lessEqualPrimary(const Extension& ext2) const
{
  return overlap.lessEqualPrimaryScrutinized(ext2.overlap);
}


unsigned Extension::index1() const
{
  return index1Int;
}


unsigned Extension::index2() const
{
  return index2Int;
}


unsigned Extension::weight() const
{
  return weightInt;
}


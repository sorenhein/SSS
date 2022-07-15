/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "PartialVoid.h"

using namespace std;


PartialVoid::PartialVoid()
{
  coverPtr = nullptr;
  additions.clear();
  rawWeightAdder = 0;
  lengthWestInt = 0;
  repeatsInt = 0;
}


void PartialVoid::setVoid(
  const unsigned lengthWestIn,
  const unsigned repeatsIn)
{
  lengthWestInt = lengthWestIn;
  repeatsInt = repeatsIn;
}


unsigned PartialVoid::lengthWest() const
{
  return lengthWestInt;
}


unsigned PartialVoid::repeats() const
{
  return repeatsInt;
}


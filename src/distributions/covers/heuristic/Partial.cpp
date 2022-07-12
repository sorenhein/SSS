/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "Partial.h"

#include "../Cover.h"
#include "../Complexity.h"

using namespace std;


Partial::Partial()
{
  coverPtr = nullptr;
  additions.clear();
  rawWeightAdder = 0;
}


void Partial::set(
  Cover const * coverPtrIn,
  const Tricks& additionsIn,
  const unsigned rawWeightAdderIn)
{
  coverPtr = coverPtrIn;
  additions = additionsIn;
  rawWeightAdder = rawWeightAdderIn;
}


bool Partial::operator < (const Partial& partial2) const
{
  return (rawWeightAdder < partial2.rawWeightAdder);
}


bool Partial::empty() const
{
  return (coverPtr == nullptr);
}


void Partial::addCoverToComplexity(Complexity& complexity) const
{
  // Add cover to a single row.
  assert(coverPtr != nullptr);
  complexity.addCoverSingleRow(
    coverPtr->getComplexity(),
    additions.getWeight());
}


void Partial::addRowToComplexity(Complexity& complexity) const
{
  // Add a whole new row.
  assert(coverPtr != nullptr);
  complexity.addRow(
    coverPtr->getComplexity(),
    additions.getWeight());
}


const Cover& Partial::cover() const
{
  assert(coverPtr != nullptr);
  return * coverPtr;
}


Cover const * Partial::coverPointer() const
{
  return coverPtr;
}


Tricks& Partial::tricks()
{
  return additions;
}


const Tricks& Partial::tricks() const
{
  return additions;
}


unsigned Partial::weight() const
{
  return rawWeightAdder;
}


string Partial::str() const
{
  if (coverPtr == nullptr)
    return nullptr;

  string s = coverPtr->strNumerical();
  s += additions.strSpaced();
  s += "Weight " + to_string(rawWeightAdder) + "\n";
  return s;
}


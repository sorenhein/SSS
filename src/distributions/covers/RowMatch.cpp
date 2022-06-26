/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <sstream>
#include <cassert>

#include "RowMatch.h"
#include "CoverRow.h"
#include "Tricks.h"


using namespace std;


void RowMatch::set(
  CoverRow const * rowPtrIn,
  const size_t westLength,
  const Tricks& tricksIn)
{
  rowPtr = rowPtrIn;

  count = 1;
  lengthFirst = westLength;
  lengthLast = westLength;

  tricks = tricksIn;
}


void RowMatch::add(const Tricks& tricksIn)
{
  assert(count > 0);
  count++;
  lengthLast++;
  tricks += tricksIn;
}


bool RowMatch::contiguous(const size_t westLength) const
{
  return (lengthLast + 1 == westLength);
}


bool RowMatch::singleCount() const
{
  return (count == 1);
}

    
const CoverRow& RowMatch::getSingleRow() const
{
  assert(rowPtr != nullptr);
  return * rowPtr;
}


const Tricks& RowMatch::getTricks() const
{
  return tricks;
}


string RowMatch::str() const
{
  stringstream ss;

  ss << "Count  " << count << "\n";
   if (lengthFirst == lengthLast)
    ss << "Length " << lengthFirst << "\n";
  else
    ss << "Length " << lengthFirst << "-" << lengthLast << "\n";

  ss << "Tricks\n";
  ss << tricks.strList() << "\n";
  ss << rowPtr->strNumerical() << "\n";
  return ss.str();
}

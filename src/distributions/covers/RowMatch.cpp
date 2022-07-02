/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <sstream>
#include <cassert>

#include "RowMatch.h"
#include "Tricks.h"
#include "product/Profile.h"

#include "../../utils/table.h"


using namespace std;


void RowMatch::setLengthsByTops(const Profile& sumProfile)
{
  lengthByTopsFirst = row.minimumByTops(OPP_WEST, sumProfile);

  lengthByTopsLast = sumProfile.length() -
    row.minimumByTops(OPP_EAST, sumProfile);
}


void RowMatch::transfer(
  CoverRow& rowIn,
  const size_t westLength)
{
  // rowIn gets invalidated!
  row = move(rowIn);

  count = 1;
  lengthFirst = westLength;
  lengthLast = westLength;

  tricks = row.getTricks();
}


void RowMatch::add(
  const Tricks& tricksIn,
  const Opponent towardVoid)
{
  assert(count > 0);
  count++;
  tricks += tricksIn;

  if (towardVoid == OPP_WEST)
    lengthFirst--;
  else
    lengthLast++;
}


bool RowMatch::contiguous(
  const size_t westLength,
  const Opponent towardVoid) const
{
  if (towardVoid == OPP_EAST)
  {
    // Toward higher West counts.
    return (lengthLast + 1 == westLength);
  }
  else
  {
    // Toward lower West counts.
    return (lengthFirst == westLength + 1);
  }
}


bool RowMatch::possible(
  const size_t westLength,
  const Opponent towardVoid) const
{
  if (towardVoid == OPP_EAST)
  {
    // Toward higher West counts.
    return (lengthLast + 1 == westLength &&
        westLength <= lengthByTopsLast);
  }
  else
  {
    // Toward lower West counts.
    return (lengthFirst == westLength + 1 &&
        westLength >= lengthByTopsFirst);
  }
}


bool RowMatch::preferred(
  const size_t westLength,
  const Opponent towardVoid) const
{
  if (towardVoid == OPP_EAST)
  {
    // Toward higher West counts.
    return (lengthFirst == lengthByTopsFirst &&
        lengthLast + 1 == lengthByTopsLast &&
        westLength == lengthByTopsLast);
  }
  else
  {
    // Toward lower West counts.
    return (lengthLast == lengthByTopsLast &&
        lengthFirst == lengthByTopsFirst + 1 &&
        westLength == lengthByTopsFirst);
  }
}


bool RowMatch::singleCount() const
{
  return (count == 1);
}

    
const CoverRow& RowMatch::getSingleRow() const
{
  return row;
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
  ss << row.strNumerical() << "\n";
  return ss.str();
}

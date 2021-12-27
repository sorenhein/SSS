/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Result.h"

#include "../../plays/Play.h"
#include "../../inputs/Control.h"

extern Control control;


// First three non-class methods. 

CompareType compressCore(const unsigned detail)
{
  if (detail & WIN_DIFFERENT_SECONDARY)
    return WIN_DIFFERENT;

  if (detail & WIN_FIRST_SECONDARY)
  {
    if (detail & WIN_SECOND_SECONDARY)
      return WIN_DIFFERENT;
    else
      return WIN_FIRST;
  }
  else if (detail & WIN_SECOND_SECONDARY)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


CompareType compressCompareDetail(const unsigned detail)
{
  if (detail & WIN_DIFFERENT_PRIMARY)
    return WIN_DIFFERENT;

  if (detail & WIN_FIRST_PRIMARY)
  {
    if (detail & WIN_SECOND_PRIMARY)
      return WIN_DIFFERENT;
    else
      return WIN_FIRST;
  }
  else if (detail & WIN_SECOND_PRIMARY)
    return WIN_SECOND;

  return compressCore(detail);
}


CompareType compressCompareSecondaryDetail(const unsigned detail)
{
  assert((detail & WIN_PRIMARY) == 0);
  return compressCore(detail);
}


Result::Result()
{
  distInt = 0;
  tricksInt = 0;
}


void Result::set(
  const unsigned char dist,
  const unsigned char tricks,
  const Winners& winners)
{
  distInt = dist;
  tricksInt = tricks;

  if (control.runRankComparisons())
    winnersInt = winners;
}


void Result::setDist(const unsigned char dist)
{
  distInt = dist;
}


void Result::setTricks(const unsigned char tricks)
{
  tricksInt = tricks;
  winnersInt.setEmpty();
}


void Result::update(
  const Play& play,
  const unsigned char dist)
{
  distInt = dist;
  tricksInt += play.trickNS;

  if (control.runRankComparisons())
    winnersInt.update(play);
}


void Result::expand(
  const unsigned char dist,
  const char rankAdder)
{
  // This is used to expand a minimal strategy to a general one.
  distInt = dist;
  winnersInt.expand(rankAdder);
}


void Result::flip()
{
  if (control.runRankComparisons())
    winnersInt.flip();
}


void Result::multiplyWinnersOnto(Result& result) const
{
  if (control.runRankComparisons())
  {
    result.winnersInt *= winnersInt;
  }
  
}


void Result::operator *= (const Result& result2)
{
  // Keep the "lower" one.
  if (tricksInt > result2.tricksInt)
    * this = result2;
  else if (tricksInt == result2.tricksInt)
  {
    if (control.runRankComparisons())
      winnersInt *= result2.winnersInt;
  }
}


void Result::operator += (const Result& result2)
{
  // Keep the "upper" one.
  if (tricksInt < result2.tricksInt)
    * this = result2;
  else if (tricksInt == result2.tricksInt)
  {
    if (control.runRankComparisons())
      winnersInt += result2.winnersInt;
  }
}

  
bool Result::operator == (const Result& res2) const
{
  if (tricksInt != res2.tricksInt)
    return false;
  else if (! control.runRankComparisons())
    return true;
  else
    return (winnersInt.compare(res2.winnersInt) == WIN_EQUAL);
}


bool Result::operator != (const Result& res2) const
{
  return ! (* this == res2);
}


CompareDetail Result::comparePrimaryInDetail(const Result& res2) const
{
  if (tricksInt > res2.tricksInt)
    return WIN_FIRST_PRIMARY;
  else if (tricksInt < res2.tricksInt)
    return WIN_SECOND_PRIMARY;
  else
    return WIN_EQUAL_OVERALL;
}


Compare Result::compareComplete(const Result& res2) const
{
  if (tricksInt > res2.tricksInt)
    return WIN_FIRST;
  else if (tricksInt < res2.tricksInt)
    return WIN_SECOND;
  else if (! control.runRankComparisons())
    return WIN_EQUAL;
  else
    return winnersInt.compare(res2.winnersInt);
}


CompareDetail Result::compareSecondaryInDetail(const Result& res2) const
{
  assert(tricksInt == res2.tricksInt);

  if (! control.runRankComparisons())
    return WIN_EQUAL_OVERALL;

  const Compare c = winnersInt.compare(res2.winnersInt);
  if (c == WIN_FIRST)
    return WIN_FIRST_SECONDARY;
  else if (c == WIN_SECOND)
    return WIN_SECOND_SECONDARY;
  else if (c == WIN_EQUAL)
    return WIN_EQUAL_OVERALL;
  else
    return WIN_DIFFERENT_SECONDARY;
}


CompareDetail Result::compareInDetail(const Result& res2) const
{
  if (tricksInt > res2.tricksInt)
    return WIN_FIRST_PRIMARY;
  else if (tricksInt < res2.tricksInt)
    return WIN_SECOND_PRIMARY;
  else if (! control.runRankComparisons())
    return WIN_EQUAL_OVERALL;
  else
    return Result::compareSecondaryInDetail(res2);
}


unsigned char Result::dist() const
{
  return distInt;
}


unsigned char Result::tricks() const
{
  return tricksInt;
}


unsigned char Result::rank() const
{
  if (control.runRankComparisons())
    return winnersInt.rank();
  else
    return 0;
}


unsigned char Result::winAbsNumber() const
{
  if (control.runRankComparisons())
    return winnersInt.absNumber();
  else
    return 0;
}


string Result::strHeaderEntry(
  const bool rankFlag,
  const string& title) const
{
  stringstream ss;
  if (rankFlag)
    ss << 
      setw(4) << "Tr" <<
      setw(8) << (title == "" ? "Win" : title);
  else
    ss << setw(4) << (title == "" ? "Tr" : title);

  return ss.str();
}


string Result::strEntry(const bool rankFlag) const
{
  stringstream ss;
  ss << setw(4) << +tricksInt;

  if (rankFlag)
    ss << setw(8) << winnersInt.strEntry();

  return ss.str();
}


string Result::strWinners() const
{
  return winnersInt.strEntry();
}


string Result::strHeader(const string& title) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";

  ss <<
    setw(4) << left << "Dist" <<
    setw(6) << "Tricks" << "\n";

  return ss.str();
}


string Result::str(const bool rankFlag) const
{
  stringstream ss;
  ss << 
    setw(4) << +distInt << 
    setw(6) << Result::strEntry(rankFlag) << "\n";

  return ss.str();
}


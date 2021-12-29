/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "MultiResult.h"
#include "Result.h"

#include "../../plays/Play.h"
#include "../../inputs/Control.h"

extern Control control;


MultiResult::MultiResult()
{
  distInt = 0;
  tricksInt = 0;
}


MultiResult& MultiResult::operator = (const Result& res)
{
  distInt = res.distInt;
  tricksInt = res.tricksInt;
  winnersInt = res.winnersInt;
  return * this;

  // TODO When Result moves to a single winner, this should be
  // something like winnersInt.push_back(res.winner)
}


void MultiResult::operator *= (const MultiResult& multiResult)
{
  // Keep the "lower" one.
  if (tricksInt > multiResult.tricksInt)
    * this = multiResult;
  else if (tricksInt == multiResult.tricksInt)
  {
    if (control.runRankComparisons())
      winnersInt *= multiResult.winnersInt;
  }
}


void MultiResult::operator *= (const Result& result)
{
  // Keep the "lower" one.
  if (tricksInt > result.tricksInt)
    * this = result;
  else if (tricksInt == result.tricksInt)
  {
    if (control.runRankComparisons())
      winnersInt *= result.winnersInt;
  }
}


void MultiResult::operator += (const MultiResult& multiResult)
{
  // Keep the "upper" one.
  if (tricksInt < multiResult.tricksInt)
    * this = multiResult;
  else if (tricksInt == multiResult.tricksInt)
  {
    if (control.runRankComparisons())
      winnersInt += multiResult.winnersInt;
  }
}

  
void MultiResult::operator += (const Result& result)
{
  // Keep the "upper" one.
  if (tricksInt < result.tricksInt)
    * this = result;
  else if (tricksInt == result.tricksInt)
  {
    if (control.runRankComparisons())
      winnersInt += result.winnersInt;
  }
}

  
bool MultiResult::operator == (const MultiResult& multiResult) const
{
  if (tricksInt != multiResult.tricksInt)
    return false;
  else if (! control.runRankComparisons())
    return true;
  else
    return (winnersInt.compare(multiResult.winnersInt) == WIN_EQUAL);
}


bool MultiResult::operator != (const MultiResult& multiResult) const
{
  return ! (* this == multiResult);
}


Compare MultiResult::compareComplete(const MultiResult& res2) const
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


Result MultiResult::result() const
{
  Result res;
  
  // TODO When Result moves to a single winner, winnersInt.front()
  // and assert size == 1.
  res.set(distInt, tricksInt, winnersInt);
  return res;
}

unsigned char MultiResult::dist() const
{
  return distInt;
}


unsigned char MultiResult::tricks() const
{
  return tricksInt;
}


string MultiResult::strHeaderEntry(
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


string MultiResult::strEntry(const bool rankFlag) const
{
  stringstream ss;
  ss << setw(4) << +tricksInt;

  if (rankFlag)
    ss << setw(8) << winnersInt.strEntry();

  return ss.str();
}


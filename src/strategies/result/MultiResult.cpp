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

#include "../../inputs/Control.h"

extern Control control;


MultiResult::MultiResult()
{
  dist = 0;
  tricks = 0;
}


MultiResult& MultiResult::operator = (const Result& result)
{
  dist = result.getDist();
  tricks = result.getTricks();
  winners.set(result.getWinner());
  return * this;
}


void MultiResult::operator *= (const MultiResult& multiResult)
{
  // Keep the "lower" one.
  if (tricks > multiResult.tricks)
    * this = multiResult;
  else if (tricks == multiResult.tricks)
  {
    if (control.runRankComparisons())
      winners *= multiResult.winners;
  }
}


void MultiResult::operator *= (const Result& result)
{
  // Keep the "lower" one.
  if (tricks > result.getTricks())
    * this = result;
  else if (tricks == result.getTricks())
  {
    if (control.runRankComparisons())
      winners *= result.getWinner();
  }
}


void MultiResult::operator += (const MultiResult& multiResult)
{
  // Keep the "upper" one.
  if (tricks < multiResult.tricks)
    * this = multiResult;
  else if (tricks == multiResult.tricks)
  {
    if (control.runRankComparisons())
      winners += multiResult.winners;
  }
}

  
void MultiResult::operator += (const Result& result)
{
  // Keep the "upper" one.
  if (tricks < result.getTricks())
    * this = result;
  else if (tricks == result.getTricks())
  {
    if (control.runRankComparisons())
      winners += result.getWinner();
  }
}

  
bool MultiResult::operator == (const MultiResult& multiResult) const
{
  if (tricks != multiResult.tricks)
    return false;
  else if (! control.runRankComparisons())
    return true;
  else
    return (winners.compare(multiResult.winners) == WIN_EQUAL);
}


bool MultiResult::operator != (const MultiResult& multiResult) const
{
  return ! (* this == multiResult);
}


Compare MultiResult::compareComplete(const MultiResult& multiResult) const
{
  if (tricks > multiResult.tricks)
    return WIN_FIRST;
  else if (tricks < multiResult.tricks)
    return WIN_SECOND;
  else if (! control.runRankComparisons())
    return WIN_EQUAL;
  else
    return winners.compare(multiResult.winners);
}


void MultiResult::constantResult(Result& result) const
{
  result.set(dist, tricks, winners.constantWinner());
}


unsigned char MultiResult::getDist() const
{
  return dist;
}


unsigned char MultiResult::getTricks() const
{
  return tricks;
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
  ss << setw(4) << +tricks;

  if (rankFlag)
    ss << setw(8) << winners.strEntry();

  return ss.str();
}


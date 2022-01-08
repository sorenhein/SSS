/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

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


Result::Result()
{
  dist = 0;
  tricks = 0;
}


void Result::set(
  const unsigned char distIn,
  const unsigned char tricksIn,
  const Winner& winnerIn)
{
  dist = distIn;
  tricks = tricksIn;

  if (control.runRankComparisons())
    winner = winnerIn;
}


void Result::setDist(const unsigned char distIn)
{
  dist = distIn;
}


void Result::setTricks(const unsigned char tricksIn)
{
  tricks = tricksIn;
}


void Result::update(
  const Play& play,
  const unsigned char distIn)
{
  dist = distIn;
  tricks += play.trickNS;

  if (control.runRankComparisons())
  {
    winner.update(play);

    if (play.trickNS)
      winner *= play.currBest;
  }
}


void Result::expand(
  const unsigned char distIn,
  const char rankAdder)
{
  // This is used to expand a minimal strategy to a general one.
  dist = distIn;
  winner.expand(rankAdder);
}


void Result::flip()
{
  if (control.runRankComparisons())
    winner.flip();
}


void Result::multiplyWinnersOnto(Result& result) const
{
  if (control.runRankComparisons())
    result.winner *= winner;
}


void Result::operator *= (const Result& result)
{
  // Keep the "lower" one.
  if (tricks > result.tricks)
    * this = result;
  else if (tricks == result.tricks)
  {
    if (control.runRankComparisons())
      winner *= result.winner;
  }
}


bool Result::operator == (const Result& result) const
{
  if (tricks != result.tricks)
    return false;
  else if (! control.runRankComparisons())
    return true;
  else
    return (winner.compare(result.winner) == WIN_EQUAL);
}


bool Result::operator != (const Result& result) const
{
  return ! (* this == result);
}


CompareDetail Result::comparePrimaryInDetail(const Result& result) const
{
  if (tricks > result.tricks)
    return WIN_FIRST_PRIMARY;
  else if (tricks < result.tricks)
    return WIN_SECOND_PRIMARY;
  else
    return WIN_EQUAL_OVERALL;
}


Compare Result::compareComplete(const Result& result) const
{
  if (tricks > result.tricks)
    return WIN_FIRST;
  else if (tricks < result.tricks)
    return WIN_SECOND;
  else if (! control.runRankComparisons())
    return WIN_EQUAL;
  else
    return winner.compare(result.winner);
}


CompareDetail Result::compareSecondaryInDetail(const Result& result) const
{
  assert(tricks == result.tricks);

  if (! control.runRankComparisons())
    return WIN_EQUAL_OVERALL;

  const Compare c = winner.compare(result.winner);

  if (c == WIN_FIRST)
    return WIN_FIRST_SECONDARY;
  else if (c == WIN_SECOND)
    return WIN_SECOND_SECONDARY;
  else if (c == WIN_EQUAL)
    return WIN_EQUAL_OVERALL;
  else
    return WIN_DIFFERENT_SECONDARY;
}


CompareDetail Result::compareInDetail(const Result& result) const
{
  if (tricks > result.tricks)
    return WIN_FIRST_PRIMARY;
  else if (tricks < result.tricks)
    return WIN_SECOND_PRIMARY;
  else if (! control.runRankComparisons())
    return WIN_EQUAL_OVERALL;
  else
    return Result::compareSecondaryInDetail(result);
}


unsigned char Result::getDist() const
{
  return dist;
}


unsigned char Result::getTricks() const
{
  return tricks;
}


unsigned char Result::getRank() const
{
  if (control.runRankComparisons())
    return winner.getRank();
  else
    return 0;
}


unsigned char Result::winAbsNumber() const
{
  if (control.runRankComparisons())
    return winner.getAbsNumber();
  else
    return 0;
}


const Winner& Result::getWinner() const
{
 return winner;
}


string Result::strEntry(const bool rankFlag) const
{
  stringstream ss;
  if (rankFlag)
  {
    ss << setw(6) << +tricks;
    ss << setw(4) << winner.str();
  }
  else
    ss << setw(4) << +tricks;

  return ss.str();
}


string Result::strWinners() const
{
  return winner.str();
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
    setw(4) << +dist << 
    Result::strEntry(rankFlag) << "\n";

  return ss.str();
}


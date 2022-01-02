/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Winner.h"

#include "../../plays/Play.h"
#include "../../const.h"

#define WIN_NORTH_SET 0x1
#define WIN_SOUTH_SET 0x2


Winner::Winner()
{
  Winner::reset();
}


void Winner::reset()
{
  north.reset();
  south.reset();
  mode = UCHAR_NOT_SET;
  rank = UCHAR_NOT_SET;
}


void Winner::set(
  const Side sideIn,
  const Card& card)
{
  if (sideIn != SIDE_NONE)
    rank = card.getRank();

  if (sideIn == SIDE_NORTH)
  {
    north = card;
    south.reset();
    mode = WIN_NORTH_ONLY;
  }
  else if (sideIn == SIDE_SOUTH)
  {
    north.reset();
    south = card;
    mode = WIN_SOUTH_ONLY;
  }
  else if (sideIn == SIDE_NONE)
    assert(false);
}


void Winner::setHigherOf(
  const Card& northIn,
  const Card& southIn)
{
  Winner::reset();

  if (northIn > southIn)
    Winner::set(SIDE_NORTH, northIn);
  else
    Winner::set(SIDE_SOUTH, southIn);
}


void Winner::setBoth(
  const Card& northIn,
  const Card& southIn)
{
  Winner::reset();

  north = northIn;
  south = southIn;
  mode = WIN_BOTH;
  rank = northIn.getRank(); // Should be same for both sides
}


bool Winner::operator == (const Winner& winner2) const
{
  if (mode != winner2.mode)
    return false;

  if (mode == UCHAR_NOT_SET)
    return true;

  if ((mode & WIN_NORTH_SET) && north != winner2.north)
    return false;

  if ((mode & WIN_SOUTH_SET) && south != winner2.south)
    return false;

  return true;
}


bool Winner::operator != (const Winner& winner2) const
{
  return ! (* this == winner2);
}


void Winner::multiplySide(
  Card& own,
  const Card& other,
  const unsigned char otherMode,
  const unsigned char bitmask)
{
  if (otherMode & bitmask)
  {
    if (mode & bitmask)
      own *= other;
    else
    {
      // As this side wasn't set, the other must be.
      own = other;
      mode = WIN_BOTH;
    }
  }
}


void Winner::operator *= (const Winner& winner2)
{
  // The opponents have the choice.

  if (winner2.mode == UCHAR_NOT_SET || rank < winner2.rank)
  {
    // OK as is.
    return;
  }
  else if (mode == UCHAR_NOT_SET || winner2.rank < rank)
  {
    * this = winner2;
    return;
  }

  Winner::multiplySide(north, winner2.north,
    winner2.mode, WIN_NORTH_SET);
  Winner::multiplySide(south, winner2.south,
    winner2.mode, WIN_SOUTH_SET);
}


Compare Winner::compareNonEmpties(const Winner& winner2) const
{
  assert(mode != UCHAR_NOT_SET);
  assert(winner2.mode != UCHAR_NOT_SET);

  // We have to test for rank first, as we might have this Winner as
  // South rank 4 number 0, and winner2 as 
  // North rank 2 number 0.

  if (rank > winner2.rank)
    return WIN_FIRST;
  else if (rank < winner2.rank)
    return WIN_SECOND;

  // We don't have to check whether something is set, as an unset
  // card has a maximum card number which declarer prefers.
  const Compare northPrefer = north.compare(winner2.north);
  const Compare southPrefer = south.compare(winner2.south);

  // Probably a matrix lookup isn't faster.
  //
  // N|S 1  2  =
  // 1   1  != 1
  // 2   != 2  2
  // =   1  2  =
  if (northPrefer == southPrefer)
    return northPrefer;
  else if (northPrefer == WIN_EQUAL)
    return southPrefer;
  else if (southPrefer == WIN_EQUAL)
    return northPrefer;
  else
    return WIN_DIFFERENT;
}


Compare Winner::compare(const Winner& winner2) const
{
  if (Winner::empty())
    return (winner2.empty() ? WIN_EQUAL : WIN_FIRST);
  else if (winner2.empty())
    return WIN_SECOND;
  else
    return Winner::compareNonEmpties(winner2);
}


void Winner::flip()
{
  if (Winner::empty())
    return;

  // Flips North and South.
  swap(north, south);

  if (mode == WIN_NORTH_ONLY)
    mode = WIN_SOUTH_ONLY;
  else if (mode == WIN_SOUTH_ONLY)
    mode = WIN_NORTH_ONLY;
}


void Winner::update(const Play& play)
{
  if (mode == WIN_NORTH_ONLY)
  {
    // This may also change the winning side.
    north = * play.northTranslate(north.getNumber());
    rank = north.getRank();
  }
  else if (mode == WIN_SOUTH_ONLY)
  {
    // This may also change the winning side.
    south = * play.southTranslate(south.getNumber());
    rank = south.getRank();
  }
  else if (mode == WIN_BOTH)
  {
    north = * play.northTranslate(north.getNumber());
    south = * play.southTranslate(south.getNumber());

    // As a result of the mapping to parent ranks, North and South
    // may actually be different ranks now.  In a Winner declarer
    // needs both, so only only keep the higher one.
    if (north.rankExceeds(south))
    {
      // Only South survives.
      north.reset();
      mode = WIN_SOUTH_ONLY;
      rank = south.getRank();
    }
    else if (south.rankExceeds(north))
    {
      south.reset();
      mode = WIN_NORTH_ONLY;
      rank = north.getRank();
    }
    else
     rank = north.getRank(); // Pick one
  }
  else if (mode == UCHAR_NOT_SET)
  {
    // Stick with the empty winner.
  }
  else
    // Should not happen.
    assert(false);
}


void Winner::expand(const char rankAdder)
{
  if (mode & WIN_NORTH_SET)
  {
    north.expand(rankAdder);
    rank = north.getRank();
  }

  if (mode & WIN_SOUTH_SET)
  {
    south.expand(rankAdder);
    rank = south.getRank();
  }
}


bool Winner::empty() const
{
  return (mode == UCHAR_NOT_SET);
}


unsigned char Winner::getRank() const
{
  // This includes UCHAR_NOT_SET if Winner::empty().
  return rank;
}


unsigned char Winner::getAbsNumber() const
{
  if (mode == UCHAR_NOT_SET)
    return 0;
  else if (mode == WIN_NORTH_SET)
    return north.getAbsNumber();
  else if (mode == WIN_SOUTH_SET)
    return south.getAbsNumber();
  else
    return min(north.getAbsNumber(), south.getAbsNumber());
}


string Winner::str() const
{
  if (mode == UCHAR_NOT_SET)
    return "-";
  else if (mode == WIN_NORTH_ONLY)
    return north.str("N");
  else if (mode == WIN_SOUTH_ONLY)
    return south.str("S");
  else
    return north.str("N") + south.str("S", false);
}


string Winner::strDebug() const
{
  stringstream ss;
  if (mode & WIN_NORTH_SET)
    ss << north.strDebug("N");
  if (mode & WIN_SOUTH_SET)
    ss << south.strDebug("S");
  return ss.str();
}


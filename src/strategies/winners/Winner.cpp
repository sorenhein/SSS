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

#define WIN_NORTH_SET 0x1
#define WIN_SOUTH_SET 0x2


Winner::Winner()
{
  Winner::reset();
}


Winner::~Winner()
{
}


void Winner::reset()
{
  north.reset();
  south.reset();
  mode = WIN_NOT_SET;
  rank = UCHAR_NOT_SET;
}


void Winner::set(
  const WinningSide sideIn,
  const unsigned char rankIn,
  const unsigned char depthIn,
  const unsigned char numberIn,
  const unsigned char nameIn)
{
  // This doesn't reset the winner, so the method can be used to build
  // up a winner with more than one component if called twice.
  // In that case, NS decide among the options.

  assert(mode == WIN_NOT_SET);
  rank = rankIn;

  if (sideIn == WIN_NORTH)
  {
    north.set(rankIn, depthIn, numberIn, nameIn);
    mode = WIN_NORTH_ONLY;
  }
  else if (sideIn == WIN_SOUTH)
  {
    south.set(rankIn, depthIn, numberIn, nameIn);
    mode = WIN_SOUTH_ONLY;
  }
  else if (sideIn == WIN_NONE)
    mode = WIN_NOT_SET;
  else
    assert(false);
}


void Winner::set(
  const WinningSide sideIn,
  const Card& card)
{
  // This doesn't reset the winner, so the method can be used to build
  // up a winner with more than one component if called twice.
  // In that case, NS decide among the options.

  assert(mode == WIN_NOT_SET);
  if (sideIn != WIN_NONE)
    rank = card.getRank();

  if (sideIn == WIN_NORTH)
  {
    north = card;
    mode = WIN_NORTH_ONLY;
  }
  else if (sideIn == WIN_SOUTH)
  {
    south = card;
    mode = WIN_SOUTH_ONLY;
  }
  else if (sideIn == WIN_NONE)
    mode = WIN_NOT_SET;
  else
    assert(false);
}


bool Winner::empty() const
{
  return (mode == WIN_NOT_SET);
}


void Winner::setEmpty()
{
  // Do nothing.
  assert(mode == WIN_NOT_SET);
}


bool Winner::operator == (const Winner& winner2) const
{
  if (mode != winner2.mode)
    return false;

  if (mode == WIN_NOT_SET)
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
  const WinnerMode otherMode,
  const unsigned bitmask)
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

  if (winner2.mode == WIN_NOT_SET)
  {
    // OK as is.
    return;
  }
  else if (mode == WIN_NOT_SET)
  {
    * this = winner2;
    return;
  }

  if (winner2.rank < rank)
    rank = winner2.rank;

  Winner::multiplySide(north, winner2.north, winner2.mode, WIN_NORTH_SET);
  Winner::multiplySide(south, winner2.south, winner2.mode, WIN_SOUTH_SET);

  if (mode == WIN_BOTH)
  {
    if (north.rankExceeds(south))
      mode = WIN_SOUTH_ONLY;
    else if (south.rankExceeds(north))
      mode = WIN_NORTH_ONLY;
  }
}


void Winner::operator += (const Winner& winner2)
{
  // Declarer has the choice.

  if (winner2.mode == WIN_NOT_SET)
  {
    // Declarer prefers no restriction.
    * this = winner2;
    return;
  }
  else if (mode == WIN_NOT_SET)
    return;

  if (winner2.rank > rank)
    rank = winner2.rank;

  if (winner2.mode & WIN_NORTH_SET)
  {
    // winner2.north is set.  Leave our north empty if it is empty.
    if (mode & WIN_NORTH_SET)
      north += winner2.north;
  }

  if (winner2.mode & WIN_SOUTH_SET)
  {
    // winner2.south is set.
    if (mode & WIN_SOUTH_SET)
      south += winner2.south;
  }

  if (mode == WIN_BOTH)
  {
    if (north.rankExceeds(south))
      mode = WIN_NORTH_ONLY;
    else if (south.rankExceeds(north))
      mode = WIN_SOUTH_ONLY;
  }
}


unsigned char Winner::getRank() const
{
  return rank;
}


Compare Winner::declarerPrefersSide(
  const Card& own,
  const Card& other,
  const WinnerMode otherMode,
  const unsigned bitmask) const
{
  if (mode & bitmask)
  {
    if (otherMode & bitmask)
      return own.compare(other);
    else
      // Declarer prefers no restriction.
      return WIN_SECOND;
  }
  else
  {
    if (otherMode & bitmask)
      // Declarer prefers no restriction.
      return WIN_FIRST;
    else
      // Both are missing, so it doesn't matter.
      return WIN_EQUAL;
  }
}



Compare Winner::declarerPrefers(const Winner& winner2) const
{
  assert(mode != WIN_NOT_SET);
  assert(winner2.mode != WIN_NOT_SET);

  if (rank > winner2.rank)
    return WIN_FIRST;
  else if (rank < winner2.rank)
    return WIN_SECOND;

  // So now the two Winner's have the same rank.

  const Compare northPrefer = Winner::declarerPrefersSide(
    north, winner2.north, winner2.mode, WIN_NORTH_SET);

  const Compare southPrefer = Winner::declarerPrefersSide(
    south, winner2.south, winner2.mode, WIN_SOUTH_SET);

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


void Winner::flip()
{
  if (mode == WIN_NOT_SET)
    return;

  // Flips North and South.
  // TODO Try std::swap?
  Card tmp = north;
  north = south;
  south = tmp;

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
  else if (mode == WIN_NOT_SET)
  {
    // Stick with the empty winner.
  }
  else
    // Should not happen.
    assert(false);
}


bool Winner::rankExceeds(const Winner& winner2) const
{
  if (mode == WIN_NOT_SET && winner2.mode != WIN_NOT_SET)
  {
    // Being unset is like having an "infinite" winning rank.
    assert(rank > winner2.rank);
    return true;
  }
  else if (winner2.mode == WIN_NOT_SET && mode != WIN_NOT_SET)
  {
    assert(rank < winner2.rank);
    return false;
  }

  return (rank > winner2.rank);
}


string Winner::str() const
{
  if (mode == WIN_NORTH_ONLY)
    return north.str("N");
  else if (mode == WIN_SOUTH_ONLY)
    return south.str("S");
  else if (mode == WIN_NOT_SET)
    return "-";
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


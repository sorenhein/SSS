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
  if (mode != WIN_SOUTH_ONLY && north != winner2.north)
    return false;
  if (mode != WIN_NORTH_ONLY && south != winner2.south)
    return false;

  return true;
}


bool Winner::operator != (const Winner& winner2) const
{
  return ! (* this == winner2);
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

  if (winner2.mode == WIN_NORTH_ONLY || winner2.mode == WIN_BOTH)
  {
    // winner2.north is set.
    if (mode == WIN_NORTH_ONLY || mode == WIN_BOTH)
      north *= winner2.north;
    else
    {
      // As north wasn't set, south must be.
      north = winner2.north;
      mode = WIN_BOTH;
    }
  }

  if (winner2.mode == WIN_SOUTH_ONLY || winner2.mode == WIN_BOTH)
  {
    // winner2.south is set.
    if (mode == WIN_SOUTH_ONLY || winner2.mode == WIN_BOTH)
      south *= winner2.south;
    else
    {
      // As south wasn't set, north must be.
      south = winner2.south;
      mode = WIN_BOTH;
    }
  }

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

  if (winner2.mode == WIN_NORTH_ONLY || winner2.mode == WIN_BOTH)
  {
    // winner2.north is set.  Leave our north empty if it is empty.
    if (mode == WIN_NORTH_ONLY || mode == WIN_BOTH)
      north += winner2.north;
  }

  if (winner2.mode == WIN_SOUTH_ONLY || winner2.mode == WIN_BOTH)
  {
    // winner2.south is set.
    if (mode == WIN_SOUTH_ONLY || winner2.mode == WIN_BOTH)
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


unsigned char Winner::rank() const
{
  if (mode == WIN_NOT_SET)
    return numeric_limits<unsigned char>::max();
  else if (mode == WIN_NORTH_ONLY || mode == WIN_BOTH)
    return north.getRank();
  else
    return south.getRank();
}


Compare Winner::declarerPrefers(const Winner& winner2) const
{
  assert(mode != WIN_NOT_SET);
  assert(winner2.mode != WIN_NOT_SET);

  // TODO Maybe Winner should know the rank.
  Card const * active1 =
      (mode == WIN_NORTH_ONLY || mode == WIN_BOTH ?
        &north : &south);
  Card const * active2 =
      (winner2.mode == WIN_NORTH_ONLY || winner2.mode == WIN_BOTH ? 
        &winner2.north : &winner2.south);

  if (active1->rankExceeds(* active2))
    return WIN_FIRST;
  else if (active2->rankExceeds(* active1))
    return WIN_SECOND;

  // So now the two Winner's have the same rank.
  // TODO Might be nice to have WinnerMode as a 2-bit vector
  // or to have separate North and South bits.

  Compare northPrefer, southPrefer;
  if (mode == WIN_NORTH_ONLY || mode == WIN_BOTH)
  {
    if (winner2.mode == WIN_NORTH_ONLY || winner2.mode == WIN_BOTH)
      northPrefer = north.compare(winner2.north);
    else
      // North prefers no restriction.
      northPrefer = WIN_SECOND;
  }
  else
  {
    if (winner2.mode == WIN_NORTH_ONLY || winner2.mode == WIN_BOTH)
      // North prefers no restriction.
      northPrefer = WIN_FIRST;
    else
      // Both are missing, so it doesn't matter.
      northPrefer = WIN_EQUAL;
  }
  
  if (mode == WIN_SOUTH_ONLY || mode == WIN_BOTH)
  {
    if (winner2.mode == WIN_SOUTH_ONLY || winner2.mode == WIN_BOTH)
      southPrefer = south.compare(winner2.south);
    else
      // South prefers no restriction.
      southPrefer = WIN_SECOND;
  }
  else
  {
    if (winner2.mode == WIN_SOUTH_ONLY || winner2.mode == WIN_BOTH)
      // South prefers no restriction.
      southPrefer = WIN_FIRST;
    else
      // Both are missing, so it doesn't matter.
      southPrefer = WIN_EQUAL;
  }

  // TODO Can set up a matrix lookup as well.
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
  }
  else if (mode == WIN_SOUTH_ONLY)
  {
    // This may also change the winning side.
    south = * play.southTranslate(south.getNumber());
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
    }
    else if (south.rankExceeds(north))
    {
      south.reset();
      mode = WIN_NORTH_ONLY;
    }
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
  // TODO Maybe Winner should know its rank.

  if (mode == WIN_NOT_SET && winner2.mode != WIN_NOT_SET)
    // Being unset is like having an "infinite" winning rank.
    return true;
  else if (winner2.mode == WIN_NOT_SET && mode != WIN_NOT_SET)
    return false;

  const unsigned rank1 =
      (mode == WIN_NORTH_ONLY || mode == WIN_BOTH ?
        north.getRank() : south.getRank());
  const unsigned rank2 =
      (winner2.mode == WIN_NORTH_ONLY || winner2.mode == WIN_BOTH ?
        winner2.north.getRank() : winner2.south.getRank());

  return (rank1 > rank2);
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
  if (mode == WIN_NORTH_ONLY || mode == WIN_BOTH)
    ss << north.strDebug("N");
  if (mode == WIN_SOUTH_ONLY || mode == WIN_BOTH)
    ss << south.strDebug("S");
  return ss.str();
}


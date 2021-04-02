#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Subwinner.h"


Subwinner::Subwinner()
{
  Subwinner::reset();
}


Subwinner::~Subwinner()
{
}


void Subwinner::reset()
{
  north.reset();
  south.reset();
  mode = SUBWIN_NOT_SET;
}


void Subwinner::set(
  const WinningSide sideIn,
  const unsigned rankIn,
  const unsigned depthIn,
  const unsigned numberIn,
  const char nameIn)
{
  // This doesn't reset the winner, so the method can be used to build
  // up a winner with more than one component if called twice.
  // In that case, NS decide among the options.

  assert(mode == SUBWIN_NOT_SET);
  if (sideIn == WIN_NORTH)
  {
    north.set(rankIn, depthIn, numberIn, nameIn);
    mode = SUBWIN_NORTH_ONLY;
  }
  else if (sideIn == WIN_SOUTH)
  {
    south.set(rankIn, depthIn, numberIn, nameIn);
    mode = SUBWIN_SOUTH_ONLY;
  }
  else if (sideIn == WIN_NONE)
    mode = SUBWIN_NOT_SET;
  else
    assert(false);
}


void Subwinner::set(
  const WinningSide sideIn,
  const Card& card)
{
  // This doesn't reset the winner, so the method can be used to build
  // up a winner with more than one component if called twice.
  // In that case, NS decide among the options.

  assert(mode == SUBWIN_NOT_SET);
  if (sideIn == WIN_NORTH)
  {
    north = card;
    mode = SUBWIN_NORTH_ONLY;
  }
  else if (sideIn == WIN_SOUTH)
  {
    south = card;
    mode = SUBWIN_SOUTH_ONLY;
  }
  else if (sideIn == WIN_NONE)
    mode = SUBWIN_NOT_SET;
  else
    assert(false);
}


bool Subwinner::operator == (const Subwinner& sw2) const
{
  if (mode != sw2.mode)
    return false;
  if (mode == SUBWIN_NOT_SET)
    return true;
  if (mode != SUBWIN_SOUTH_ONLY && north != sw2.north)
    return false;
  if (mode != SUBWIN_NORTH_ONLY && south != sw2.south)
    return false;

  return true;
}


bool Subwinner::operator != (const Subwinner& sw2) const
{
  return ! (* this == sw2);
}


void Subwinner::operator *= (const Subwinner& sw2)
{
  // The opponents have the choice.

  if (sw2.mode == SUBWIN_NOT_SET)
  {
    // OK as is.
    return;
  }
  else if (mode == SUBWIN_NOT_SET)
  {
    * this = sw2;
    return;
  }

  if (sw2.mode == SUBWIN_NORTH_ONLY || sw2.mode == SUBWIN_BOTH)
  {
    // sw2.north is set.
    if (mode == SUBWIN_NORTH_ONLY || mode == SUBWIN_BOTH)
      north *= sw2.north;
    else
    {
      // As north wasn't set, south must be.
      north = sw2.north;
      mode = SUBWIN_BOTH;
    }
  }

  if (sw2.mode == SUBWIN_SOUTH_ONLY || sw2.mode == SUBWIN_BOTH)
  {
    // w2.south is set.
    if (mode == SUBWIN_SOUTH_ONLY || sw2.mode == SUBWIN_BOTH)
      south *= sw2.south;
    else
    {
      // As south wasn't set, north must be.
      south = sw2.south;
      mode = SUBWIN_BOTH;
    }
  }

  if (mode == SUBWIN_BOTH)
  {
    if (north.rankExceeds(south))
      mode = SUBWIN_SOUTH_ONLY;
    else if (south.rankExceeds(north))
      mode = SUBWIN_NORTH_ONLY;
  }
}


WinnerCompare Subwinner::declarerPrefers(const Subwinner& sw2) const
{
  assert(mode != SUBWIN_NOT_SET);
  assert(sw2.mode != SUBWIN_NOT_SET);

  // TODO Maybe Subwinner should know the rank.
  Card const * active1 =
      (mode == SUBWIN_NORTH_ONLY || mode == SUBWIN_BOTH ?
        &north : &south);
  Card const * active2 =
      (sw2.mode == SUBWIN_NORTH_ONLY || sw2.mode == SUBWIN_BOTH ? 
        &sw2.north : &sw2.south);

  if (active1->rankExceeds(* active2))
    return WIN_FIRST;
  else if (active2->rankExceeds(* active1))
    return WIN_SECOND;

  // So now the two Subwinners have the same rank.
  // TODO Might be nice to have SubwinnerMode as a 2-bit vector
  // or to have separate North and South bits.

  WinnerCompare northPrefer, southPrefer;
  if (mode == SUBWIN_NORTH_ONLY || mode == SUBWIN_BOTH)
  {
    if (sw2.mode == SUBWIN_NORTH_ONLY || mode == SUBWIN_BOTH)
      northPrefer = north.compare(sw2.north);
    else
      // North prefers no restriction.
      northPrefer = WIN_SECOND;
  }
  else
  {
    if (sw2.mode == SUBWIN_NORTH_ONLY || mode == SUBWIN_BOTH)
      // North prefers no restriction.
      northPrefer = WIN_FIRST;
    else
      // Both are missing, so it doesn't matter.
      northPrefer = WIN_EQUAL;
  }
  
  if (mode == SUBWIN_SOUTH_ONLY || mode == SUBWIN_BOTH)
  {
    if (sw2.mode == SUBWIN_SOUTH_ONLY || mode == SUBWIN_BOTH)
      southPrefer = south.compare(sw2.north);
    else
      // South prefers no restriction.
      southPrefer = WIN_SECOND;
  }
  else
  {
    if (sw2.mode == SUBWIN_SOUTH_ONLY || mode == SUBWIN_BOTH)
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
  else if (northPrefer == SUBWIN_BOTH)
    return southPrefer;
  else if (southPrefer == SUBWIN_BOTH)
    return northPrefer;
  else
    return WIN_DIFFERENT;
}


void Subwinner::flip()
{
  if (mode == SUBWIN_NOT_SET)
    return;

  // Flips North and South.
  // TODO Try std::swap?
  Card tmp = north;
  north = south;
  south = tmp;

  if (mode == SUBWIN_NORTH_ONLY)
    mode = SUBWIN_SOUTH_ONLY;
  else if (mode == SUBWIN_SOUTH_ONLY)
    mode = SUBWIN_NORTH_ONLY;
}


void Subwinner::update(
  vector<Card> const * northOrderPtr,
  vector<Card> const * southOrderPtr)
{
  if (mode == SUBWIN_NORTH_ONLY)
  {
    // This may also change the winning side.
    assert(northOrderPtr != nullptr);
    assert(north.getNumber() < northOrderPtr->size());
    north = (* northOrderPtr)[north.getNumber()];
  }
  else if (mode == SUBWIN_SOUTH_ONLY)
  {
    // This may also change the winning side.
    assert(southOrderPtr != nullptr);
    assert(south.getNumber() < southOrderPtr->size());
    south = (* southOrderPtr)[south.getNumber()];
  }
  else if (mode == SUBWIN_BOTH)
  {
    assert(northOrderPtr != nullptr);
    assert(north.getNumber() < northOrderPtr->size());
    north = (* northOrderPtr)[north.getNumber()];

    assert(southOrderPtr != nullptr);
    assert(south.getNumber() < southOrderPtr->size());
    south = (* southOrderPtr)[south.getNumber()];

    // As a result of the mapping to parent ranks, North and South
    // may actually be different ranks now.  As North-South choose,
    // they only keep the higher one.
    if (north.rankExceeds(south))
    {
      // Only North survives.
      south.reset();
      mode = SUBWIN_NORTH_ONLY;
    }
    else if (south.rankExceeds(north))
    {
      north.reset();
      mode = SUBWIN_SOUTH_ONLY;
    }
  }
  else if (mode == SUBWIN_NOT_SET)
  {
    // Stick with the empty winner.
  }
  else
    // Should not happen.
    assert(false);
}


bool Subwinner::rankExceeds(const Subwinner& sw2) const
{
  // TODO Maybe Subwinner should know its rank.
  const unsigned rank1 =
      (mode == SUBWIN_NORTH_ONLY || mode == SUBWIN_BOTH ?
        north.getRank() : south.getRank());
  const unsigned rank2 =
      (sw2.mode == SUBWIN_NORTH_ONLY || sw2.mode == SUBWIN_BOTH ?
        sw2.north.getRank() : sw2.south.getRank());

  return (rank1 > rank2);
}


string Subwinner::str() const
{
  if (mode == SUBWIN_NORTH_ONLY)
    return north.str("N");
  else if (mode == SUBWIN_SOUTH_ONLY)
    return south.str("S");
  else if (mode == SUBWIN_NOT_SET)
    return "-";
  else
    return north.str("N") + south.str("S", false);
}


string Subwinner::strDebug() const
{
  stringstream ss;
  if (mode == SUBWIN_NORTH_ONLY || mode == SUBWIN_BOTH)
    ss << north.strDebug("N");
  if (mode == SUBWIN_SOUTH_ONLY || mode == SUBWIN_BOTH)
    ss << south.strDebug("S");
  return ss.str();
}


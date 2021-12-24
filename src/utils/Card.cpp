/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Card.h"
#include "../const.h"

using namespace std;


Card::Card()
{
  Card::reset();
}


void Card::reset()
{
  rank = 0;
  depth = 0;
  number = UCHAR_NOT_SET;
  // TODO Try it with void as 0.
  absNumber = 0;
  // absNumber = UCHAR_NOT_SET;
  name = '-';
}


void Card::set(
  const unsigned char rankIn,
  const unsigned char depthIn,
  const unsigned char numberIn,
  const unsigned char absNumberIn,
  const unsigned char nameIn)
{
  rank = rankIn;
  depth = depthIn;
  number = numberIn;
  absNumber = absNumberIn;
  name = nameIn;
}


void Card::updateName(const unsigned char nameIn)
{
  // Used for opponents' cards once we know to replace, say, '6' or '7'
  // with 'x'.
  name = nameIn;
}


bool Card::operator > (const Card& card2) const
{
/*
if ((number > card2.number && absNumber <= card2.absNumber) ||
    (number < card2.number && absNumber >= card2.absNumber))
{
  cout << "n " << +number << ", n2 << " << +card2.number << endl;
  cout << "a " << +absNumber << ", abs2 " << +card2.absNumber << endl;
  cout << Card::strDebug("warnfirst");
  cout << card2.strDebug("warnsecond");
}
*/

  return (absNumber > card2.absNumber);
}


bool Card::operator != (const Card& card2) const
{
  return (absNumber != card2.absNumber);
}


bool Card::identical(const Card& card2) const
{
  return (rank == card2.rank &&
      depth == card2.depth &&
      number == card2.number &&
      name == card2.name);
}


Compare Card::compare(const Card& card2) const
{
  if (number > card2.number)
    return WIN_FIRST;
  else if (number < card2.number)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


bool Card::isVoid() const
{
  return (rank == 0);
}


bool Card::rankSame(const Card& card2) const
{
  return (rank == card2.rank);
}


bool Card::rankExceeds(const Card& card2) const
{
  return (rank > card2.rank);
}


void Card::operator *= (const Card& card2)
{
  // Choose the lowest card.
  if (number > card2.number)
    * this = card2;
}


void Card::operator += (const Card &card2)
{
  // Choose the highest card.
  if (number < card2.number)
    * this = card2;
}


unsigned char Card::getRank() const
{
  return rank;
}


unsigned char Card::getDepth() const
{
  return depth;
}


unsigned char Card::getNumber() const
{
  return number;
}


unsigned char Card::getAbsNumber() const
{
  return absNumber;
}


unsigned char Card::getName() const
{
  return name;
}


void Card::expand(const char rankAdder)
{
  rank = static_cast<unsigned char>(static_cast<char>(rank) + rankAdder);
}


void Card::flipDepth(const unsigned char maxDepth)
{
  // Depths arise in Ranks in opposite order (see comment in
  // Declarer::fixDepths), so we have to flip them afterwards.
  depth = maxDepth - depth;
}


string Card::str(
  const string& side,
  const bool rankFlag) const
{
  stringstream ss;
  if (rankFlag)
    ss << hex << uppercase << +rank << dec;

  ss << side;
  if (depth == 1)
    ss << "'";
  else if (depth == 2)
    ss << "\"";
  else if (depth > 2)
    ss << "+";
  return ss.str();
}


string Card::strDebug(const string& side) const
{
  stringstream ss;
  ss << side << ": " <<
    "rank " << +rank <<
    " depth " << +depth <<
    " number " << +number << 
    " absNumber " << +absNumber << 
    " name " << name <<
    "\n";
  return ss.str();
}


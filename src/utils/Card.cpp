/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

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
  number = UCHAR_NOT_SET;
  absNumber = 0;
  name = '-';
}


void Card::set(
  const unsigned char rankIn,
  const unsigned char numberIn,
  const unsigned char absNumberIn,
  const unsigned char nameIn)
{
  rank = rankIn;
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
  return (absNumber > card2.absNumber);
}


bool Card::operator != (const Card& card2) const
{
  return (absNumber != card2.absNumber);
}


bool Card::identical(const Card& card2) const
{
  return (rank == card2.rank &&
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


string Card::str() const
{
  return string(1, static_cast<char>(name));
}


string Card::strDebug(const string& side) const
{
  stringstream ss;
  ss << side << ": " <<
    "rank " << +rank <<
    " number " << +number << 
    " absNumber " << +absNumber << 
    " name " << name <<
    "\n";
  return ss.str();
}


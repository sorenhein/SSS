#include <iostream>
#include <iomanip>
#include <sstream>

#include "Card.h"

using namespace std;


void Card::reset()
{
  rank = 0;
  depth = 0;
  number = 0;
  name = '-';
}


void Card::set(
  const unsigned rankIn,
  const unsigned depthIn,
  const unsigned numberIn,
  const char nameIn)
{
  rank = rankIn;
  depth = depthIn;
  number = numberIn;
  name = nameIn;
}


bool Card::operator > (const Card& sw2) const
{
  return (number > sw2.number);
}


bool Card::operator >= (const Card& sw2) const
{
  return (number >= sw2.number);
}


bool Card::operator == (const Card& sw2) const
{
  return (number == sw2.number);
}


bool Card::operator != (const Card& sw2) const
{
  return (number != sw2.number);
}


bool Card::operator <= (const Card& sw2) const
{
  return (number >= sw2.number);
}


bool Card::operator < (const Card& sw2) const
{
  return (number < sw2.number);
}


WinnerCompare Card::compare(const Card& sw2) const
{
  if (number > sw2.number)
    return WIN_FIRST;
  else if (number < sw2.number)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


bool Card::rankSame(const Card& sw2) const
{
  return (rank == sw2.rank);
}


bool Card::rankExceeds(const Card& sw2) const
{
  return (rank > sw2.rank);
}


void Card::operator *= (const Card& sw2)
{
  // Choose the lowest card.
  if (number > sw2.number)
    * this = sw2;
}


unsigned Card::getRank() const
{
  return rank;
}


unsigned Card::getNumber() const
{
  return number;
}


char Card::getName() const
{
  return name;
}


string Card::str(
  const string& side,
  const bool rankFlag) const
{
  stringstream ss;
  if (rankFlag)
    ss << hex << uppercase << rank << dec;

  ss << side;
  if (depth == 2)
    ss << "'";
  else if (depth == 3)
    ss << "\"";
  else if (depth > 3)
    ss << "+";
  return ss.str();
}


string Card::strDebug(const string& side) const
{
  stringstream ss;
  ss << side << ": " <<
    "rank " << rank <<
    " depth " << depth <<
    " number " << number << 
    " name " << name <<
    endl;
  return ss.str();
}


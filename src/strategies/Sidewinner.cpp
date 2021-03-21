#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Sidewinner.h"

using namespace std;


void Sidewinner::reset()
{
  rank = 0;
  depth = 0;
  number = 0;
}


void Sidewinner::set(
  const unsigned rankIn,
  const unsigned depthIn,
  const unsigned numberIn)
{
  rank = rankIn;
  depth = depthIn;
  number = numberIn;
}


bool Sidewinner::operator > (const Sidewinner& sw2) const
{
  return (number > sw2.number);
}


bool Sidewinner::operator >= (const Sidewinner& sw2) const
{
  return (number >= sw2.number);
}


bool Sidewinner::operator == (const Sidewinner& sw2) const
{
  return (number == sw2.number);
}


bool Sidewinner::operator != (const Sidewinner& sw2) const
{
  return (number != sw2.number);
}


bool Sidewinner::operator <= (const Sidewinner& sw2) const
{
  return (number >= sw2.number);
}


bool Sidewinner::operator < (const Sidewinner& sw2) const
{
  return (number < sw2.number);
}


WinnerCompare Sidewinner::compare(const Sidewinner& sw2) const
{
  if (number > sw2.number)
    return WIN_FIRST;
  else if (number < sw2.number)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


bool Sidewinner::rankSame(const Sidewinner& sw2) const
{
  return (rank == sw2.rank);
}


bool Sidewinner::rankExceeds(const Sidewinner& sw2) const
{
  return (rank > sw2.rank);
}


void Sidewinner::operator *= (const Sidewinner& sw2)
{
  // Choose the lowest card.
  if (number > sw2.number)
    * this = sw2;
}


unsigned Sidewinner::no() const
{
  return number;
}


string Sidewinner::str(
  const string& name,
  const bool rankFlag) const
{
  stringstream ss;
  if (rankFlag)
    ss << hex << uppercase << rank << dec;

  ss << name;
  if (depth == 2)
    ss << "'";
  else if (depth == 3)
    ss << "\"";
  else if (depth > 3)
    ss << "+";
  return ss.str();
}


string Sidewinner::strDebug(const string& name) const
{
  stringstream ss;
  ss << name << ": " <<
    "rank " << rank <<
    " depth " << depth <<
    " number " << number << endl;
  return ss.str();
}


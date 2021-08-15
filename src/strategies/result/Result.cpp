#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Result.h"

// TODO Clean up operators


void Result::set(
  const unsigned char dist,
  const unsigned char tricks,
  const Winners& winners)
{
  distInt = dist;
  tricksInt = tricks;
  winnersInt = winners;
}


void Result::setDist(const unsigned char dist)
{
  distInt = dist;
}


void Result::setTricks(const unsigned char tricks)
{
  tricksInt = tricks;
  winnersInt.setEmpty();
}


void Result::update(
  const unsigned char dist,
  const unsigned char trickNS)
{
  distInt = dist;
  tricksInt += trickNS;
}


void Result::update(const Play& play)
{
  winnersInt.update(play);
}


void Result::flip()
{
  winnersInt.flip();
}


void Result::multiplyWinnersOnto(Result& result) const
{
  result.winnersInt *= winnersInt;
  
}


void Result::operator *= (const Result& result2)
{
  // Keep the "lower" one.
  if (tricksInt > result2.tricksInt)
    * this = result2;
  else if (tricksInt == result2.tricksInt)
    winnersInt *= result2.winnersInt;
}


void Result::operator += (const Result& result2)
{
  // Keep the "upper" one.
  if (tricksInt < result2.tricksInt)
    * this = result2;
  else if (tricksInt == result2.tricksInt)
    winnersInt += result2.winnersInt;
}

  
bool Result::operator < (const Result& res2) const
{
  if (tricksInt < res2.tricksInt)
    return true;
  else if (tricksInt > res2.tricksInt)
    return false;
  else
    return (winnersInt.compareForDeclarer(res2.winnersInt) == WIN_SECOND);
}


bool Result::operator == (const Result& res2) const
{
  if (tricksInt != res2.tricksInt)
    return false;
  else
    return (winnersInt.compareForDeclarer(res2.winnersInt) == WIN_EQUAL);
}


bool Result::operator != (const Result& res2) const
{
  return ! (* this == res2);
}


Compare Result::compareByTricks(const Result& res2) const
{
  if (tricksInt > res2.tricksInt)
    return WIN_FIRST;
  else if (tricksInt < res2.tricksInt)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


Compare Result::compareForDeclarer(const Result& res2) const
{
  if (tricksInt > res2.tricksInt)
    return WIN_FIRST;
  else if (tricksInt < res2.tricksInt)
    return WIN_SECOND;
  else
    return winnersInt.compareForDeclarer(res2.winnersInt);
}


unsigned char Result::dist() const
{
  return distInt;
}


unsigned char Result::tricks() const
{
  return tricksInt;
}


string Result::strHeaderEntry(
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


string Result::strEntry(const bool rankFlag) const
{
  stringstream ss;
  ss << setw(4) << +tricksInt;

  if (rankFlag)
    ss << setw(8) << winnersInt.strEntry();

  return ss.str();
}


string Result::strWinners() const
{
  return winnersInt.strEntry();
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
    setw(4) << +distInt << 
    setw(6) << Result::strEntry(rankFlag) << "\n";

  return ss.str();
}


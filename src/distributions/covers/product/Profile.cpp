/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Profile.h"

#include "../term/TopData.h"

#include "../../../ranks/RanksNames.h"


void Profile::set(const vector<unsigned char>& topsIn)
{
  tops = topsIn;

  lengthInt = 0;
  for (auto& t: tops)
    lengthInt += t;
}


void Profile::setSum(
  const vector<unsigned char>& topsWest,
  const vector<unsigned char>& topsEast)
{
  const size_t s = topsWest.size();
  assert(topsEast.size() == s);

  tops.resize(s);

  lengthInt = 0;
  for (unsigned i = 0; i < s; i++)
  {
    tops[i] = topsWest[i] + topsEast[i];
    lengthInt += tops[i];
  }
}


void Profile::limit()
{
  for (unsigned i = 0; i+1 < tops.size(); i++)
    tops[i] = 0;
}


unsigned char Profile::length() const
{
  return lengthInt;
}


void Profile::getTopData(
  const unsigned char topNo,
  const RanksNames& ranksNames,
  TopData& topData) const
{
  assert(topNo < tops.size());

  topData.value = tops[topNo];
  topData.rankNamesPtr = &ranksNames.getOpponents(topNo);
}


unsigned char Profile::operator [] (const unsigned char topNo) const
{
  assert(topNo < tops.size());
  return tops[topNo];
}


bool Profile::onlyEquals(
  const Profile& upperProfile,
  const Profile& sumProfile) const
{
  assert(tops.size() == upperProfile.tops.size());

  if (lengthInt != upperProfile.lengthInt)
    return false;

  // This ignores the 0'th top!  So we can compare lower and upper.
  // We only consider tops that are used.
  for (unsigned i = 1; i < tops.size(); i++)
  {
    if (tops[i] == 0 && upperProfile.tops[i] == sumProfile.tops[i])
      continue;

    if (tops[i] != upperProfile.tops[i])
{
  /*
  cout << Profile::strHeader();
  cout << Profile::strLine();
  cout << upperProfile.strLine();
  cout << sumProfile.strLine();
  cout << endl;
  *&
      return false;
}
  }

  return true;
}


bool Profile::symmetricEntry(
  const unsigned char lower,
  const unsigned char upper,
  const unsigned char sum) const
{
  if (upper == 0xf)
  {
    if (lower != 0)
      return false;
  }
  else if (lower + upper != sum)
    return false;

  return true;
}


bool Profile::symmetricAgainst(
  const Profile& upperProfile,
  const Profile& sumProfile) const
{
  assert(tops.size() == upperProfile.tops.size());

  if (! Profile::symmetricEntry(lengthInt, upperProfile.lengthInt,
      sumProfile.lengthInt))
    return false;

  // This ignores the 0'th top!
  for (unsigned i = 1; i < tops.size(); i++)
  {
    if (! Profile::symmetricEntry(tops[i], upperProfile.tops[i],
        sumProfile.tops[i]))
    return false;
  }

  return true;
}


size_t Profile::size() const
{
  return tops.size();
}


unsigned long long Profile::getLowerCode() const
{
  // There should be at most 7 different tops in a 13-card distribution.
  assert(tops.size() < 8);

  // As the length is also <= 13, we can code everything in 32 bits.
  // Later on we will combine two profiles (upper and lower), so we
  // use 64 bits here as well -- it is not stored in Profile anyway.

  // The layout is: length, then the tops from most significant one down.

  unsigned long long code = 0;
  for (size_t i = tops.size(); i-- > 0; )
    code = (code << 4) | tops[i];

  code |= (lengthInt << (4*tops.size()));

  return code;
}


unsigned long long Profile::getUpperCode(
  const Profile& sumProfile,
  const Profile& lowerProfile) const
{
  // This is similar, but is used for an upper profile.  Actual
  // greater-equal counts are rounded up to 15 (0xf),
  // so that the unused terms later on compare correctly.
  assert(tops.size() < 8);

  unsigned long long code = 0;
  for (size_t i = tops.size(); i-- > 0; )
  {
    const unsigned value = (
      tops[i] == sumProfile.tops[i] &&
      tops[i] > lowerProfile.tops[i] ? 0xf : tops[i]);
    code = (code << 4) | value;
  }

  const unsigned vlen = (
    lengthInt == sumProfile.lengthInt &&
    lengthInt > lowerProfile.lengthInt ? 0xf : lengthInt);

  code |= (vlen << (4*tops.size()));

  return code;
}


string Profile::strHeader() const
{
  stringstream ss;

  ss << setw(7) << "Length";
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(5) << ("Top" + to_string(i));

  return ss.str() + "\n";
}


string Profile::strLine() const
{
  stringstream ss;

  ss << setw(6) << +lengthInt << ":";
  for (auto& t: tops)
    ss << setw(5) << +t;

  return ss.str() + "\n";
}


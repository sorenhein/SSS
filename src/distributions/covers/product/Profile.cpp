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
  const unsigned s = topsWest.size();
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


unsigned char Profile::operator [] (const unsigned char topNo) const
{
  assert(topNo < tops.size());
  return tops[topNo];
}


bool Profile::operator == (const Profile& profile2) const
{
  assert(tops.size() == profile2.tops.size());

  if (lengthInt != profile2.lengthInt)
    return false;

  // This ignores the 0'th top!  So we can compare lower and upper.
  for (unsigned i = 1; i < tops.size(); i++)
  {
    if (tops[i] != profile2.tops[i])
      return false;
  }

  return true;
}


unsigned Profile::size() const
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
  for (unsigned i = tops.size(); i-- > 0; )
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
  for (unsigned i = tops.size(); i-- > 0; )
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

  ss << setw(6) << "Length";
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(3) << ("Top" + to_string(i));

  return ss.str() + "\n";
}


string Profile::strLine() const
{
  stringstream ss;

  ss << setw(6) << +lengthInt << ":";
  for (auto& t: tops)
    ss << setw(3) << +t;

  return ss.str() + "\n";
}


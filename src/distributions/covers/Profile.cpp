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


void Profile::set(
  const vector<unsigned char>& topsIn,
  const unsigned char numTops)
{
  const unsigned num = (numTops == 0 ? topsIn.size() : numTops);
  tops.resize(num);

  length = 0;
  for (unsigned i = 0; i < num; i++)
  {
    tops[i] = topsIn[i];
    length += topsIn[i];
  }
}


void Profile::setSingle(
  const unsigned char numTops,
  const unsigned char lengthIn,
  const unsigned char topIn)
{
  length = lengthIn;

  // The bottom top is not used, so this is what a single top means.
  assert(numTops > 0);
  tops.resize(numTops);
  tops.back() = topIn;
}


void Profile::setSum(
  const vector<unsigned char>& topsWest,
  const vector<unsigned char>& topsEast)
{
  const unsigned s = topsWest.size();
  assert(topsEast.size() == s);

  tops.resize(s);

  length = 0;
  for (unsigned i = 0; i < s; i++)
  {
    tops[i] = topsWest[i] + topsEast[i];
    length += tops[i];
  }
}


void Profile::mirrorAround(const Profile& sumProfile)
{
  // Turn this profile into sumProfile minus this one.
  length = sumProfile.length - length;

  const unsigned s = tops.size();
  assert(sumProfile.tops.size() == s);
  for (unsigned i = 0; i < s; i++)
    tops[i] = sumProfile.tops[i] - tops[i];
}


unsigned char Profile::count(const unsigned char topNo) const
{
  assert(topNo < tops.size());
  return tops[topNo];
}


unsigned Profile::size() const
{
  return tops.size();
}


unsigned char Profile::getLength() const
{
  return length;
}


const vector<unsigned char>& Profile::getTops() const
{
  return tops;
}


unsigned long long Profile::getCode() const
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

  code |= (length << (4*tops.size()));

  return code;
}


unsigned long long Profile::getCode(
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
    length == sumProfile.length &&
    length > lowerProfile.length ? 0xf : length);

  code |= (vlen << (4*tops.size()));

  return code;
}


string Profile::strHeader() const
{
  stringstream ss;

  ss << setw(6) << "Length";
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(6) << ("Top" + to_string(i));

  return ss.str() + "\n";
}


string Profile::strLine() const
{
  stringstream ss;

  ss << setw(6) << +length << ":";
  for (auto& t: tops)
    ss << setw(6) << +t;

  return ss.str() + "\n";
}


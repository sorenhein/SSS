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
  const unsigned char lengthIn,
  const unsigned char topIn)
{
  length = lengthIn;

  // The bottom top is not used, so this is what a single top means.
  tops.resize(2);
  tops[1] = topIn;
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


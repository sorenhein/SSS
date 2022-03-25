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


void Profile::resize(const unsigned numTops)
{
  tops.resize(numTops);
}


void Profile::set(
  const vector<unsigned char>& topsIn,
  const unsigned char lastUsed)
{
  tops.resize(lastUsed+1);

  length = 0;
  for (unsigned i = 0; i <= lastUsed; i++)
  {
    tops[i] = topsIn[i];
    length += topsIn[i];
  }
}


void Profile::mirror(const Profile& profile2)
{
  // Turn this profile into pp2 (a sum profile) minus this one.

  length = profile2.length - length;

  const unsigned s = tops.size();
  assert(profile2.tops.size() == s);
  for (unsigned i = 0; i < s; i++)
    tops[i] = profile2.tops[i] - tops[i];
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


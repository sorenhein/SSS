/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Composition.h"


Composition::Composition()
{
  Composition::reset();
}


void Composition::reset()
{
  length = 0;
  tops.clear();
}


void Composition::set(
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


unsigned char Composition::count(const unsigned char topNo) const
{
  assert(topNo < tops.size());
  return tops[topNo];
}


unsigned Composition::size() const
{
  return tops.size();
}


string Composition::strHeader(const unsigned char width) const
{
  stringstream ss;

  ss << setw(6) << "Length";
  for (unsigned i = 0; i < width; i++)
    ss << setw(6) << ("Top" + to_string(i));

  return ss.str() + "\n";
}


string Composition::strLine() const
{
  stringstream ss;

  ss << setw(6) << +length;
  for (auto t: tops)
    ss << setw(6) << +t;

  return ss.str() + "\n";
}

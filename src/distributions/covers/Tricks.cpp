/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <limits>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Tricks.h"

#include "product/Product.h"
#include "product/Profile.h"

#include "../../strategies/result/Result.h"


void Tricks::clear()
{
  tricks.clear();
}


void Tricks::resize(const unsigned len)
{
  tricks.resize(len);
}


void Tricks::set(
  const list<Result>& results,
  unsigned char& tricksMin)
{
  // Unlike the other set, this method subtracts out the minimum.
  tricks.resize(results.size());
  tricksMin = numeric_limits<unsigned char>::max();
  unsigned i = 0;

  for (auto& res:results)
  {
    tricks[i] = res.getTricks();
    if (tricks[i] < tricksMin)
      tricksMin = tricks[i];

    i++;
  }

  for (i = 0; i < tricks.size(); i++)
    tricks[i] -= tricksMin;
}


void Tricks::weigh(
  const vector<unsigned char>& cases,
  unsigned char& weight) const
{
  assert(cases.size() == tricks.size());

  weight = 0;

  for (unsigned i = 0; i < tricks.size(); i++)
  {
    // TODO Can delete "if"
    if (tricks[i])
      weight += cases[i] * tricks[i];
  }
}


void Tricks::prepare(
  const Product& product,
  const bool symmFlag,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases,
  unsigned& weight,
  unsigned char& numDist)
{
  const unsigned len = distProfiles.size();
  assert(len == cases.size());
  tricks.resize(len);

  weight = 0;
  numDist = 0;

  if (symmFlag)
  {
    for (unsigned dno = 0; dno < len; dno++)
    {
      if (product.includes(distProfiles[dno]) ||
          product.includes(distProfiles[len-1-dno]))
      {
        tricks[dno] = 1;
        weight += static_cast<unsigned>(cases[dno]);
        numDist++;
      }
    }
  }
  else
  {
    for (unsigned dno = 0; dno < len; dno++)
    {
      if (product.includes(distProfiles[dno]))
      {
        tricks[dno] = 1;
        weight += static_cast<unsigned>(cases[dno]);
        numDist++;
      }
    }
  }
}


bool Tricks::symmetrize(
  const vector<unsigned char>& cases,
  unsigned& weight,
  unsigned char& numDist)
{
  // Will invalidate Tricks if not symmetrizable!
  // We only symmetrize if there is no overlap with the mirror.
  // We cannot be sure to double weight and numDist, as the middle
  // element in an odd-length tricks will not be repeated.

  const unsigned len = tricks.size();

  for (unsigned dno = 0; dno < len/2; dno++)
  {
    const unsigned mno = len - 1 - dno;
    if (tricks[dno] == 0)
    {
      if (tricks[mno] > 0)
      {
        tricks[dno] = tricks[mno];
        weight += cases[mno] * tricks[mno];
        numDist++;
      }
    }
    else if (tricks[mno] == 0)
    {
      tricks[mno] = tricks[dno];
      weight += cases[dno] * tricks[dno];
      numDist++;
    }
    else
      return false;
  }

  return true;
}


bool Tricks::possible(
  const Tricks& explained,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned char& weightAdded) const
{
  assert(tricks.size() == explained.size());
  assert(tricks.size() == residuals.size());
  assert(tricks.size() == cases.size());
  assert(tricks.size() == additions.size());

  weightAdded = 0;
  for (unsigned i = 0; i < tricks.size(); i++)
  {
    // If the cover has an entry that has not already been set:
    if (tricks[i] && ! explained.tricks[i])
    {
      if (residuals.tricks[i])
      {
        // We need that entry.  The residuals is a binary vector.
        additions.tricks[i] = 1;
        weightAdded += cases[i];
      }
      else
      {
        // We cannot have that entry.
        return false;
      }
    }
    else
      additions.tricks[i] = 0;
  }

  // Could still have been fully contained.
  return (weightAdded > 0);
}


bool Tricks::possible(
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned char& weightAdded) const
{
  assert(tricks.size() == residuals.size());
  assert(tricks.size() == cases.size());
  assert(tricks.size() == additions.size());

  weightAdded = 0;
  for (unsigned i = 0; i < tricks.size(); i++)
  {
    if (tricks[i] == 0)
      additions.tricks[i] = 0;
    else if (residuals.tricks[i])
    {
      // We need that entry.  The residuals is a binary vector.
      additions.tricks[i] = 1;
      weightAdded += cases[i];
    }
    else
    {
      // We cannot have that entry.
      return false;
    }
  }

  // Could still have been fully contained.
  return (weightAdded > 0);
}


Tricks& Tricks::operator += (const Tricks& tricks2)
{
  // No checking that we don't go out of the positive range (0..3).
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] += tricks2.tricks[i];
  
  return * this;
}


Tricks& Tricks::operator -= (const Tricks& tricks2)
{
  // No checking of <= first.
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= tricks2.tricks[i];
  
  return * this;
}


Tricks& Tricks::operator |= (const Tricks& tricks2)
{
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] |= tricks2.tricks[i];
  
  return * this;
}


void Tricks::orSymm(const Tricks& tricks2)
{
  const unsigned len = tricks.size();
  assert(len == tricks2.tricks.size());

  for (unsigned i = 0; i < len; i++)
    tricks[i] |= tricks2.tricks[i] | tricks2.tricks[len-1-i];
}


bool Tricks::operator == (const Tricks& tricks2) const
{
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    if (tricks[i] != tricks2.tricks[i])
      return false;
  
  return true;
}


bool Tricks::operator <= (const Tricks& tricks2) const
{
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    if (tricks[i] > tricks2.tricks[i])
      return false;
  
  return true;
}


unsigned Tricks::size() const
{
  return tricks.size();
}


string Tricks::strList() const
{
  stringstream ss;

  for (unsigned i = 0; i < tricks.size(); i++)
    ss << setw(2) << i << setw(4) << +tricks[i] << "\n";

  return ss.str();
}


string Tricks::strShort() const
{
  string s;
  for (auto t: tricks)
    s += (t ? "1" : "-");
  return s + "  ";
}


string Tricks::strSpaced() const
{
  stringstream ss;

  for (auto t: tricks)
    ss << setw(2) << +t;

  return ss.str() + "\n";
}


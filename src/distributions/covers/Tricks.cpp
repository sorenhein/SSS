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

#include "Product.h"
#include "Profile.h"
#include "CoverRowOld.h"

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
  const vector<unsigned char>& values,
  unsigned char& sum)
{
  tricks.resize(values.size());

  sum = 0;
  for (unsigned i = 0; i < values.size(); i++)
  {
    tricks[i] = values[i];
    sum += values[i];
  }
}


void Tricks::set(
  const Tricks& tricks2,
  unsigned char& sum)
{
  // TODO Do we need this long-term, or just use = ?

  tricks = tricks2.tricks;

  sum = 0;
  for (unsigned i = 0; i < tricks2.size(); i++)
    sum += tricks2.tricks[i];
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


void Tricks::prepare(
  const Product& product,
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


void Tricks::prepare(
  // const CoverSpec& spec,
  const CoverRowOld& coverRow,
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

  for (unsigned dno = 0; dno < len; dno++)
  {
    if (coverRow.includes(distProfiles[dno]))
    {
      tricks[dno] = 1;
      weight += static_cast<unsigned>(cases[dno]);
      numDist++;
    }
  }
}


bool Tricks::possible(
  const Tricks& explained,
  const Tricks& residuals,
  Tricks& additions,
  unsigned char& tricksAdded) const
{
  assert(tricks.size() == explained.size());
  assert(tricks.size() == residuals.size());
  assert(tricks.size() == additions.size());

  tricksAdded = 0;
  for (unsigned i = 0; i < tricks.size(); i++)
  {
    // If the cover has an entry that has not already been set:
    if (tricks[i] && ! explained.tricks[i])
    {
      if (residuals.tricks[i])
      {
        // We need that entry.
        additions.tricks[i] = 1;
        tricksAdded++;
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
  return (tricksAdded > 0);
}


CoverState Tricks::explain(Tricks& tricks2) const
{
  assert(tricks.size() == tricks2.tricks.size());

  CoverState state = COVER_DONE;

  for (unsigned i = 0; i < tricks.size(); i++)
  {
    if (tricks[i] > tricks2.tricks[i])
      return COVER_IMPOSSIBLE;
    else if (tricks[i] < tricks2.tricks[i])
      state = COVER_OPEN;
  }

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks2.tricks[i] -= tricks[i];
  
  return state;
}


void Tricks::add(
  const Tricks& additions,
  Tricks& residuals,
  unsigned char& residualsSum,
  unsigned char& numDist)
{
  // additions are disjoint from tricks.
  assert(tricks.size() == additions.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
  {
    const unsigned char t = additions.tricks[i];
    tricks[i] += t;
    residuals.tricks[i] -= t;
    residualsSum -= t;
    numDist += t;
  }
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
  for (unsigned i = 0; i < tricks.size(); i++)
    s += (tricks[i] ? "1" : "-");
  return s + "  ";
}


string Tricks::strSpaced() const
{
  stringstream ss;

  for (unsigned i = 0; i < tricks.size(); i++)
    ss << setw(2) << +tricks[i];

  return ss.str() + "\n";
}


/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Covers.h"
#include "CoverMemory.h"

#include "../strategies/result/Result.h"
#include "../const.h"


Covers::Covers()
{
  Covers::reset();
}


void Covers::reset()
{
  covers.clear();
  fits.clear();
}


void Covers::prepare(
  const CoverMemory& coverMemory,
  const unsigned char maxLength,
  const unsigned char maxTops,
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const vector<unsigned char>& cases)
{
  assert(lengths.size() == tops.size());
  assert(lengths.size() == cases.size());
  assert(maxLength >= 2);
  assert(maxTops >= 1);

  covers.resize(10);
  auto citer = covers.begin();

  for (auto miter = coverMemory.begin(maxLength, maxTops);
      miter != coverMemory.end(maxLength, maxTops); miter++)
  {
    assert(citer != covers.end());
    citer->prepare(lengths, tops, cases, * miter);
    citer++;
  }

  if (citer != covers.end())
    covers.erase(citer, covers.end());

  covers.sort([](const Cover& cover1, const Cover& cover2)
  {
    return (cover1.getWeight() >= cover2.getWeight());
  });
}


CoverState Covers::explain(const list<Result>& results)
{
  CoverState state = COVER_OPEN;
  auto iter = covers.begin();
  fits.clear();

  vector<unsigned char> tricks(results.size());
  unsigned char tmin = UCHAR_NOT_SET;
  unsigned i = 0;

  for (auto& res: results)
  {
    tricks[i] = res.getTricks();
    if (tricks[i] < tmin)
      tmin = tricks[i];

    i++;
  }

  for (i = 0; i < tricks.size(); i++)
    tricks[i] -= tmin;

  while (true)
  {
    if (iter == covers.end())
      return COVER_IMPOSSIBLE;

    state = iter->explain(tricks);

    if (state == COVER_DONE)
    {
      fits.push_back(&* iter);
cout << "Fits\n";
cout << Covers::str();
      return COVER_DONE;
    }
    else if (state == COVER_OPEN)
    {
      fits.push_back(&* iter);
      continue;
    }
    else
      iter++;
  }

  // Can't happen
  return COVER_STATE_SIZE;
}


string Covers::str() const
{
  string s;
  for (auto& eptr: fits)
  {
    s += eptr->str() + "\n";
  }
  return s;
}


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

#include "../../strategies/result/Result.h"
#include "../../const.h"


Covers::Covers()
{
  Covers::reset();
}


void Covers::reset()
{
  covers.clear();
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

  covers.resize(coverMemory.size(maxLength, maxTops));
  auto citer = covers.begin();

  for (auto miter = coverMemory.begin(maxLength, maxTops);
      miter != coverMemory.end(maxLength, maxTops); miter++)
  {
    assert(citer != covers.end());
    citer->prepare(lengths, tops, cases, * miter);

    if (citer->getWeight() == 0)
    {
      cout << "Covers::prepare: " << 
        +maxLength << ", " << +maxTops << "\n";
      cout << "Adding " << citer->str() << "\n";
      assert(citer->getWeight() != 0);
    }

    citer++;
  }

  covers.sort([](const Cover& cover1, const Cover& cover2)
  {
    return (cover1.getWeight() >= cover2.getWeight());
  });
}


void Covers::setup(
  const list<Result>& results,
  vector<unsigned char>& tricks,
  unsigned char& tricksMin) const
{
  tricks.resize(results.size());
  tricksMin = UCHAR_NOT_SET;
  unsigned i = 0;

  for (auto& res: results)
  {
    tricks[i] = res.getTricks();
    if (tricks[i] < tricksMin)
      tricksMin = tricks[i];

    i++;
  }

  for (i = 0; i < tricks.size(); i++)
    tricks[i] -= tricksMin;
}


CoverState Covers::explain(
  const list<Result>& results,
  ResExpl& resExpl) const
{
  CoverState state = COVER_OPEN;
  auto iter = covers.begin();

  vector<unsigned char> tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);
  resExpl.setMinimum(tmin);

  while (true)
  {
    if (iter == covers.end())
    {
      cout << Covers::strDebug("Left with", tricks);
      return COVER_IMPOSSIBLE;
    }

    state = iter->explain(tricks);

    if (state == COVER_DONE)
    {
      resExpl.insert(* iter);
      return COVER_DONE;
    }
    else if (state == COVER_OPEN)
    {
      resExpl.insert(* iter);
      continue;
    }
    else
    {
      // cout << Covers::strDebug("Could not use", tricks);
      iter++;
    }
  }

  // Can't happen
  return COVER_STATE_SIZE;
}


string Covers::strDebug(
  const string& title,
  const vector<unsigned char>& tricks) const
{
  stringstream ss;
  ss << title << "\n";

  for (unsigned i = 0; i < tricks.size(); i++)
    if (tricks[i])
      ss << i << ": " << +tricks[i] << "\n";
  ss << "\n";
  return ss.str();
}


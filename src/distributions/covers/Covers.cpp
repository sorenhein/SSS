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


list<ExplData>::iterator Covers::dominator(
  list <ExplData>& fits,
  const Cover& cover) const
{
  // Returns fits.end() if there is no dominator.
  // Returns the dominator with the highest level number,
  // and among these, the one with the lowest weight.
  
  list<ExplData>::iterator resIter = fits.end();
  unsigned char levelBest = 0;
  unsigned char weightBest = UCHAR_NOT_SET;

  for (auto iter = fits.begin(); iter != fits.end(); iter++)
  {
    if (! (cover <= * (iter->coverPtr)))
      continue;

    if (cover.getWeight() == iter->weight)
    {
      // TODO
      cout << "Don't know yet how to deal with repeats" << endl;
      cout << "Already have:\n";
      cout << Covers::str(fits);
      cout << "Cover:\n";
      cout << cover.str() << "\n";
      cout << cover.strProfile() << "\n";
      cout << "iter:\n";
      cout << iter->coverPtr->str() << "\n";
      cout << iter->coverPtr->strProfile() << endl;
      assert(false);
    }

    if (resIter == fits.end() || 
        iter->level > levelBest ||
        (iter->level == levelBest && iter->weight < weightBest))
    {
      resIter = iter;
      levelBest = iter->level;
      weightBest = iter->weight;
    }
  }

  return resIter;
}


void Covers::insert(
  list<ExplData>& fits,
  const Cover& cover) const
{
  auto domIter = Covers::dominator(fits, cover);

  if (domIter == fits.end())
  {
    fits.emplace_back(ExplData());
    ExplData& ed = fits.back();

    ed.coverPtr = &cover;
    ed.weight = cover.getWeight();
    ed.numDist = cover.getNumDist();
    ed.level = 0;
  }
  else
  {
    ExplData& ed = * fits.emplace(domIter, ExplData());

    ed.coverPtr = &cover;
    ed.weight = cover.getWeight();
    ed.numDist = cover.getNumDist();
    ed.level = domIter->level + 1;
  }
}


CoverState Covers::explain(
  const list<Result>& results,
  list<ExplData>& fits) const
{
  CoverState state = COVER_OPEN;
  auto iter = covers.begin();
  fits.clear();

  vector<unsigned char> tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);


// cout << "tmin " << +tmin << "\n";

  while (true)
  {
    if (iter == covers.end())
    {
/* */
cout << "Left with\n";
for (unsigned i = 0; i < tricks.size(); i++)
  if (tricks[i])
    cout << i << ": " << +tricks[i] << "\n";
cout << "\n";
/* */

      return COVER_IMPOSSIBLE;
    }

    state = iter->explain(tricks);

    if (state == COVER_DONE)
    {
      Covers::insert(fits, * iter);
      return COVER_DONE;
    }
    else if (state == COVER_OPEN)
    {
      Covers::insert(fits, * iter);
      continue;
    }
    else
    {
/*
cout << "Could not use\n";
for (i = 0; i < tricks.size(); i++)
  if (tricks[i])
    cout << i << ": " << +tricks[i] << "\n";
cout << "\n";
cout << iter->strProfile() << "\n";
*/
      iter++;
    }
  }

  // Can't happen
  return COVER_STATE_SIZE;
}


string Covers::str(list<ExplData>& fits) const
{
  string s;
  for (auto& fit: fits)
  {
    s += fit.coverPtr->str() + " [" + 
      to_string(fit.coverPtr->index()) + ": " +
      to_string(fit.numDist) + ", " +
      to_string(fit.weight) + "]\n";
  }
  return s;
}


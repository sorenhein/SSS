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


void Covers::prepareSpecific(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const unsigned char maxLength,
  const unsigned char maxTops,
  list<Cover>::iterator& iter)
{
  iter = covers.begin();
  CoverSpec spec;

  // This loop does combinations of the form <=, ==, >=.

  for (unsigned char length = 0; length <= maxLength; length++)
  {
    spec.length = length;
    for (unsigned char top = 0; top <= maxTops; top++)
    {
      spec.top = top;
      for (unsigned mode = 0; mode < COVER_MODE_SIZE; mode++)
      {
        spec.mode = static_cast<CoverMode>(mode);
        for (unsigned lOper = 0; lOper < COVER_OPERATOR_SIZE; lOper++)
        {
          if ((length == 0 || length == maxLength) && lOper != COVER_EQUAL)
            continue;

          spec.lengthOper = static_cast<CoverOperator>(lOper);

          for (unsigned tOper = 0; tOper < COVER_OPERATOR_SIZE; tOper++)
          {
            if ((top == 0 || top == maxTops) && tOper != COVER_EQUAL)
              continue;

            spec.topOper = static_cast<CoverOperator>(tOper);

            assert(iter != covers.end());
            Cover& cover = * iter;

            cover.prepare(lengths, tops, spec);
          }
        }
      }
    }
  }
}


void Covers::prepareMiddles(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const unsigned char maxLength,
  const unsigned char maxTops,
  list<Cover>::iterator& iter)
{
  // This loop does more global distributions of the form
  // "4-2 or better".

  CoverSpec spec;
  spec.lengthOper = static_cast<CoverOperator>(COVER_WITHIN_RANGE);

  // With 5 or 6 cards, we run from 1 to 2 as the lower end.
  const unsigned char middleCount = (maxLength-1) >> 1;

  for (unsigned char length = 1; length <= middleCount; length++)
  {
    const unsigned char mirrorValue = maxLength - length;
    spec.length = length;
    for (unsigned char top = 0; top <= maxTops; top++)
    {
      spec.top = top;
      for (unsigned mode = 0; mode < COVER_MODE_SIZE; mode++)
      {
        spec.mode = static_cast<CoverMode>(mode);
        for (unsigned tOper = 0; tOper < COVER_OPERATOR_SIZE; tOper++)
        {
          if ((top == 0 || top == maxTops) && tOper != COVER_EQUAL)
            continue;

          spec.topOper = static_cast<CoverOperator>(tOper);

          assert(iter != covers.end());
          Cover& cover = * iter;

          cover.prepare(lengths, tops, spec);
        }
      }
    }
  }
}


void Covers::prepare(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const unsigned char maxLength,
  const unsigned char maxTops)
{
  assert(lengths.size() == tops.size());
  assert(maxLength >= 1);
  assert(maxTops >= 1);

  // We consider two kinds of distribution information:
  // (1) <=, ==, >= a specific number of West cards.
  // (2) Both West and East in a certain middle range.
  // 
  // For (1) there are generally 3 operators for each of lengths and tops;
  // 4 ways to combine them; and a number of possible lengths and tops.
  // For the maximum and minimum value of lengths and tops there is
  // only one operator, ==.
  //
  // For (2) there are (maxLength-1) >> 1 interesting splits.
  // For example, for 7 cards there are any 4-3; up to 5-2; up to 6-1,
  // and (7-1) >> 1 is 3.  For 8 cards there are also 3, as 4-4 is
  // already covered by the "exactly 4 West cards" split above.

  const unsigned middleCount = (maxLength-1) >> 2;

  const unsigned coverCount = COVER_MODE_SIZE *
    ((maxLength-1) * (maxTops-1) * 
      (COVER_OPERATOR_SIZE-1) * (COVER_OPERATOR_SIZE-1) +
    2 * (maxTops-1) * 1 * (COVER_OPERATOR_SIZE-1) +
    (maxLength-1) * 2 * (COVER_OPERATOR_SIZE-1) * 1 +
    2 * 2 * 1 * 1) +

    COVER_MODE_SIZE * middleCount *
    ((maxTops-1) * 1 * (COVER_OPERATOR_SIZE-1) +
    2 * 1 * 1);

  covers.resize(coverCount);

  list<Cover>::iterator iter;
  Covers::prepareSpecific(lengths, tops, maxLength, maxTops, iter);
  Covers::prepareMiddles(lengths, tops, maxLength, maxTops, iter);

  covers.sort([](const Cover& cover1, const Cover& cover2)
  {
    return (cover1.getWeight() >= cover2.getWeight());
  });
}


CoverState Covers::explain(const vector<Result>& results)
{
  CoverState state = COVER_OPEN;
  auto iter = covers.begin();

  vector<unsigned char> tricks(results.size());
  unsigned char tmin = UCHAR_NOT_SET;
  for (unsigned i = 0; i < results.size(); i++)
  {
    tricks[i] = results[i].getTricks();
    if (tricks[i] < tmin)
      tmin = tricks[i];
  }

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= tmin;

  while (true)
  {
    if (iter == covers.end())
      return COVER_IMPOSSIBLE;

    state = iter->explain(tricks);

    if (state == COVER_DONE)
    {
      fits.push_back(&* iter);
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


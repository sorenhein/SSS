/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <mutex>
#include <cassert>

#include "Study.h"

#include "../result/Result.h"
#include "../result/Ranges.h"
#include "../result/ResConvert.h"

extern ResConvert resConvert;


Study::Study()
{
  Study::reset();
}


void Study::reset()
{
  summary.clear();
  studiedFlag = false;

  profiles.clear();
}


void Study::study(const list<Result>& results)
{
  if (results.empty())
   return;

  const unsigned groups = 
    static_cast<unsigned>(sqrt(static_cast<float>(results.size())));

  summary.clear();
  summary.resize(groups);

  unsigned i = 0;
  for (auto& result: results)
  {
    summary[i % groups] += result.getTricks();
    i++;
  }
  studiedFlag = true;
}


void Study::unstudy()
{
  studiedFlag = false;
}


bool Study::studied() const
{
  return studiedFlag;
}


void Study::scrutinize(
  const list<Result>& results,
  const Ranges& ranges)
{
  resConvert.scrutinizeRange(results, ranges, profiles);
}


bool Study::maybeLessEqualStudied(const Study& study2) const
{
  // This uses studied results if possible, otherwise the basic method.

  assert(studiedFlag);
  assert(study2.studiedFlag);

  for (unsigned i = 0; i < summary.size(); i++)
  {
    if (summary[i] > study2.summary[i])
      return false;
  }

  // Expect the caller to do the full comparison.  The summaries may
  // still hide differences.
  return true;
}


Compare Study::comparePartialPrimaryStudied(const Study& study2) const
{
  // This only goes as far as the studied results allows.
  // Unlike in the other methods: 
  //                  >=   ==   <=   !=
  // WIN_EQUAL        poss poss poss poss
  // WIN_FIRST        poss           poss
  // WIN_SECOND                 poss poss
  // WIN_DIFFERENT                   poss

  assert(studiedFlag);

  bool greaterFlag = false;
  bool lowerFlag = false;
  
  for (unsigned i = 0; i < summary.size(); i++)
  {
    if (summary[i] > study2.summary[i])
      greaterFlag = true;
    else if (summary[i] < study2.summary[i])
      lowerFlag = true;
  }

  if (greaterFlag)
    return (lowerFlag ? WIN_DIFFERENT : WIN_FIRST);
  else
    return (lowerFlag ? WIN_SECOND : WIN_EQUAL);
}


bool Study::lessEqualScrutinized(const Study& study2) const
{
  // This uses the scrutinized results, which must exist.
  // The existence cannot be checked from here, as it is at the
  // Strategies level.

  assert(profiles.size() == study2.profiles.size());

  auto piter1 = profiles.begin();
  auto piter2 = study2.profiles.begin();
  while (piter1 != profiles.end())
  {
    if (! resConvert.greaterEqual(* piter2, * piter1))
      return false;

    piter1++;
    piter2++;
  }

  return true;
}


Compare Study::comparePrimaryScrutinized(const Study& study2) const
{
  // This too uses the scrutinized results.

  assert(profiles.size() == study2.profiles.size());
  assert(! profiles.empty());

  auto piter1 = profiles.begin();
  auto piter2 = study2.profiles.begin();
  bool greaterFlag = false;
  bool lowerFlag = false;
  while (piter1 != profiles.end())
  {
    const unsigned char b1 = resConvert.greaterEqual(* piter1, * piter2);
    const unsigned char b2 = resConvert.greaterEqual(* piter2, * piter1);

    if (b1)
    {
      if (! b2)
      {
        if (lowerFlag)
          return WIN_DIFFERENT;
          
        greaterFlag = true;
      }
    }
    else if (b2)
    {
      if (greaterFlag)
        return WIN_DIFFERENT;

      lowerFlag = true;
    }
    else
      return WIN_DIFFERENT;

    piter1++;
    piter2++;
  }

  if (greaterFlag)
    return WIN_FIRST;
  else if (lowerFlag)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <mutex>
#include <cassert>

#include "Study.h"

#include "../result/Ranges.h"

// A major time drain is the component-wise comparison of results.  
// In the most optimized implementation, 5 result entries are
// gathered into a 10-bit vector, and two such 10-bit vectors
// are compared in a 20-bit lookup.  The lookup table must be global
// and initialized once.

#define LOOKUP_GROUP 5
#define LOOKUP_BITS (LOOKUP_GROUP + LOOKUP_GROUP)
#define LOOKUP_SIZE (LOOKUP_BITS + LOOKUP_BITS)

mutex mtxStudy;
static bool init_flag = false;
vector<unsigned char> lookupGE;


Study::Study()
{
  mtxStudy.lock();
  if (! init_flag)
  {
    Study::setConstants();
    init_flag = true;
  }
  mtxStudy.unlock();

  Study::reset();
}


void Study::setConstants()
{
  // In order to speed up the comparison of Strategy's, we group
  // their trick excesses (over the minimum) into a profile list,
  // where each entry combines five entries into 10 bits.
  // We can then look up two 10-bit profiles and get a partial answer.
  
  lookupGE.resize(1 << LOOKUP_SIZE);
  for (unsigned i = 0; i < (1 << LOOKUP_BITS); i++)
  {
    for (unsigned j = 0; j < (1 << LOOKUP_BITS); j++)
    {
      unsigned flagGE = 1;
      unsigned i0 = i;
      unsigned j0 = j;
      for (unsigned p = 0; p < LOOKUP_GROUP; p++)
      {
        // Break each index down into a two-bit number.
        const unsigned entry1 = (i0 & 0x3);
        const unsigned entry2 = (j0 & 0x3);
        if (entry1 < entry2)
        {
          // Can no longer be >=.
          flagGE = 0;
          break;
        }
        else
        {
          i0 >>= 2;
          j0 >>= 2;
        }
      }

      if (flagGE)
        lookupGE[(i << LOOKUP_BITS) | j] = 1;
    }
  }
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
    summary[i % groups] += result.tricks();
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
  assert(ranges.size() >= results.size());
  profiles.clear();

  auto riter = results.begin();
  auto miter = ranges.begin();

  // IDEA: We combine consecutive results in groups of 5.
  // It might be better to space them out, so that the results
  // from 0, 6, 12, 18, ... (for example) go into the same group.

  unsigned counter = 0;
  unsigned profile = 0;
  while (riter != results.end())
  {
    assert(miter != ranges.end());
    if (miter->dist() < riter->dist())
    {
      miter++;
      continue;
    }

    assert(riter->dist() == miter->dist());

    const unsigned diff = riter->tricks() - miter->min();
    assert(diff < 4); // Must fit in 2 bits for this to work

    profile = (profile << 2) | diff;
    counter++;

    if (counter == LOOKUP_GROUP - 1)
    {
      profiles.push_back(profile);
      counter = 0;
      profile = 0;
    }

    riter++;
    miter++;
  }

  if (counter > 0)
    profiles.push_back(profile);
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
    if (! lookupGE[((* piter2) << 10) | (* piter1)])
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
    const unsigned char b1 = lookupGE[((* piter1) << 10) | (* piter2)];
    const unsigned char b2 = lookupGE[((* piter2) << 10) | (* piter1)];

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


/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "ResConvert.h"
#include "Result.h"
#include "RAnges.h"

// A major time drain is the component-wise comparison of results
// in Strategy's.  In the most optimized implementation in Study, 
// 5 result entries are gathered into a 10-bit vector, and two such 
// 10-bit vectors are compared in a 20-bit lookup.  The lookup table 
// must be global and initialized once.
//
// This is also useful when finding covers of verbal descriptions
// of results.

#define LOOKUP_GROUP 5
#define LOOKUP_BITS (LOOKUP_GROUP + LOOKUP_GROUP)
#define LOOKUP_SIZE (LOOKUP_BITS + LOOKUP_BITS)


ResConvert::ResConvert()
{
  ResConvert::setConstants();
}


void ResConvert::setConstants()
{
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


size_t ResConvert::profileSize(const size_t len) const
{
  // 2 for 1-10, 4 for 11-20, 6 for 21-30 etc.
  return 2 * ((len + LOOKUP_GROUP + 4) / (2 * LOOKUP_GROUP));
}


void ResConvert::increment(
  unsigned& counter,
  unsigned& profile,
  const unsigned incr,
  list<unsigned>& profiles) const
{
  profile = (profile << 2) | incr;
  counter++;

  // TODO Is this actually correct, or do we make too-small groups?!
  if (counter == LOOKUP_GROUP - 1)
  {
    profiles.push_back(profile);
    counter = 0;
    profile = 0;
  }
}


void ResConvert::increment(
  unsigned& counter,
  unsigned& accum,
  const unsigned char value,
  unsigned& position,
  unsigned& result) const
{
  // result will typically be some vector[position].
  accum = (accum << 2) | value;
  counter++;

  if (counter == LOOKUP_GROUP)
  {
    result = accum;
    counter = 0;
    accum = 0;
    position++;
  }
}


void ResConvert::finish(
  unsigned& counter,
  unsigned& accum,
  unsigned& position,
  unsigned& result) const
{
  if (counter == 0)
    return;

  result = accum << 2 * (LOOKUP_GROUP - counter);
  counter = 0;
  accum = 0;
  position++;
}


void ResConvert::scrutinizeRange(
  const list<Result>& results,
  const Ranges& ranges,
  list<unsigned>& profiles) const
{
  // This is called from Study::scrutinize.
  // In order to speed up the comparison of Strategy's, we group
  // their trick excesses (over the minimum) into a profile list,
  // where each entry combines five entries into 10 bits.
  // We can then look up two 10-bit profiles and get a partial answer.

  assert(ranges.size() >= results.size());
  profiles.clear();

  auto riter = results.begin();
  auto miter = ranges.begin();

  // We combine consecutive results in groups of 5.
  // It might be better to space them out, so that the results
  // from 0, 6, 12, 18, ... (for example) go into the same group.

  unsigned counter = 0;
  unsigned profile = 0;
  while (riter != results.end())
  {
    assert(miter != ranges.end());
    if (miter->dist() < riter->getDist())
    {
      miter++;
      continue;
    }

    assert(riter->getDist() == miter->dist());

    const unsigned diff = riter->getTricks() - miter->min();
    assert(diff < 4); // Must fit in 2 bits for this to work

    ResConvert::increment(counter, profile, diff, profiles);

    riter++;
    miter++;
  }

  if (counter > 0)
    profiles.push_back(profile);
}


void ResConvert::scrutinizeConstant(
  const list<Result>& results,
  const unsigned minTricks,
  list<unsigned>& profiles) const
{
  // This is similar, but there is a constant number of tricks that
  // is subtracted, rather than a distribution-dependent range.

  profiles.clear();
  unsigned counter = 0;
  unsigned profile = 0;

  for (auto& result: results)
  {
    const unsigned diff = result.getTricks() - minTricks;
    assert(diff < 4); // Must fit in 2 bits for this to work

    ResConvert::increment(counter, profile, diff, profiles);
  }

  if (counter > 0)
    profiles.push_back(profile);
}



void ResConvert::scrutinizeBinary(
  const list<unsigned char>& binaryTricks,
  list<unsigned>& profiles) const
{
  // This too is similar, but we scrutinize a binary vector.

  profiles.clear();
  unsigned counter = 0;
  unsigned profile = 0;

  for (auto trick: binaryTricks)
  {
    assert(trick < 2);

    ResConvert::increment(counter, profile, trick, profiles);
  }

  if (counter > 0)
    profiles.push_back(profile);
}


unsigned char ResConvert::lookup(
  const vector<unsigned>& profiles,
  const size_t lastForward,
  const size_t index) const
{
  if (index <= lastForward)
  {
    // The forward half.
    const size_t group = index / LOOKUP_GROUP;
    const size_t shift = 2 * (LOOKUP_GROUP - 1 - (index % LOOKUP_GROUP));
  
    assert(group < profiles.size() / 2);
    return static_cast<unsigned>((profiles[group] >> shift) & 0x3);
  }
  else
  {
    // The backward half.
    const size_t rebased = index - lastForward - 1;
    const size_t group = profiles.size() / 2 + rebased / LOOKUP_GROUP;
    const size_t shift = 2 * (LOOKUP_GROUP - 1 - (rebased % LOOKUP_GROUP));

    assert(group < profiles.size());
    return static_cast<unsigned>((profiles[group] >> shift) & 0x3);
  }
}


bool ResConvert::fullHouse(const unsigned value) const
{
  return (value == 0x3ff);
}


unsigned ResConvert::limit(
  const size_t lastForward,
  const unsigned value) const
{
  const unsigned mod = lastForward % LOOKUP_GROUP;
  if (mod == 0)
    return 0;
  else if (mod == 1)
    return (value & 0x300);
  else if (mod == 2)
    return (value & 0x3c0);
  else if (mod == 3)
    return (value & 0x3f0);
  else if (mod == 4)
    return (value & 0x3fc);
  else
  {
    assert(false);
    return 0;
  }
}


bool ResConvert::greaterEqual(
  const unsigned arg1,
  const unsigned arg2) const
{
  return (lookupGE[(arg1 << 10) | arg2] ? 1 : 0);
}


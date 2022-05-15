/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "TrickConvert.h"

// This is similar to ResConvert, but with a different grouping.
// In ResConvert we examine the excess tricks for each distribution
// separately, and this (I hope!) cannot exceed 3.
// Here we examine the excess tricks over a constant baseline across
// all distributions, and this can indeed exceed 3.
// So we go up to the next size of 7 which means we cannot fit
// as many groups into a reasonable lookup table size.
// It might have been possible to make a general convert class,
// but it didn't seem worth it.

// It will almost work to change these define's in synchrony,
// but limit() has to be done manually.

#define LOOKUP_GROUP 4 // Groups
#define LOOKUP_WIDTH 3 // Bits per group
#define LOOKUP_MASK 0x7 // To get the three bits
#define LOOKUP_BITS (LOOKUP_WIDTH * LOOKUP_GROUP) // 12 bits
#define LOOKUP_SIZE (LOOKUP_BITS + LOOKUP_BITS) // 24 bits 
#define FULL_HOUSE 0xfff // All 12 bits


TrickConvert::TrickConvert()
{
  TrickConvert::setConstants();
}


void TrickConvert::setConstants()
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
        // Break each index down into a three-bit number.
        const unsigned entry1 = (i0 & LOOKUP_MASK);
        const unsigned entry2 = (j0 & LOOKUP_MASK);
        if (entry1 < entry2)
        {
          // Can no longer be >=.
          flagGE = 0;
          break;
        }
        else
        {
          i0 >>= LOOKUP_WIDTH;
          j0 >>= LOOKUP_WIDTH;
        }
      }

      if (flagGE)
        lookupGE[(i << LOOKUP_BITS) | j] = 1;
    }
  }
}


size_t TrickConvert::profileSize(const size_t len) const
{
  // 2 for 1-(2_LOOKUP_GROUP), 4 up to (4*LOOKUP_GROUP), 6 etc.
  return 2 * ((len + LOOKUP_GROUP + (LOOKUP_GROUP-1)) / (2 * LOOKUP_GROUP));
}


void TrickConvert::increment(
  unsigned& counter,
  unsigned& accum,
  const unsigned char value,
  unsigned& position,
  unsigned& result) const
{
  // result will typically be some vector[position].
  accum = (accum << LOOKUP_WIDTH) | value;
  counter++;

  if (counter == LOOKUP_GROUP)
  {
    result = accum;
    counter = 0;
    accum = 0;
    position++;
  }
}


void TrickConvert::finish(
  unsigned& counter,
  unsigned& accum,
  unsigned& position,
  unsigned& result) const
{
  if (counter == 0)
    return;

  result = accum << LOOKUP_WIDTH * (LOOKUP_GROUP - counter);
  counter = 0;
  accum = 0;
  position++;
}


unsigned char TrickConvert::lookup(
  const vector<unsigned>& profiles,
  const size_t lastForward,
  const size_t index) const
{
  if (index <= lastForward)
  {
    // The forward half.
    const size_t group = index / LOOKUP_GROUP;
    const size_t shift = 
      LOOKUP_WIDTH * (LOOKUP_GROUP - 1 - (index % LOOKUP_GROUP));
  
    assert(group < profiles.size() / 2);
    return static_cast<unsigned>((profiles[group] >> shift) & LOOKUP_MASK);
  }
  else
  {
    // The backward half.
    const size_t rebased = index - lastForward - 1;
    const size_t group = profiles.size() / 2 + rebased / LOOKUP_GROUP;
    const size_t shift = 
      LOOKUP_WIDTH * (LOOKUP_GROUP - 1 - (rebased % LOOKUP_GROUP));

    assert(group < profiles.size());
    return static_cast<unsigned>((profiles[group] >> shift) & LOOKUP_MASK);
  }
}


bool TrickConvert::fullHouse(const unsigned value) const
{
  return (value == FULL_HOUSE);
}


unsigned TrickConvert::limit(
  const size_t lastForward,
  const unsigned value) const
{
  // If the define's change, this method has to be adapted manually.
  const unsigned mod = lastForward % LOOKUP_GROUP;
  if (mod == 0)
    return 0;
  else if (mod == 1)
    return (value & 0xe00);
  else if (mod == 2)
    return (value & 0xfc0);
  else if (mod == 3)
    return (value & 0xff8);
  else
  {
    assert(false);
    return 0;
  }
}


bool TrickConvert::greaterEqual(
  const unsigned arg1,
  const unsigned arg2) const
{
  return (lookupGE[(arg1 << LOOKUP_BITS) | arg2] ? 1 : 0);
}


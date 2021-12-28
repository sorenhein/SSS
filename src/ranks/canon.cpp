/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <vector>
#include <cassert>

#include "Declarer.h"
#include "Opponents.h"
#include "canon.h"

#include "../utils/table.h"

#include "../const.h"

using namespace std;


vector<unsigned> HOLDING3_RANK_FACTOR;
vector<unsigned> HOLDING3_RANK_ADDER;

vector<unsigned> HOLDING2_RANK_SHIFT;
vector<unsigned> HOLDING2_RANK_ADDER;


void setCanonicalConstants()
{
  // First we set up tables for a single count of a certain position
  // (North, South, opps).  The factor is a power of 3 or 2 in order
  // to shift the trinary or binary holding up, and the adder is the
  // value (in "trits" or in bits) to add to the holding.

  vector<unsigned> HOLDING3_FACTOR;
  vector<vector<unsigned>> HOLDING3_ADDER;

  vector<unsigned> HOLDING2_SHIFT;
  vector<vector<unsigned>> HOLDING2_ADDER;

  HOLDING3_FACTOR.resize(MAX_CARDS+1);
  HOLDING3_FACTOR[0] = 1;
  for (unsigned c = 1; c < HOLDING3_FACTOR.size(); c++)
    HOLDING3_FACTOR[c] = 3 * HOLDING3_FACTOR[c-1];

  assert(SIDE_NORTH == 0);
  assert(SIDE_SOUTH == 1);

  HOLDING3_ADDER.resize(MAX_CARDS+1);
  for (unsigned c = 0; c < HOLDING3_FACTOR.size(); c++)
  {
    HOLDING3_ADDER[c].resize(3);
    HOLDING3_ADDER[c][2] = HOLDING3_FACTOR[c] - 1;
    HOLDING3_ADDER[c][1] = HOLDING3_ADDER[c][2] / 2;
    HOLDING3_ADDER[c][0] = 0;
  }

  HOLDING2_SHIFT.resize(MAX_CARDS+1);
  for (unsigned c = 0; c < HOLDING2_SHIFT.size(); c++)
    HOLDING2_SHIFT[c] = c;

  HOLDING2_ADDER.resize(MAX_CARDS+1);
  for (unsigned c = 0; c < HOLDING2_SHIFT.size(); c++)
  {
    HOLDING2_ADDER[c].resize(2);
    HOLDING2_ADDER[c][1] = (c == 0u ? 0u : (1u << c) - 1u);
    HOLDING2_ADDER[c][0] = 0;
  }

  // Then we set up tables for a complete rank, including counts with
  // North, South and opps.  Of course either North+South or opps will
  // have a rank, and not both, but the table works uniformly.

  // We store the counts of a rank in a 12-bit word.  As we only consider
  // a limited number of cards, we only fill out the table entries up
  // to a sum of 16 cards.

  assert(MAX_CARDS <= 15);
  HOLDING3_RANK_FACTOR.resize(4096);
  HOLDING3_RANK_ADDER.resize(4096);

  HOLDING2_RANK_SHIFT.resize(4096);
  HOLDING2_RANK_ADDER.resize(4096);

  for (unsigned oppCount = 0; oppCount < 16; oppCount++)
  {
    for (unsigned decl1Count = 0; 
        decl1Count <= MAX_CARDS - oppCount; decl1Count++)
    {
      for (unsigned decl2Count = 0; 
          decl2Count <= MAX_CARDS - oppCount - decl1Count; decl2Count++)
      {
        const unsigned sum = oppCount + decl1Count + decl2Count;
        const unsigned index = (oppCount << 8 ) |
          (decl1Count << 4) | decl2Count;
        
        HOLDING3_RANK_FACTOR[index] = HOLDING3_FACTOR[sum];

        HOLDING3_RANK_ADDER[index] = 
          HOLDING3_ADDER[oppCount][SIDE_OPPS] *
            HOLDING3_FACTOR[decl1Count + decl2Count] +
          HOLDING3_ADDER[decl1Count][SIDE_NORTH] *
            HOLDING3_FACTOR[decl2Count] +
          HOLDING3_ADDER[decl2Count][SIDE_SOUTH];

        HOLDING2_RANK_SHIFT[index] = HOLDING2_SHIFT[sum];

        HOLDING2_RANK_ADDER[index] = 
          (HOLDING2_ADDER[oppCount][PAIR_EW] << 
            (decl1Count + decl2Count)) |
          HOLDING2_ADDER[decl1Count + decl2Count][PAIR_NS];
      }
    }
  }
}


unsigned canonicalTrinary(
  const Declarer& dominant,
  const Declarer& recessive,
  const Opponents& opponents,
  const unsigned char maxGlobalRank)
{
  // This is similar to canonicalBoth, but only does holding3.
  // Actually it is only guaranteed to generate a canonical holding3
  // if there is no rank reduction among the opponents' cards.
  // For example, with AJ / KT missing Q9, if the trick goes
  // T, Q, A, - then we're left with J /K missing the 9, where
  // either side is >= the other.  Therefore the rotateFlag and the
  // order will depend on the order of comparison.
  // Therefore Combinations::getPtr looks up the canonical index.
  unsigned h3 = 0;

  // Exclude void
  for (unsigned char rank = maxGlobalRank; rank > 0; rank--)
  {
    const unsigned index =
      (static_cast<unsigned>(opponents.count(rank)) << 8) |
      (static_cast<unsigned>(dominant.count(rank)) << 4) |
       static_cast<unsigned>(recessive.count(rank));

    h3 =
      HOLDING3_RANK_FACTOR[index] * h3 +
      HOLDING3_RANK_ADDER[index];
  }
  return h3;
}


void canonicalBoth(
  const Declarer& dominant,
  const Declarer& recessive,
  const Opponents& opponents,
  const unsigned char maxGlobalRank,
  unsigned& holding3,
  unsigned& holding2)
{
  // This is similar to canonicalTrinary, but generates both the binary
  // and trinary holdings.
  holding3 = 0;
  holding2 = 0;

  // Exclude void
  for (unsigned char rank = maxGlobalRank; rank > 0; rank--)
  {
    const unsigned index =
      (static_cast<unsigned>(opponents.count(rank)) << 8) |
      (static_cast<unsigned>(dominant.count(rank)) << 4) |
       static_cast<unsigned>(recessive.count(rank));

    holding3 =
      HOLDING3_RANK_FACTOR[index] * holding3 +
      HOLDING3_RANK_ADDER[index];
    holding2 =
      (holding2 << HOLDING2_RANK_SHIFT[index]) |
      HOLDING2_RANK_ADDER[index];
  }
}


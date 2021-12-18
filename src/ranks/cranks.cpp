/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <vector>
#include <cassert>

#include "cranks.h"

#include "../const.h"
#include "../utils/table.h"

using namespace std;


extern vector<unsigned> HOLDING4_TO_HOLDING3;
extern vector<unsigned> HOLDING3_TO_HOLDING4;

extern vector<unsigned> HOLDING3_RANK_FACTOR;
extern vector<unsigned> HOLDING3_RANK_ADDER;

extern vector<unsigned> HOLDING2_RANK_SHIFT;
extern vector<unsigned> HOLDING2_RANK_ADDER;


void setRankConstants4()
{
  // Lookup of 8 cards in trit format.  There are 3^8 = 6561 entries,
  // each with a 16-bit number.

  // First make temporary tables with the square root of the numbers.
  vector<unsigned> h4_to_h3_partial(256);
  vector<unsigned> h3_to_h4_partial(81);

  for (unsigned c0 = 0; c0 < 3; c0++)
  {
    for (unsigned c1 = 0; c1 < 3; c1++)
    {
      for (unsigned c2 = 0; c2 < 3; c2++)
      {
        for (unsigned c3 = 0; c3 < 3; c3++)
        {
          const unsigned h3 = 27*c0 + 9*c1 + 3*c2 + c3;
          const unsigned h4 = (c0 << 6) | (c1 << 4) | (c2 << 2) | c3;

          h4_to_h3_partial[h4] = h3;
          h3_to_h4_partial[h3] = h4;
        }
      }
    }
  }

  // Then make the big tables.
  HOLDING4_TO_HOLDING3.resize(65536);
  HOLDING3_TO_HOLDING4.resize(6561);

  for (unsigned p0 = 0; p0 < 81; p0++)
  {
    for (unsigned p1 = 0; p1 < 81; p1++)
    {
      const unsigned h3 = 81*p0 + p1;
      const unsigned h4 = 
        (h3_to_h4_partial[p0] << 8) | h3_to_h4_partial[p1];

      HOLDING3_TO_HOLDING4[h3] = h4;
      HOLDING4_TO_HOLDING3[h4] = h3;
    }
  }
}


void setRankConstants23()
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


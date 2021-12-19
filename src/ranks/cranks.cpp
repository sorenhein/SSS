/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <vector>
#include <array>
#include <algorithm>
#include <cassert>

#include "../plays/Play.h"

#include "cranks.h"

#include "../const.h"
#include "../utils/table.h"

using namespace std;


extern vector<unsigned> HOLDING4_TO_HOLDING3;

vector<array<unsigned char, 4>> SORT4_PLAYS;

extern vector<unsigned> HOLDING3_RANK_FACTOR;
extern vector<unsigned> HOLDING3_RANK_ADDER;

extern vector<unsigned> HOLDING2_RANK_SHIFT;
extern vector<unsigned> HOLDING2_RANK_ADDER;

const vector<unsigned> HOLDING4_MASK_LOW =
{
  0x00000000, //  0
  0x00000000, //  1
  0x00000003, //  2
  0x0000000f, //  3
  0x0000003f, //  4
  0x000000ff, //  5
  0x000003ff, //  6
  0x00000fff, //  7
  0x00003fff, //  8
  0x0000ffff, //  9
  0x0003ffff, // 10
  0x000fffff, // 11
  0x003fffff, // 12
  0x00ffffff, // 13
  0x03ffffff, // 14
  0x0fffffff  // 15
};

const vector<unsigned> HOLDING4_MASK_HIGH =
{
  0x3fffffff, //  0
  0x3ffffffc, //  1
  0x3ffffff0, //  2
  0x3fffffc0, //  3
  0x3fffff00, //  4
  0x3ffffc00, //  5
  0x3ffff000, //  6
  0x3fffc000, //  7
  0x3fff0000, //  8
  0x3ffc0000, //  9
  0x3ff00000, // 10
  0x3fc00000, // 11
  0x3f000000, // 12
  0x3c000000, // 13
  0x30000000, // 14
  0x00000000  // 15
};


void set4to3();

void set4sort();

unsigned punchHolding4(
  const unsigned holding4,
  const Play& play);


void set4to3()
{
  // Lookup of 8 cards in trit format.  There are 3^8 = 6561 entries,
  // each with a 16-bit number.

  // First make temporary tables with the square root of the numbers.
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

          h3_to_h4_partial[h3] = h4;
        }
      }
    }
  }

  // Then make the big tables.
  HOLDING4_TO_HOLDING3.resize(65536);

  for (unsigned p0 = 0; p0 < 81; p0++)
  {
    for (unsigned p1 = 0; p1 < 81; p1++)
    {
      const unsigned h3 = 81*p0 + p1;
      const unsigned h4 = 
        (h3_to_h4_partial[p0] << 8) | h3_to_h4_partial[p1];

      HOLDING4_TO_HOLDING3[h4] = h3;
    }
  }
}


void set4sort()
{
  // A given 16-bit input is mapped onto an array of four sorted plays.
  // The input consists of four groups of four bits each.
  // Voids (0) may repeat, but others may not.

  SORT4_PLAYS.resize(65536);
  SORT4_PLAYS[0] = {0, 0, 0, 0};

  for (unsigned p0 = 1; p0 < 4; p0++)
  {
    for (unsigned p1 = 0; p1 < p0; p1++)
    {
      for (unsigned p2 = 0; p2 < max(p1, 1u); p2++)
      {
        for (unsigned p3 = 0; p3 < max(p2, 1u); p3++)
        {
          const array<unsigned char, 4> res = 
          { 
            static_cast<unsigned char>(p0), 
            static_cast<unsigned char>(p1),
            static_cast<unsigned char>(p2),
            static_cast<unsigned char>(p3)
          };

          SORT4_PLAYS[(p0 << 12) | (p1 << 8) | (p2 << 4) | p3] = res;
          SORT4_PLAYS[(p0 << 12) | (p1 << 8) | (p3 << 4) | p2] = res;
          SORT4_PLAYS[(p0 << 12) | (p2 << 8) | (p1 << 4) | p3] = res;
          SORT4_PLAYS[(p0 << 12) | (p2 << 8) | (p3 << 4) | p1] = res;
          SORT4_PLAYS[(p0 << 12) | (p3 << 8) | (p1 << 4) | p2] = res;
          SORT4_PLAYS[(p0 << 12) | (p3 << 8) | (p2 << 4) | p1] = res;

          SORT4_PLAYS[(p1 << 12) | (p0 << 8) | (p2 << 4) | p3] = res;
          SORT4_PLAYS[(p1 << 12) | (p0 << 8) | (p3 << 4) | p2] = res;
          SORT4_PLAYS[(p1 << 12) | (p2 << 8) | (p0 << 4) | p3] = res;
          SORT4_PLAYS[(p1 << 12) | (p2 << 8) | (p3 << 4) | p0] = res;
          SORT4_PLAYS[(p1 << 12) | (p3 << 8) | (p0 << 4) | p2] = res;
          SORT4_PLAYS[(p1 << 12) | (p3 << 8) | (p2 << 4) | p0] = res;

          SORT4_PLAYS[(p2 << 12) | (p1 << 8) | (p0 << 4) | p3] = res;
          SORT4_PLAYS[(p2 << 12) | (p1 << 8) | (p3 << 4) | p0] = res;
          SORT4_PLAYS[(p2 << 12) | (p0 << 8) | (p1 << 4) | p3] = res;
          SORT4_PLAYS[(p2 << 12) | (p0 << 8) | (p3 << 4) | p1] = res;
          SORT4_PLAYS[(p2 << 12) | (p3 << 8) | (p1 << 4) | p0] = res;
          SORT4_PLAYS[(p2 << 12) | (p3 << 8) | (p0 << 4) | p1] = res;

          SORT4_PLAYS[(p3 << 12) | (p1 << 8) | (p2 << 4) | p0] = res;
          SORT4_PLAYS[(p3 << 12) | (p1 << 8) | (p0 << 4) | p2] = res;
          SORT4_PLAYS[(p3 << 12) | (p2 << 8) | (p1 << 4) | p0] = res;
          SORT4_PLAYS[(p3 << 12) | (p2 << 8) | (p0 << 4) | p1] = res;
          SORT4_PLAYS[(p3 << 12) | (p0 << 8) | (p1 << 4) | p2] = res;
          SORT4_PLAYS[(p3 << 12) | (p0 << 8) | (p2 << 4) | p1] = res;
        }
      }
    }
  }
}


void setRankConstants4()
{
  set4to3();
  set4sort();
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


unsigned holding4_to_holding3(const unsigned holding4)
{
  return 6561 * HOLDING4_TO_HOLDING3[holding4 >> 16] +
    HOLDING4_TO_HOLDING3[holding4 & 0xffff];
}


unsigned punchHolding4(
  const unsigned holding4,
  const Play& play)
{
  const unsigned playIndex =
    (play.leadPtr->getNumber() << 12) |
    (play.lhoPtr->getNumber() << 8) |
    (play.pardPtr->getNumber() << 4) |
     play.rhoPtr->getNumber();

  const array<unsigned char, 4>& sorted = SORT4_PLAYS[playIndex];

  unsigned punched = holding4;

  for (auto s: sorted)
  {
    if (s == 0)
    {
      // Once we reach a void, all others are voids too.
      break;
    }
    else
    {
      // Punch out the highest numbers first.
      // As 0 is a void, 1 is the first real card which occupies
      // the two lowest bits.
      punched = ((punched & HOLDING4_MASK_HIGH[s]) >> 2) |
        (punched & HOLDING4_MASK_LOW[s]);
    }
  }

  return punched;
}


unsigned uncanonicalTrinary(
  const unsigned holding4,
  const Play& play)
{
  return holding4_to_holding3(punchHolding4(holding4, play));
}


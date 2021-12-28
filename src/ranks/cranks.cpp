/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <vector>
#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>

#include "cranks.h"

#include "../plays/Play.h"
#include "../utils/table.h"

#include "../const.h"

#define BINARY4 16
#define BINARY8 256
#define TRINARY4 81
#define TRINARY8 6561
#define QUARTENARY4 256
#define QUARTENARY8 65536

using namespace std;


vector<unsigned> HOLDING4_TO_HOLDING3;
vector<unsigned> HOLDING4_TO_HOLDING2;
// TODO Probably unneeded after all
vector<unsigned> HOLDING3_TO_HOLDING4;

vector<array<unsigned char, 4>> SORT4_PLAYS;

vector<unsigned char> HOLDING4_TOP;

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

vector<unsigned> NORTH_REPEATS;
vector<unsigned> SOUTH_REPEATS;
vector<unsigned> OPPS_REPEATS;

vector<unsigned> OPPS_REPEATS_ROTATE;

// This is used to rotate a holding4.  We exploit that North and South
// are coded as 0 and 1.  The vector is indexed by the number of cards
// to get the right xor vector.

const vector<unsigned> HOLDING4_ROTATE =
{
  0x00000000, //  0
  0x00000001, //  1
  0x00000005, //  2
  0x00000015, //  3
  0x00000055, //  4
  0x00000155, //  5
  0x00000555, //  6
  0x00001555, //  7
  0x00005555, //  8
  0x00015555, //  9
  0x00055555, // 10
  0x00155555, // 11
  0x00555555, // 12
  0x01555555, // 13
  0x05555555, // 14
  0x15555555  // 15
};

// This is used to set SIDE_NONE's to unused high bits in a holding4.
// Otherwise 00 will look like SIDE_NORTH which caused holding4_to_rotate
// not to detect the highest card correctly.

const vector<unsigned> HOLDING4_NONE_HIGH =
{
  0xaaaaaaaa, //  0
  0xaaaaaaa8, //  1
  0xaaaaaaa0, //  2
  0xaaaaaa80, //  3
  0xaaaaaa00, //  4
  0xaaaaa800, //  5
  0xaaaaa000, //  6
  0xaaaa8000, //  7
  0xaaaa0000, //  8
  0xaaa80000, //  9
  0xaaa00000, // 10
  0xaa800000, // 11
  0xaa000000, // 12
  0xa8000000, // 13
  0xa0000000, // 14
  0x80000000  // 15
};

unsigned char update_has_top(
  const unsigned char has_top,
  const unsigned char trit);

void set4to32();

void set4sort();

void setRepeatsTable();
void setRepeats();

unsigned holding4_to_holding3(const unsigned holding4);
unsigned holding4_to_holding2(const unsigned holding4);
unsigned holding3_to_holding4(const unsigned holding4);

void holding4_to_both(
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2);

unsigned punchHolding4(
  const unsigned holding4,
  const Play& play);

bool holding4_to_rotate(
  const unsigned holding4,
   const unsigned cardsLeft);


unsigned char update_has_top(
  const unsigned char has_top,
  const unsigned char trit)
{
  if (has_top == SIDE_NORTH || has_top == SIDE_SOUTH)
    return has_top;
  else
    return trit;
}


void set4to32()
{
  // See comment of HOLDING4_ROTATE above.
  // Actually this part would also work if they were swapped,
  // but they have to share the same bit.
  assert(SIDE_NORTH == 0 && SIDE_SOUTH == 1);

  // Lookup of 8 cards into trit format.  There are 4^8 = 65536 entries
  // of holding4, 3^8 = 6561 possible values of 8-trit sequences,
  // and 2^8 = 256 possible values of 8-bit sequences.

  // First make temporary tables with the square root of the numbers.
  vector<unsigned> h3_to_h4_partial(TRINARY4);
  vector<unsigned> h3_to_h4_partial_shadow(TRINARY4);
  vector<unsigned> h3_to_h2_partial(TRINARY4);
  vector<unsigned char> h3_top_partial(TRINARY4);

  for (unsigned c0 = 0; c0 < 3; c0++)
  {
    const unsigned b0 = (c0 == SIDE_OPPS ? 1 : 0);
    for (unsigned c1 = 0; c1 < 3; c1++)
    {
      const unsigned b1 = (c1 == SIDE_OPPS ? 1 : 0);
      for (unsigned c2 = 0; c2 < 3; c2++)
      {
        const unsigned b2 = (c2 == SIDE_OPPS ? 1 : 0);
        for (unsigned c3 = 0; c3 < 3; c3++)
        {
          const unsigned b3 = (c3 == SIDE_OPPS ? 1 : 0);
          const unsigned h2 = (b0 << 3) | (b1 << 2) | (b2 << 1) | b3;
          const unsigned h3 = 27*c0 + 9*c1 + 3*c2 + c3;
          const unsigned h4 = (c0 << 6) | (c1 << 4) | (c2 << 2) | c3;

          // In a somewhat ugly hack, we also make a shadow holding4
          // in which opponents are represented by 3, not 2.
          // This is useful when we rotate a holding4, as North and
          // South are mirrors (00 and 01), but the opponents (10)
          // are "rotated" into 11.  It's either all of them or none.
          const unsigned c0s = (c0 == 2 ? 3 : c0);
          const unsigned c1s = (c1 == 2 ? 3 : c1);
          const unsigned c2s = (c2 == 2 ? 3 : c2);
          const unsigned c3s = (c3 == 2 ? 3 : c3);
          const unsigned h4s = (c0s << 6) | (c1s << 4) | (c2s << 2) | c3s;

assert(h3 < h3_to_h4_partial.size());
assert(h3 < h3_to_h2_partial.size());
assert(h4s < 256);

          h3_to_h4_partial[h3] = h4;
          h3_to_h2_partial[h3] = h2;

          h3_to_h4_partial_shadow[h3] = h4s;

          unsigned char has_top = SIDE_OPPS;
          has_top = update_has_top(has_top, static_cast<unsigned char>(c0));
          has_top = update_has_top(has_top, static_cast<unsigned char>(c1));
          has_top = update_has_top(has_top, static_cast<unsigned char>(c2));
          has_top = update_has_top(has_top, static_cast<unsigned char>(c3));

          h3_top_partial[h3] = has_top; 
// cout << "h3 " << h3 << 
  // ", " << +c0 << " " << +c1 << " " << +c2 << " " << +c3 << 
// ": has_top " << +has_top << endl;
        }
      }
    }
  }

  // Then make the big tables.
  HOLDING4_TO_HOLDING3.resize(QUARTENARY8);
  HOLDING4_TO_HOLDING2.resize(QUARTENARY8);
  HOLDING3_TO_HOLDING4.resize(TRINARY8);
  HOLDING4_TOP.resize(QUARTENARY8);

  for (unsigned p0 = 0; p0 < TRINARY4; p0++)
  {
    for (unsigned p1 = 0; p1 < TRINARY4; p1++)
    {
assert(p0 < h3_to_h2_partial.size());
assert(p1 < h3_to_h2_partial.size());
assert(p0 < h3_to_h4_partial.size());
assert(p1 < h3_to_h4_partial.size());

      const unsigned h3 = TRINARY4*p0 + p1;
      const unsigned h2 =
        (h3_to_h2_partial[p0] << 4) | h3_to_h2_partial[p1];
      const unsigned h4 = 
        (h3_to_h4_partial[p0] << 8) | h3_to_h4_partial[p1];
      const unsigned h4s = 
        (h3_to_h4_partial_shadow[p0] << 8) | h3_to_h4_partial_shadow[p1];

assert(h4 < HOLDING4_TO_HOLDING2.size());
assert(h4 < HOLDING4_TO_HOLDING3.size());
assert(h4s < HOLDING4_TO_HOLDING2.size());
assert(h4s < HOLDING4_TO_HOLDING3.size());

      HOLDING4_TO_HOLDING2[h4] = h2;
      HOLDING4_TO_HOLDING2[h4s] = h2;

      HOLDING4_TO_HOLDING3[h4] = h3;
      HOLDING4_TO_HOLDING3[h4s] = h3;

      HOLDING3_TO_HOLDING4[h3] = h4;

      const unsigned char top = h3_top_partial[p0];
      if (top == SIDE_NORTH || top == SIDE_SOUTH)
      {
        HOLDING4_TOP[h4] = top;
        HOLDING4_TOP[h4s] = top;
      }
      else
      {
        HOLDING4_TOP[h4] = h3_top_partial[p1];
        HOLDING4_TOP[h4s] = h3_top_partial[p1];
      }

// cout << "p0 " << p0 << " p1 " << p1 << ": h4 " << h4 <<
  // ", top0 " << +h3_top_partial[p0] << " top 1 " <<
  // +h3_top_partial[p1] <<
  // ", h4top " << +HOLDING4_TOP[h4] << "\n";
    }
  }
}


void set4sort()
{
  // A given 16-bit input is mapped onto an array of four sorted plays.
  // The input consists of four groups of four bits each.
  // Voids (0) may repeat, but others may not.

  SORT4_PLAYS.resize(QUARTENARY8);
  SORT4_PLAYS[0] = {0, 0, 0, 0};

  // If the cards were all by real absolute number, there would be
  // no repetitions other than 0 (discards).  But E-W will seem to play 
  // the same card if they play the same rank, so this is an efficient
  // place to disentangle them.  For example, if the plays are
  // N 4, E 3, S 0, W 3, then the second E-W card should have the
  // absolute number 2 and not 3.

  for (unsigned p0 = 1; p0 < 16; p0++)
  {
    for (unsigned p1 = 0; p1 <= p0; p1++)
    {
      const unsigned p1adj = (p1 == p0 ? p1-1 : p1);

      for (unsigned p2 = 0; p2 <= p1adj; p2++)
      {
        const unsigned p2adj = (p2 == p1 && p2 != 0 ? p2-1 : p2);

        for (unsigned p3 = 0; p3 <= p2adj; p3++)
        {
          const unsigned p3adj = (p3 == p2 && p3 != 0 ? p3-1 : p3);

          const array<unsigned char, 4> res = 
          { 
            static_cast<unsigned char>(p0), 
            static_cast<unsigned char>(p1adj),
            static_cast<unsigned char>(p2adj),
            static_cast<unsigned char>(p3adj)
          };

  assert(((p0 << 12) | (p1 << 8) | (p2 << 4) | p3) < 65536);
  assert(((p0 << 12) | (p1 << 8) | (p3 << 4) | p2) < 65536);
  assert(((p0 << 12) | (p2 << 8) | (p1 << 4) | p3) < 65536);
  assert(((p0 << 12) | (p2 << 8) | (p3 << 4) | p1) < 65536);
  assert(((p0 << 12) | (p3 << 8) | (p1 << 4) | p2) < 65536);
  assert(((p0 << 12) | (p3 << 8) | (p2 << 4) | p1) < 65536);

  assert(((p1 << 12) | (p0 << 8) | (p2 << 4) | p3) < 65536);
  assert(((p1 << 12) | (p0 << 8) | (p3 << 4) | p2) < 65536);
  assert(((p1 << 12) | (p2 << 8) | (p0 << 4) | p3) < 65536);
  assert(((p1 << 12) | (p2 << 8) | (p3 << 4) | p0) < 65536);
  assert(((p1 << 12) | (p3 << 8) | (p0 << 4) | p2) < 65536);
  assert(((p1 << 12) | (p3 << 8) | (p2 << 4) | p0) < 65536);

  assert(((p2 << 12) | (p1 << 8) | (p0 << 4) | p3) < 65536);
  assert(((p2 << 12) | (p1 << 8) | (p3 << 4) | p0) < 65536);
  assert(((p2 << 12) | (p0 << 8) | (p1 << 4) | p3) < 65536);
  assert(((p2 << 12) | (p0 << 8) | (p3 << 4) | p1) < 65536);
  assert(((p2 << 12) | (p3 << 8) | (p1 << 4) | p0) < 65536);
  assert(((p2 << 12) | (p3 << 8) | (p0 << 4) | p1) < 65536);

  assert(((p3 << 12) | (p1 << 8) | (p2 << 4) | p0) < 65536);
  assert(((p3 << 12) | (p1 << 8) | (p0 << 4) | p2) < 65536);
  assert(((p3 << 12) | (p2 << 8) | (p1 << 4) | p0) < 65536);
  assert(((p3 << 12) | (p2 << 8) | (p0 << 4) | p1) < 65536);
  assert(((p3 << 12) | (p0 << 8) | (p1 << 4) | p2) < 65536);
  assert(((p3 << 12) | (p0 << 8) | (p2 << 4) | p1) < 65536);


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


void setRepeatsTable(
  const unsigned value,
  vector<unsigned>& repeats)
{
  unsigned r = 0;
  for (unsigned n = 0; n < repeats.size(); n++)
  {
    repeats[n] = r;
    r = (r << 2) | value;
  }
}


void setRepeats()
{
  NORTH_REPEATS.resize(16);
  SOUTH_REPEATS.resize(16);
  OPPS_REPEATS.resize(16);

  setRepeatsTable(SIDE_NORTH, NORTH_REPEATS);
  setRepeatsTable(SIDE_SOUTH, SOUTH_REPEATS);
  setRepeatsTable(SIDE_OPPS, OPPS_REPEATS);

  OPPS_REPEATS_ROTATE.resize(16);

  setRepeatsTable(SIDE_NONE, OPPS_REPEATS_ROTATE);
}


void setRankConstants4()
{
  set4to32();
  set4sort();
  setRepeats();
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
assert((holding4 >> 16) < HOLDING4_TO_HOLDING3.size());
assert((holding4 & 0xffff) < HOLDING4_TO_HOLDING3.size());

/*
cout << "h4to3: " << holding4 << ", " <<
  (holding4 >> 16) << ", " <<
  (holding4 & 0xfff) << ", " <<
  HOLDING4_TO_HOLDING3[holding4 >> 16] << ", " <<
  HOLDING4_TO_HOLDING3[holding4 & 0xffff] << "\n";
  */

  return 6561 * HOLDING4_TO_HOLDING3[holding4 >> 16] +
    HOLDING4_TO_HOLDING3[holding4 & 0xffff];
}


unsigned holding4_to_holding2(const unsigned holding4)
{
assert((holding4 >> 16) < HOLDING4_TO_HOLDING2.size());
assert((holding4 & 0xffff) < HOLDING4_TO_HOLDING2.size());

/*
cout << "h4to2: " << holding4 << ", " <<
  (holding4 >> 16) << ", " <<
  (holding4 & 0xfff) << ", " <<
  HOLDING4_TO_HOLDING2[holding4 >> 16] << ", " <<
  HOLDING4_TO_HOLDING2[holding4 & 0xffff] << "\n";
  */

  return 256 * HOLDING4_TO_HOLDING2[holding4 >> 16] +
    HOLDING4_TO_HOLDING2[holding4 & 0xffff];
}


unsigned holding3_to_holding4(const unsigned holding3)
{
  const unsigned h3hi = holding3 / TRINARY8;
  const unsigned h3lo = holding3 % TRINARY8;

  return (QUARTENARY8 * HOLDING3_TO_HOLDING4[h3hi]) |
    HOLDING3_TO_HOLDING4[h3lo];
}


void holding4_to_both(
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2)
{
  holding3 = holding4_to_holding3(holding4);
  holding2 = holding4_to_holding2(holding4);
}


unsigned punchHolding4(
  const unsigned holding4,
  const Play& play)
{
  const unsigned playIndex =
    (play.leadPtr->getAbsNumber() << 12) |
    (play.lhoPtr->getAbsNumber() << 8) |
    (play.pardPtr->getAbsNumber() << 4) |
     play.rhoPtr->getAbsNumber();

assert(playIndex < SORT4_PLAYS.size());

  const array<unsigned char, 4>& sorted = SORT4_PLAYS[playIndex];

/*
cout << "punch h4 " << holding4 << "\n";
cout << "play " << play.strLine() << endl;
cout << "pindex " << playIndex << "\n";
cout << "lead " << +play.leadPtr->getAbsNumber() << endl;
cout << "lho  " << +play.lhoPtr->getAbsNumber() << endl;
cout << "pard " << +play.pardPtr->getAbsNumber() << endl;
cout << "rho  " << +play.rhoPtr->getAbsNumber() << endl;
for (unsigned i = 0; i < 4; i++)
  cout << i << ": " << +sorted[i] << "\n";
cout << "Starting on lookup" << endl;
*/

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

assert(s < HOLDING4_MASK_HIGH.size());
assert(s < HOLDING4_MASK_LOW.size());

      punched = ((punched & HOLDING4_MASK_HIGH[s]) >> 2) |
        (punched & HOLDING4_MASK_LOW[s]);

// cout << "s: " << +s << ", punched now " << punched << endl;
    }
  }

  return punched;
}


void orientedBoth(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2)
{
  const unsigned holding4rot = (rotateFlag ?
    (holding4 ^ HOLDING4_ROTATE[cards]) : holding4);

// cout << "orientedBoth: h4 in " << holding4 << ", rot " << holding4rot <<endl;

  holding4_to_both(holding4rot, holding3, holding2);
}


bool holding4_to_rotate(
  const unsigned holding4,
  const unsigned cardsLeft)
{
  const unsigned holding4padded = holding4 | HOLDING4_NONE_HIGH[cardsLeft];

// cout << "padded " << holding4padded << endl;
// cout << "    hi " << (holding4padded >> 16) << endl;
// cout << "    lo " << (holding4padded & 0xffff) << endl;
// cout << "    hi " << +HOLDING4_TOP[(holding4padded >> 16)] << endl;
// cout << "    lo " << +HOLDING4_TOP[(holding4padded & 0xffff)] << endl;

  if (HOLDING4_TOP[holding4padded >> 16] == SIDE_SOUTH)
    return true;
  else
    return (HOLDING4_TOP[holding4padded & 0xffff] == SIDE_SOUTH);
}


void orientedTrinary(
  const unsigned cards,
  const unsigned holding4,
  const Play& play,
  unsigned& holding3,
  bool& rotateFlag)
{
  unsigned holding4punched = punchHolding4(holding4, play);

  // If both North and South are void, that stays as a North winner.
  rotateFlag = holding4_to_rotate(holding4punched, play.cardsLeft);

// cout << "orientedTrinary: holding4punched " << holding4punched << endl;
// cout << "holding4 was " << holding4 << endl;
// cout << "rotate " << rotateFlag << endl;

  // If rotateFlag is set, the trinary holding is rotated,
  // so North and South are swapped.
  if (rotateFlag)
    holding4punched ^= HOLDING4_ROTATE[cards];

  holding3 = holding4_to_holding3(holding4punched);
}


unsigned uncanonicalBinary(const unsigned holding4)
{
  return holding4_to_holding2(holding4);
}


void uncanonicalBoth(
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2)
{
  holding4_to_both(holding4, holding3, holding2);
}


unsigned minimalizeRanked(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned oppsCount,
  const unsigned northCount,
  const unsigned southCount,
  const unsigned& holding4)
{
  // Resort the low cards in the holding4 order opps, North, South
  // (top to bottom).

  const unsigned lows = oppsCount + southCount + northCount;
  assert(lows >= 1);

  if (rotateFlag)
  {
    // The returned value will have SIDE_NONE (3) and not SIDE_OPPS
    // for the opponents, but that's OK for the table lookups.
    // The key is that they should all be 3's (or all 2's), hence
    // OPPS_REPEAT_ROTATE.
    const unsigned holding4rot = holding4 ^ HOLDING4_ROTATE[cards];

/*
cout << "rotate h4 to " << holding4rot << endl;
cout << "h4mask " << (holding4rot & HOLDING4_MASK_HIGH[lows]) << endl;
cout << "opprot " << (OPPS_REPEATS_ROTATE[oppsCount] << 2*(northCount + southCount)) << endl;
cout << "south  " << (SOUTH_REPEATS[northCount] << 2*southCount) << endl;
cout << "north  " << NORTH_REPEATS[southCount] << endl;
*/

    const unsigned holding4min = (holding4rot & HOLDING4_MASK_HIGH[lows]) |
      ((OPPS_REPEATS_ROTATE[oppsCount] << 2*(northCount + southCount)) |
       (NORTH_REPEATS[southCount] << 2*northCount) |
       SOUTH_REPEATS[northCount]);

       // orig
       // (SOUTH_REPEATS[northCount] << 2*southCount) |
       // NORTH_REPEATS[southCount]);

    return holding4min;
  }
  else
  {
    return (holding4 & HOLDING4_MASK_HIGH[lows]) |
      ((OPPS_REPEATS[oppsCount] << 2*(northCount + southCount)) |
       (NORTH_REPEATS[northCount] << 2*southCount) |
       SOUTH_REPEATS[southCount]);

       // orig
       // (SOUTH_REPEATS[southCount] << 2*northCount) |
       // NORTH_REPEATS[northCount]);
  }
}
  

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <vector>
#include <array>
#include <algorithm>
#include <cassert>

#include "cranked.h"

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


/* -------------------------------------------------------- *
 |                                                          |
 | Helper vectors for initialization                        |
 |                                                          |
 * -------------------------------------------------------- */

// This and the next vector are used to punch out a card from a
// holding4 in the position indicated by the index (play).  
// For example, if we are punching out number 3 (which is in bits 4-5), 
// then the low mask selects the two lower cards (in bits 0-3) and
// the high mask selects everything from number 4 (bit 6) upwards.

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

// This is used to rotate a holding4.  We exploit that North and South
// are coded as 0 and 1.  The vector is indexed by the number of cards
// to get the right XOR vector.  This does alias 10 (SIDE_OPPS) into
// 11 (which is not a holding in its own right and therefore becomes
// a shadow holding).

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

// This is used to set SIDE_NONE's in unused high bits in a holding4.
// Otherwise 00 will look like SIDE_NORTH which will cuause 
// holding4isRotated not to detect the highest card correctly.

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

vector<unsigned> NORTH_REPEATS =
{
  0x00000000, //  0
  0x00000000, //  1
  0x00000000, //  2
  0x00000000, //  3
  0x00000000, //  4
  0x00000000, //  5
  0x00000000, //  6
  0x00000000, //  7
  0x00000000, //  8
  0x00000000, //  9
  0x00000000, // 10
  0x00000000, // 11
  0x00000000, // 12
  0x00000000, // 13
  0x00000000, // 14
  0x00000000  // 15
};

vector<unsigned> SOUTH_REPEATS =
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

vector<unsigned> OPPS_REPEATS =
{
  0x00000000, //  0
  0x00000002, //  1
  0x0000000a, //  2
  0x0000002a, //  3
  0x000000aa, //  4
  0x000002aa, //  5
  0x00000aaa, //  6
  0x00002aaa, //  7
  0x0000aaaa, //  8
  0x0002aaaa, //  9
  0x000aaaaa, // 10
  0x002aaaaa, // 11
  0x00aaaaaa, // 12
  0x02aaaaaa, // 13
  0x0aaaaaaa, // 14
  0x2aaaaaaa  // 15
};

vector<unsigned> NONE_REPEATS =
{
  0x00000000, //  0
  0x00000003, //  1
  0x0000000f, //  2
  0x0000003f, //  3
  0x000000ff, //  4
  0x000003ff, //  5
  0x00000fff, //  6
  0x00003fff, //  7
  0x0000ffff, //  8
  0x0003ffff, //  9
  0x000fffff, // 10
  0x003fffff, // 11
  0x00ffffff, // 12
  0x03ffffff, // 13
  0x0fffffff, // 14
  0x3fffffff  // 15
};

/************************************************************
 *                                                          *
 * Actual vectors being initialized for later lookups       *
 *                                                          *
 ************************************************************/

// 8-card maps including shadow holdings of holding4 (see below).
// 65536 entries (2 bits per card).

vector<unsigned> HOLDING4_TO_HOLDING3;
vector<unsigned> HOLDING4_TO_HOLDING2;

// 4-play map into sorted array.
// 65536 entries (4 bits per play).

vector<array<unsigned char, 4>> SORT4_PLAYS;

// 8-card map into a 2-bit (00, 01, 02) indication of top card.
// 65536 entries  (2 bits per card including shadows).

vector<unsigned char> HOLDING4_TOP;



/* -------------------------------------------------------- *
 |                                                          |
 | Helper functions for initialization                      |
 |                                                          |
 * -------------------------------------------------------- */

unsigned holding4partialShadow(
  const unsigned c0,
  const unsigned c1,
  const unsigned c2,
  const unsigned c3);

unsigned char updateTop(
  const unsigned char top,
  const unsigned trit);

unsigned char findTop(
  const unsigned c0,
  const unsigned c1,
  const unsigned c2,
  const unsigned c3);

void set4holdings();

void enterSorted(
  const unsigned play0,
  const unsigned play1,
  const unsigned play2,
  const unsigned play3,
  const array<unsigned char, 4>& result);

void set4sorts();


/* -------------------------------------------------------- *
 |                                                          |
 | Helper functions for run-time calculations               |
 |                                                          |
 * -------------------------------------------------------- */

unsigned holding4to3(const unsigned holding4);

unsigned holding4to2(const unsigned holding4);

void holding4toBoth(
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2);

unsigned holding4punch(
  const unsigned holding4,
  const Play& play);

bool holding4isRotated(
  const unsigned holding4,
   const unsigned cardsLeft);



/************************************************************
 *                                                          *
 * Helper functions for initialization                      *
 *                                                          *
 ************************************************************/

unsigned holding4partialShadow(
  const unsigned c0,
  const unsigned c1,
  const unsigned c2,
  const unsigned c3)
{
  // Makes a partial shadow holding -- see comment below.
  const unsigned c0s = (c0 == 2 ? 3 : c0);
  const unsigned c1s = (c1 == 2 ? 3 : c1);
  const unsigned c2s = (c2 == 2 ? 3 : c2);
  const unsigned c3s = (c3 == 2 ? 3 : c3);

  return ((c0s << 6) | (c1s << 4) | (c2s << 2) | c3s);
}


unsigned char updateTop(
  const unsigned char top,
  const unsigned trit)
{
  // This function helps to find the top card of a holding4.

  if (top == SIDE_NORTH || top == SIDE_SOUTH)
    return top;
  else
    return static_cast<unsigned char>(trit);
}


unsigned char findTop(
  const unsigned c0,
  const unsigned c1,
  const unsigned c2,
  const unsigned c3)
{
  unsigned char top = SIDE_OPPS;
  top = updateTop(top, c0);
  top = updateTop(top, c1);
  top = updateTop(top, c2);
  top = updateTop(top, c3);
  return top;
}


void set4holdings()
{
  // We exploit in many places that North and South are coded as 0 and 1.
  // This is either a clever optimization or an ugly hack.
  assert(SIDE_NORTH == 0 && SIDE_SOUTH == 1);

  // Lookup of 8 cards.  There are:
  // 4^8 = QUATERNARY8 entries of 8-quaternary sequences
  // 3^8 = TRINARY8 possible values of 8-trit sequences, and
  // 2^8 = BINARY8 possible values of 8-bit sequences.

  // First make temporary tables with the square root of the numbers,
  // so 4 cards each.

  // A recular map from a holding3 to a holding4.
  vector<unsigned> h3to4partial(TRINARY4);

  // This is like h3to4partial, but with 11 instead of 10 for SIDE_OPPS
  // in the holding4. The reason is that when a holding4 is rotated, 
  // 10 becomes 11.  We catch this by storing corresponding entries in 
  // the tables even though these don't occur in nature.
  vector<unsigned> h3to4partialShadow(TRINARY4);

  vector<unsigned> h3to2partial(TRINARY4);

  // h3topPartial records whether North, South or nobody (yet) has
  // the top card among the cards seen.
  vector<unsigned char> h3topPartial(TRINARY4);

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

          // The partial holding2 consists of the four bits.
          const unsigned h2 = (b0 << 3) | (b1 << 2) | (b2 << 1) | b3;

          // The partial holding3 consists of the four trits.
          const unsigned h3 = 27*c0 + 9*c1 + 3*c2 + c3;

          // The partial holding4 is a more dispersed version of h3.
          const unsigned h4 = (c0 << 6) | (c1 << 4) | (c2 << 2) | c3;

          h3to4partial[h3] = h4;

          h3to2partial[h3] = h2;

          h3to4partialShadow[h3] = holding4partialShadow(c0, c1, c2, c3);

          h3topPartial[h3] = findTop(c0, c1, c2, c3);
        }
      }
    }
  }

  // Then make the 8-card tables.
  HOLDING4_TO_HOLDING3.resize(QUARTENARY8);
  HOLDING4_TO_HOLDING2.resize(QUARTENARY8);
  HOLDING4_TOP.resize(QUARTENARY8);

  for (unsigned p0 = 0; p0 < TRINARY4; p0++)
  {
    for (unsigned p1 = 0; p1 < TRINARY4; p1++)
    {
      const unsigned h2 = (h3to2partial[p0] << 4) | h3to2partial[p1];
      const unsigned h3 = TRINARY4*p0 + p1;
      const unsigned h4 = (h3to4partial[p0] << 8) | h3to4partial[p1];
      const unsigned h4shadow = 
        (h3to4partialShadow[p0] << 8) | h3to4partialShadow[p1];

      HOLDING4_TO_HOLDING2[h4] = h2;
      HOLDING4_TO_HOLDING2[h4shadow] = h2;

      HOLDING4_TO_HOLDING3[h4] = h3;
      HOLDING4_TO_HOLDING3[h4shadow] = h3;

      const unsigned char top = h3topPartial[p0];
      if (top == SIDE_NORTH || top == SIDE_SOUTH)
      {
        HOLDING4_TOP[h4] = top;
        HOLDING4_TOP[h4shadow] = top;
      }
      else
      {
        HOLDING4_TOP[h4] = h3topPartial[p1];
        HOLDING4_TOP[h4shadow] = h3topPartial[p1];
      }

    }
  }
}


void enterSorted(
  const unsigned play0,
  const unsigned play1,
  const unsigned play2,
  const unsigned play3,
  const array<unsigned char, 4>& result)
{
  SORT4_PLAYS[(play0 << 12) | (play1 << 8) | (play2 << 4) | play3] = 
    result;
}


void set4sorts()
{
  // A given 16-bit input is mapped onto an array of four sorted plays.
  // The input consists of four groups of four bits each.
  // Voids (0) may repeat.  Others may repeat once, as opponents
  // may play the same rank twice in a trick.

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

          enterSorted(p0, p1, p2, p3, res);
          enterSorted(p0, p1, p3, p2, res);
          enterSorted(p0, p2, p1, p3, res);
          enterSorted(p0, p2, p3, p1, res);
          enterSorted(p0, p3, p1, p2, res);
          enterSorted(p0, p3, p2, p1, res);

          enterSorted(p1, p0, p2, p3, res);
          enterSorted(p1, p0, p3, p2, res);
          enterSorted(p1, p2, p0, p3, res);
          enterSorted(p1, p2, p3, p0, res);
          enterSorted(p1, p3, p0, p2, res);
          enterSorted(p1, p3, p2, p0, res);

          enterSorted(p2, p1, p0, p3, res);
          enterSorted(p2, p1, p3, p0, res);
          enterSorted(p2, p0, p1, p3, res);
          enterSorted(p2, p0, p3, p1, res);
          enterSorted(p2, p3, p1, p0, res);
          enterSorted(p2, p3, p0, p1, res);

          enterSorted(p3, p1, p2, p0, res);
          enterSorted(p3, p1, p0, p2, res);
          enterSorted(p3, p2, p1, p0, res);
          enterSorted(p3, p2, p0, p1, res);
          enterSorted(p3, p0, p1, p2, res);
          enterSorted(p3, p0, p2, p1, res);
        }
      }
    }
  }
}


void setRankedConstants()
{
  set4holdings();
  set4sorts();
}


/************************************************************
 *                                                          *
 * Helper functions for run-time calculations               *
 *                                                          *
 ************************************************************/

unsigned holding4to3(const unsigned holding4)
{
  return TRINARY8 * HOLDING4_TO_HOLDING3[holding4 >> 16] +
    HOLDING4_TO_HOLDING3[holding4 & 0xffff];
}


unsigned holding4to2(const unsigned holding4)
{
  return BINARY8 * HOLDING4_TO_HOLDING2[holding4 >> 16] +
    HOLDING4_TO_HOLDING2[holding4 & 0xffff];
}


void holding4toBoth(
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2)
{
  holding3 = holding4to3(holding4);
  holding2 = holding4to2(holding4);
}


#include <iostream>
unsigned holding4punch(
  const unsigned holding4,
  const Play& play)
{
  // Punch the 2-bit cards out of holding4, shrinking it in the process.
  // This is best done from above, so we look up the sorted cards
  // from the actual play index.

  const unsigned playIndex =
    (play.lead(true) << 12) |
    (play.lho(true) << 8) |
    (play.pard(true) << 4) |
     play.rho(true);

  unsigned punched = holding4;

  const array<unsigned char, 4>& sorted = SORT4_PLAYS[playIndex];
  for (auto s: sorted)
  {
    // Punch out cards in descending order.
    if (s == 0)
    {
      // Once we reach a void, all others are voids too.
      break;
    }
    else
    {
      // As 0 is a void, 1 is the first real card which occupies
      // the two lowest bits.
      punched = ((punched & HOLDING4_MASK_HIGH[s]) >> 2) |
        (punched & HOLDING4_MASK_LOW[s]);
    }
  }

  return punched;
}


bool holding4isRotated(
  const unsigned holding4,
  const unsigned cardsLeft)
{
  // Change the leading 00's to 10's so they don't look like
  // actual North cards.

  const unsigned holding4padded = 
    holding4 | HOLDING4_NONE_HIGH[cardsLeft];

  if (HOLDING4_TOP[holding4padded >> 16] == SIDE_SOUTH)
    return true;
  else
    return (HOLDING4_TOP[holding4padded & 0xffff] == SIDE_SOUTH);
}


/************************************************************
 *                                                          *
 * Interface functions                                      *
 *                                                          *
 ************************************************************/

void rankedTrinary(
  const unsigned cards,
  const unsigned holding4,
  const Play& play,
  unsigned& holding3,
  bool& rotateFlag)
{
  // Makes the plays and calculates holding3 including preset rotation.

  unsigned holding4punched = holding4punch(holding4, play);

  // If both North and South are void, that stays as a North winner.
  rotateFlag = holding4isRotated(holding4punched, play.cardsLeft);

  // If rotateFlag is set, the trinary holding is rotated,
  // so North and South are swapped.
  if (rotateFlag)
    holding4punched ^= HOLDING4_ROTATE[cards];

  holding3 = holding4to3(holding4punched);
}


void rankedBoth(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2)
{
  // Turns holding4 into holding3 and holding2.  Can rotate.

  const unsigned holding4rot = (rotateFlag ?
    (holding4 ^ HOLDING4_ROTATE[cards]) : holding4);

  holding4toBoth(holding4rot, holding3, holding2);
}


unsigned rankedMinimalize(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned oppsCount,
  const unsigned northCount,
  const unsigned southCount,
  const unsigned& holding4)
{
  // Redistributes the low cards in the holding4 order 
  // opps, North, South (top to bottom).

  const unsigned lows = oppsCount + southCount + northCount;
  assert(lows >= 1);

  if (rotateFlag)
  {
    // The returned value will have SIDE_NONE (3) and not SIDE_OPPS
    // for the opponents, but that's OK for the table lookups.
    // The key is that they should all be 3's (or all 2's), hence
    // NONE_REPEATS.
    const unsigned holding4rot = holding4 ^ HOLDING4_ROTATE[cards];

    return (holding4rot & HOLDING4_MASK_HIGH[lows]) |
      ((NONE_REPEATS[oppsCount] << 2*(northCount + southCount)) |
       (NORTH_REPEATS[southCount] << 2*northCount) |
       SOUTH_REPEATS[northCount]);
  }
  else
  {
    return (holding4 & HOLDING4_MASK_HIGH[lows]) |
      ((OPPS_REPEATS[oppsCount] << 2*(northCount + southCount)) |
       (NORTH_REPEATS[northCount] << 2*southCount) |
       SOUTH_REPEATS[southCount]);
  }
}
  

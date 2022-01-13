/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "DistMap.h"

#include "../utils/table.h"


DistMap::DistMap()
{
  DistMap::reset();
}


void DistMap::reset()
{
  full2reduced.clear();
  reduced2full.clear();
  rankSize = 0;
}


void DistMap::shrink(
  const unsigned maxFullRank,
  const unsigned maxReducedRank)
{
  rankSize = maxReducedRank + 1;
  full2reduced.resize(maxFullRank+1);
  reduced2full.resize(rankSize);
  opponents.counts.resize(rankSize);
}


void DistMap::setRanks(
  const unsigned cardsIn,
  const unsigned holding2)
{
  cards = cardsIn;
  // We go with a minimal representation of East-West in terms of ranks,
  // so the rank numbers will be smaller.  We keep a correspondence
  // back and forth with the outside world.
  full2reduced.resize(cards+1);
  reduced2full.resize(cards);
  opponents.counts.resize(cards);
  
  bool prev_is_NS = ((holding2 & 1) == PAIR_NS);

  // The full rank is the rank used in Combinations.
  // East-West might have ranks 1, 3 and 5, for example.
  unsigned nextFullRank = 1;
  unsigned maxFullRank = 1;

  // The reduced rank is used internally in Distribution and only
  // considers the East-West ranks which might be 0, 1 and 2.
  unsigned nextReducedRank = 0;
  unsigned maxReducedRank = 0;

  unsigned h = holding2;
  opponents.len = 0;

  // We could use a more compact rank format, but we want to have the
  // same rank numbers as in Combination.
  for (unsigned i = 0; i < cards; i++)
  {
    const unsigned c = h & 1;
    if (c == PAIR_NS)
    {
      if (! prev_is_NS)
      {
        nextFullRank++;
        nextReducedRank++;
      }

      prev_is_NS = true;
    }
    else
    {
      if (prev_is_NS)
        nextFullRank++;

      full2reduced[nextFullRank] = nextReducedRank;
      reduced2full[nextReducedRank] = nextFullRank;
      opponents.counts[nextReducedRank]++;
      opponents.len++;

      maxFullRank = nextFullRank;
      maxReducedRank = nextReducedRank;

      prev_is_NS = false;
    }
    h >>= 1;
  }

  // Shrink to fit.
  DistMap::shrink(maxFullRank, maxReducedRank);
}


DistID DistMap::getID() const
{
  // The canonical holding arises when EW have the lowest card and
  // when each NS rank consists of exactly 1 card.  So effectively
  // the canonical holding is a binary representation of a composition
  // of len cards into the particular rank profile.  Therefore the
  // number of such representations is the number of compositions.

  // The number of bits needed to represent the canonical holding,
  // i.e. the number of cards that both sides need, is.
  DistID res;
  res.cards = opponents.len + rankSize - 1;

  // This assumes that PAIR_EW is 1.
  assert(PAIR_EW == 1);

  res.holding = 0;
  for (unsigned r = rankSize; r-- > 0; )
  {
    // Make room for bits.
    // If we need e.g. 3 bits, they are going to be "011".
    const unsigned bits = opponents.counts[r] + 1;
    res.holding = (res.holding << bits) | ((1u << (bits-1)) - 1u);
  }

  return res;
}


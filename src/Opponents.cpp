#include <iostream>
#include <iomanip>
#include <sstream>
#include <codecvt>
#include <cassert>

#include "Opponents.h"

/*
 * This is a specialized class derived from Player.  Its members
 * only make sense (or are only used) for opponents, not for
 * North and South.
 */


void Opponents::setVoid()
{
  // A void among other possibilities.
  // TODO Does this even show up in the result, as maxPos isn't set?
  // Can we just do away with the method?
  minRank = 0;
  rankInfo[0].count = 1;
  rankInfo[0].ptr = nullptr;
}


void Opponents::setNames()
{
  // For the opponents we simplify more than for North-South.
  unsigned index = 0;
  unsigned rankPrev = numeric_limits<unsigned>::max();

  for (auto cit = cards.rbegin(); cit != cards.rend(); cit++)
  {
    const unsigned r = cit->getRank();
    if (r == rankPrev)
      continue;

    rankPrev = r;
    const unsigned count = rankInfo[r].count;
    if (count > 1)
    {
      // Turn <= ~ an eight into 'x'.
      const char rep = (r == minRank && minAbsCardNumber <= 6 ?
        'x' : GENERIC_NAMES[index]);

      rankInfo[r].names = string(count, rep);
      index++;
    }
    else if (count == 1)
      rankInfo[r].names = cit->getName();
  }
}


void Opponents::playRank(const unsigned rank)
{
  assert(rankInfo[rank].count > 0);
  rankInfo[rank].count--;
}


const deque<Card const *>& Opponents::getCards() const
{
  // Always only the ranks, not the complete set of cards.
  return ranksPtr;
}


Card const * Opponents::voidPtr() const
{
  return &voidCard;
}

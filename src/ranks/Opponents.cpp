#include <iostream>
#include <iomanip>
#include <sstream>
#include <codecvt>
#include <cassert>

#include "Opponents.h"
#include "../const.h"

/*
 * This is a specialized class derived from Player.  Its members
 * only make sense (or are only used) for opponents, not for
 * North and South.
 */


void Opponents::setVoid()
{
  // A void among other possibilities.
  minRank = 0;
  rankInfo[0].count = 1;
}


void Opponents::setNames()
{
  // For the opponents we simplify more than for North-South.
  unsigned index = 0;
  unsigned char rankPrev = numeric_limits<unsigned char>::max();
  char rep = '?';

  for (auto cit = cards.rbegin(); cit != cards.rend(); cit++)
  {
    const unsigned char r = cit->getRank();
    if (r == rankPrev)
    {
      cit->updateName(static_cast<unsigned char>(rep));
      continue;
    }

    rankPrev = r;
    const unsigned count = rankInfo[r].count;
    if (count > 1)
    {
      // Turn <= ~ an eight into 'x'.
      rep = (r == minRank && minAbsCardNumber <= 7 ?
        'x' : GENERIC_NAMES[index]);

      rankInfo[r].names = string(count, rep);
      cit->updateName(static_cast<unsigned char>(rep));
      index++;
    }
    else if (count == 1)
      rankInfo[r].names = static_cast<char>(cit->getName());
  }
}


void Opponents::playRank(const unsigned char rank)
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

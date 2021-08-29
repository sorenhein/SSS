#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Declarer.h"
#include "Opponents.h"

/*
 * This is a specialized class derived from Player.  Its members
 * only make sense (or are only used) for North and South, not for
 * opponents.
 */


void Declarer::setVoid()
{
  if (len > 0)
    return;

  // A real void.
  minRank = 0;
  maxRank = 0;
  rankInfo[0].count = 1;

  // For North-South, a void goes in cards, cardsPtr and ranksPtr.

  cards.emplace_back(Card());
  cardsPtr.push_back(&cards.front());
  ranksPtr.push_back(&cards.front());
}


void Declarer::setSingleRank()
{
  singleRank = (len >= 1 && ranksPtr.size() == 1);
}


void Declarer::setNames()
{
  // As setVoid may create an extra "card", this must be done first.
  Declarer::setVoid();

  for (auto cit = cards.rbegin(); cit != cards.rend(); cit++)
    rankInfo[cit->getRank()].names += 
      static_cast<char>(cit->getName());
}


void Declarer::fixDepths()
{
  // In Ranks we loop through the cards from the bottom up, so they
  // are numbered from 0 up.  Also, the depths within a rank are
  // numbered from 0 up.  The depths are not used that much, but
  // when we print the card in Card, it is useful for depth == 0 to
  // be the highest and not the lowest card within a rank.  In this
  // way we can easily decorate the names in a useful way.
  // We cannot avoid the problem by looping the other way in Ranks --
  // then we'd have the problem with the ranks.
  // So we fix (flip) the ranks from from e.g. 0, 1, 2 to 2, 1, 0
  // in this inelegant piece of post-processing.

  const unsigned char csize = static_cast<unsigned char>(cards.size());
  for (unsigned char cno = 0; cno < csize; cno++)
  {
    if (cards[cno].getDepth() == 1)
    {
      // Find the index past the current rank (may be beyond the end).
      unsigned char dno = cno+1;
      while (dno < csize && cards[dno].getDepth() > 0)
        dno++;

      unsigned char maxDepth = dno - cno;

      // Flip the ranks, starting from the zero depth.
      for (cno--; cno < dno; cno++)
        cards[cno].flipDepth(maxDepth);
    }
  }
}


bool Declarer::playRank(
  const unsigned char rank,
  const Declarer& partner,
  const unsigned char maxGlobalRank)
{
  assert(rankInfo[rank].count > 0);
  rankInfo[rank].count--;

  // This is the collapse flag where a rank disappears meaningfully.
  return (rank > 1 &&
    rank != maxGlobalRank &&
    ! Declarer::hasRank(rank) &&
    ! partner.hasRank(rank));
}


void Declarer::finish()
{
  Declarer::setSingleRank();
  Declarer::fixDepths();
}


bool Declarer::greater(
  const Declarer& p2,
  const Opponents& opps) const
{
  // There may be rank collapses from played EW cards.
  unsigned run1 = 0;
  unsigned run2 = 0;
  for (unsigned char r = max(maxRank, p2.maxRank); ; r -= 2)
  {
    run1 += rankInfo[r].count;
    run2 += p2.rankInfo[r].count;

    if (r > 2 && ! opps.hasRank(r-1))
      // EW collapse
      continue;  
    else if (run1 > run2)
      return true;
    else if (run1 < run2)
      return false;
    else if (r <= 2)
      // Nothing else happens, so equality
      return true; 
    
    run1 = 0;
    run2 = 0;
  }
}


const Card& Declarer::top() const
{
  assert(! cards.empty());
  return cards.back();
}


const deque<Card const *>& Declarer::getCards(const bool fullFlag) const
{
  if (fullFlag)
    return cardsPtr;
  else
    return ranksPtr;
}


bool Declarer::isSingleRanked() const
{
  return singleRank;
}


bool Declarer::needsUpshift(const Card& card) const
{
  // We need to shift up all ranks when
  // 1. The rank we're splitting is our lowest rank, and
  // 2. We have to split the rank.

  // The lowest winner must be higher than an opposing card.
  const unsigned rank = card.getRank();
  assert(rank > 1);

  // There is no need to shift up if there is a lower rank.
  if (rank > minRank)
    return false;

  assert(rank < rankInfo.size());
  const unsigned ourCount = rankInfo[rank].count;

  const unsigned depth = card.getDepth();
  assert(depth+1 <= ourCount);

  // Check whether we have to split the rank.
  return (depth+1 < ourCount);
}


bool Declarer::minimize(const Card& card)
{
  // When we need to split ranks, this is where to do it.
  // We may also just shift down entire ranks to our lowest one.

  const unsigned char rank = card.getRank();
  const unsigned char depth = card.getDepth();
  const unsigned char ourCount = rankInfo[rank].count;

  if (depth+1 == ourCount)
    return false;

  assert(rank >= 3);
  unsigned char rest = ourCount - (depth+1);
  rankInfo[rank].count -= rest;

  for (unsigned r = minRank; r < rank; r++)
  {
    rest += rankInfo[r].count;
    rankInfo[r].count = 0;
  }

  // Shift down as far as possible.
  unsigned char minRankNew = minRank % 2;
  if (minRankNew == 0)
    minRankNew = 2;

  minRank = minRankNew;
  rankInfo[minRank].count += rest;

  return true;
}


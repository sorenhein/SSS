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


void Declarer::resize(
  const unsigned cardsIn,
  const CardPosition sideIn)
{
  Player::resize(cardsIn, sideIn);

  best.resize(cardsIn+1);
  for (unsigned rThis = 0; rThis <= cardsIn; rThis++)
    best[rThis].resize(cardsIn+1);

  bestNew.resize(cardsIn+1);
  for (unsigned rThis = 0; rThis <= cardsIn; rThis++)
    bestNew[rThis].resize(cardsIn+1);
}


void Declarer::setVoid()
{
  if (len > 0)
    return;

  // A real void.
  minRank = 0;
  maxRank = 0;

  rankInfo[0].count = 1;
  rankInfo[0].ptr = nullptr;

  // For North-South, a void goes in cardsNew, cardsPtr and
  // ranksPtr.  This is recognized by forceFlag == false.

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
    rankInfo[cit->getRank()].names += cit->getName();
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

  for (unsigned cno = 0; cno < cards.size(); cno++)
  {
    if (cards[cno].getDepth() == 1)
    {
      // Find the index past the current rank (may be beyond the end).
      unsigned dno = cno+1;
      while (dno < cards.size() && cards[dno].getDepth() > 0)
        dno++;

      unsigned maxDepth = dno - cno;

      // Flip the ranks, starting from the zero depth.
      for (cno--; cno < dno; cno++)
        cards[cno].flipDepth(maxDepth);
    }
  }
}


void Declarer::countNumbers(vector<unsigned>& numbers) const
{
  // TODO Probably we can just use cards to look this up?
  if (len == 0)
  {
    // TODO Hopefully won't be necessary long-term.
    numbers.resize(1);
    return;
  }

  numbers.resize(maxRank+1);
  unsigned running = len-1;
  for (unsigned r = maxRank; r > 0; r--)
  {
    numbers[r] = running;
    running -= rankInfo[r].count;
  }
}


void Declarer::setBest(const Declarer& partner)
{
  // We pre-calculate the Winner that arises for any combination of
  // a lead and a partner card, assuming that the declaring side does
  // in fact win the trick.

assert(side != POSITION_OPPS);
  WinningSide wside, pside;
  if (side == POSITION_NORTH)
  {
    wside = WIN_NORTH;
    pside = WIN_SOUTH;
  }
  else
  {
    wside = WIN_SOUTH;
    pside = WIN_NORTH;
  }

  // Loop over the unique cards with depth 0 among different ranks.
  for (auto& rankTop: ranksPtr)
  {
    const unsigned rLead = rankTop->getRank();
assert(rLead < bestNew.size());
    for (auto& rankTopOther: partner.ranksPtr)
    {
      const unsigned rPard = rankTopOther->getRank();
assert(rPard < bestNew[rLead].size());
      
      Winner& current = bestNew[rLead][rPard];
      current.reset();

      if (rLead > rPard)
      {
        current.set(wside, * rankTop);
      }
      else if (rLead < rPard)
      {
        current.set(pside, * rankTopOther);
      }
      else
      {
        current.set(wside, * rankTop);
        current.set(pside, * rankTopOther);
      }
    }
  }
}


bool Declarer::playRank(
  const unsigned rank,
  const Declarer& partner,
  const unsigned maxGlobalRank)
{
  assert(rankInfo[rank].count > 0);
  rankInfo[rank].count--;

  return (rank > 1 &&
    rank != maxGlobalRank &&
    ! Declarer::hasRank(rank) &&
    ! partner.hasRank(rank));
}


void Declarer::finish(const Declarer& partner)
{
  Declarer::setSingleRank();
  Declarer::fixDepths();
  Declarer::setBest(partner);
}


bool Declarer::greater(
  const Declarer& p2,
  const Opponents& opps) const
{
  // There may be rank collapses from played EW cards.
  unsigned run1 = 0;
  unsigned run2 = 0;
  for (unsigned r = max(maxRank, p2.maxRank); ; r -= 2)
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


const Winner& Declarer::getWinner(
  const unsigned lead,
  const unsigned pard) const
{
  assert(lead < bestNew.size());
  assert(pard < bestNew[lead].size());
  return bestNew[lead][pard];
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


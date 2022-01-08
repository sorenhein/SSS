/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

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
    {
      // Nothing else happens, so equality in this sense.
      if (Declarer::isVoid())
        return true;
      else
        // TODO Actually this seems wrong, at least when cards
        // have been played, as these only change rankInfo and
        // not cards.
        return (Declarer::top() > p2.top());
      // return true; 
    }
    
    run1 = 0;
    run2 = 0;
  }
}


bool Declarer::tops(const Declarer& p2) const
{
  // This method is used to order ranked holdings.

  if (Declarer::isVoid())
    // Both void OK.
    return p2.isVoid();
  else if (p2.isVoid())
    return true;
  else
    return (Declarer::top() > p2.top());
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


bool Declarer::isSingleRun() const
{
  // Returns true if all the player's cards are consecutive.
  if (! singleRank)
    return false;
  if (len == 1)
    return true;

  const unsigned char absNumber = cards.front().getAbsNumber();
  for (auto citer = next(cards.begin()); citer != cards.end(); citer++)
  {
    if (citer->getAbsNumber() != absNumber)
      return false;
  }

  return true;
}


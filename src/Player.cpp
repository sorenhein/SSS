#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Player.h"

#include "struct.h"
#include "const.h"

/*
 * This class keeps track of player holdings within Ranks.  There is
 * one instance for North, one for South, and one for opposing cards.
 */


Player::Player()
{
  Player::clear();
}


Player::~Player()
{
}


void Player::resize(const unsigned cardsIn)
{
  // Worst case, leaving room for voids at rank 0.
  ranks.resize(cardsIn+1);
  maxPos = cardsIn;

  fullCount.resize(cardsIn+1);
  maxRank = cardsIn;
}

void Player::clear()
{
  ranks.clear();
  fullCount.clear();
}

void Player::zero()
{
  for (unsigned pos = 0; pos <= maxPos; pos++)
    ranks[pos].count = 0;

  for (unsigned rank = 0; rank <= maxRank; rank++)
    fullCount[rank] = 0;
        
  len = 0;
}

void Player::update(
  const unsigned position,
  const unsigned rank,
  bool& firstFlag)
{
  ranks[position].add(rank);
  maxPos = position;

  fullCount[rank]++;
  maxRank = rank;

  if (firstFlag)
  {
    minPos = position;
    minRank = rank;
    firstFlag = false;
  }

  len++;
}

void Player::setVoid(const bool forceFlag)
{
  if (forceFlag || len == 0)
  {
    ranks[0].add(0);
    minPos = 0;
    maxPos = 0;

    fullCount[0] = 1;
    minRank = 0;
    maxRank = 0;
  }
}

void Player::setSingleRank()
{
  singleRank = (len >= 1 && minRank == maxRank);
}


bool Player::greater(
  const Player& p2,
  const Player& opps) const
{
  // There may be rank collapses from played EW cards.
  unsigned run1 = 0;
  unsigned run2 = 0;
  for (unsigned r = max(maxRank, p2.maxRank); ; r -= 2)
  {
    run1 += fullCount[r];
    run2 += p2.fullCount[r];
    if (r > 2 && opps.fullCount[r-1] == 0)
      continue;  // EW collapse
    else if (run1 > run2)
      return true;
    else if (run1 < run2)
      return false;
    else if (r <= 2)
      return true; // Nothing else happens, so equality
    
    run1 = 0;
    run2 = 0;
  }
}


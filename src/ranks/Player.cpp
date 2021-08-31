#include <iostream>
#include <iomanip>
#include <sstream>
#include <codecvt>
#include <cassert>

#include "Player.h"
#include "../const.h"

/*
 * This class keeps track of player holdings within Ranks.  
 * There is one derived class for North-South and another for
 * opposing cards.
 */


Player::Player()
{
}


Player::~Player()
{
}


void Player::resize(
  const unsigned cardsIn,
  const Side sideIn)
{
  // Worst case, leaving room for voids at rank 0.
  rankInfo.resize(cardsIn+1);
  side = sideIn;
}


void Player::zero()
{
  cards.clear();
  cardsPtr.clear();
  ranksPtr.clear();

  for (auto& ri: rankInfo)
    ri.clear();
        
  len = 0;

  numberNextCard = 0;
  firstUpdateFlag = true;

  maxRank = numeric_limits<unsigned char>::max();

  depthNext = 0;
  posNext = 1;

  firstOfRankFlag = true;
}


void Player::updateStep(const unsigned char rankNew)
{
  // Back down to the first card of the next rank.
  depthNext = 0;

  if (rankNew == maxRank+2)
  {
    // Player has this reduced rank already, so we advance.
    posNext++;
    firstOfRankFlag = true;
  }
}


void Player::update(
  const unsigned char rank,
  const unsigned char absCardNumber)
{
  if (firstUpdateFlag)
  {
    minRank = rank;
    minAbsCardNumber = absCardNumber;
    firstUpdateFlag = false;
  }

  len++;
  maxRank = rank;
  
  cards.emplace_back(Card());
  Card * cptr = &cards.back();
  cptr->set(rank, depthNext, numberNextCard, 
    static_cast<unsigned char>(CARD_NAMES[absCardNumber]));

  cardsPtr.push_back(cptr);

  if (firstOfRankFlag)
  {
    ranksPtr.push_back(cptr);
    firstOfRankFlag = false;
  }
  else
    ranksPtr.back() = cptr;

  // This may be an overwrite with the same value, but no matter.
  rankInfo[rank].count++;

  numberNextCard++;
  depthNext++;
}


void Player::upshift(const unsigned char shift)
{
  // Shifts the entire rankInfo vector up to make room for low
  // cards in some cases.  This is intended for making minimal
  // combinations.
  
  // Deal with voids.
  if (maxRank == 0)
    return;

  assert(maxRank+shift < static_cast<unsigned char>(rankInfo.size()));

  // Shift up.
  for (unsigned r = maxRank; r >= minRank; r--)
    rankInfo[r+shift] = rankInfo[r];

  for (unsigned char r = minRank; r < minRank+shift; r++)
    rankInfo[r].clear();

  minRank += static_cast<unsigned char>(shift);
  maxRank += static_cast<unsigned char>(shift);
}


void Player::restoreRank(const unsigned char rank)
{
  rankInfo[rank].count++;
}


bool Player::hasRank(const unsigned char rankIn) const
{
  return (rankInfo[rankIn].count > 0);
}


unsigned Player::length() const
{
  return len;
}


unsigned char Player::minFullRank() const
{
  return minRank;
}


unsigned char Player::maxFullRank() const
{
  return maxRank;
}


bool Player::isVoid() const
{
  return (len == 0);
}


unsigned char Player::count(const unsigned char rankIn) const
{
  return rankInfo[rankIn].count;
}


string Player::playerName() const
{
  if (side == SIDE_NORTH)
    return "North";
  else if (side == SIDE_SOUTH)
    return "South";
  else
    return "Opps";
}


string Player::strRankHeader() const
{
  stringstream ss;
  ss << right <<
    setw(6) << Player::playerName() <<
    setw(3) << "#" << " " <<
    setw(12) << left << "cards";

  return ss.str();
}


string Player::strRank(const unsigned char rank) const
{
  stringstream ss;
  if (rankInfo[rank].count == 0)
    ss << right <<
      setw(6) << "-" << 
      setw(3) << "-" << " " <<
      setw(12) << left << "-";
  else
  {
    ss << right <<
      setw(6) << Player::playerName() <<
      setw(3) << +rankInfo[rank].count << " " <<
      setw(12) << left << rankInfo[rank].names;
  }

  return ss.str();
}


wstring Player::wstr() const
{
  if (len == 0)
    return L"-";

  string s = "";
  for (auto it = rankInfo.rbegin(); it != rankInfo.rend(); it++)
    s += it->names;

  wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
  return conv.from_bytes(s);
}

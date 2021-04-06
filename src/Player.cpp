#include <iostream>
#include <iomanip>
#include <sstream>
#include <codecvt>
#include <cassert>

#include "Player.h"

/*
 * This class keeps track of player holdings within Ranks.  
 * There is one derived class for North-South and another for
 * opposing cards.
 */


Player::Player()
{
  Player::clear();
}


Player::~Player()
{
}


void Player::clear()
{
  // Is this clearing needed or used?
  rankInfo.clear();

  cards.clear();
  cardsPtr.clear();
  ranksPtr.clear();
}


void Player::resize(
  const unsigned cardsIn,
  const CardPosition sideIn)
{
  // Worst case, leaving room for voids at rank 0.
  rankInfo.resize(cardsIn+1);

  cards.clear();

  maxRank = cardsIn;

  side = sideIn;
}


void Player::zero()
{
  for (auto& ri: rankInfo)
    ri.clear();
        
  len = 0;

  numberNextCard = 0;
  firstUpdateFlag = true;

  cards.clear();

  cardsPtr.clear();
  ranksPtr.clear();

  maxRank = numeric_limits<unsigned>::max();

  depthNext = 0;
  posNext = 1;

  firstOfRankFlag = true;
}


void Player::updateStep(const unsigned rankNew)
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
  const unsigned rank,
  const unsigned absCardNumber)
{
  maxRank = rank;


  if (firstUpdateFlag)
  {
    minRank = rank;
    minAbsCardNumber = absCardNumber;
    firstUpdateFlag = false;
  }

  len++;
  
  cards.emplace_back(Card());
  Card * cptr = &cards.back();
  cptr->set(rank, depthNext, numberNextCard, CARD_NAMES[absCardNumber]);

  cardsPtr.push_back(cptr);

  if (firstOfRankFlag)
  {
    ranksPtr.push_back(cptr);
    firstOfRankFlag = false;
  }
  else
    ranksPtr.back() = cptr;

  rankInfo[rank].count++;
  rankInfo[rank].ptr = cptr;

  numberNextCard++;
  depthNext++;
}


void Player::restoreRank(const unsigned rank)
{
  rankInfo[rank].count++;
}


bool Player::hasRank(const unsigned rankIn) const
{
  return (rankInfo[rankIn].count > 0);
}


unsigned Player::length() const
{
  return len;
}


unsigned Player::minFullRank() const
{
  return minRank;
}


unsigned Player::maxFullRank() const
{
  return maxRank;
}


bool Player::isVoid() const
{
  return (len == 0);
}


unsigned Player::count(const unsigned rankIn) const
{
  return rankInfo[rankIn].count;
}


string Player::playerName() const
{
  if (side == POSITION_NORTH)
    return "North";
  else if (side == POSITION_SOUTH)
    return "South";
  else
    return "Opps";
}


string Player::strRankHeader() const
{
  stringstream ss;
  ss << right <<
    setw(8) << Player::playerName() <<
    setw(4) << "#" <<
    setw(6) << "cards";

  return ss.str();
}


string Player::strRank(const unsigned rank) const
{
  stringstream ss;
  if (rankInfo[rank].count == 0)
    ss << setw(8) << "-" << setw(4) << "-" << setw(6) << "-";
  else
  {
    ss << 
      setw(8) << Player::playerName() <<
      setw(4) << rankInfo[rank].count <<
      setw(6) << rankInfo[rank].names;
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

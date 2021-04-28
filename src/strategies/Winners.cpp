#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "../Play.h"

#include "Winners.h"

/*
   The winner is structured as a number of sub-winners that are
   added / OR'ed together.  Declarer can choose between the sub-winners.
   Each sub-winner potentially has a North card and a South card that
   are both required, so they are multiplied / AND'ed together.

   In the following examples there are 6 cards in play, so EW have 2.

   AK / xx: (N:K && S:-).
   Ax / Kx: (N:A && S:K).
   Kx / Qx: (N:K && S:K).
   KQ / Jx: (N:Q) || (N:K && S:J).  So effectively, two tops.
   KJ / Qx: (N:J) || (N:K && S:Q).  (This would not be canonical.)
   KQ / JT: (N:Q) || (S:T) || (N:K && S:J).
   AQ / JT: (N:Q) || (S:T).
   AQ / Jx: (N:Q).
*/


Winners::Winners()
{
  Winners::reset();
}


Winners::~Winners()
{
}


void Winners::reset()
{
  winners.clear();
}


void Winners::setEmpty()
{
  // Makes a new winner every time.
  winners.emplace_back(Winner());
  Winner& win = winners.back();
  win.setEmpty();
}


void Winners::set(
  const WinningSide sideIn,
  const Card& card)
{
  // Makes a new subwinner every time.
  winners.emplace_back(Winner());
  Winner& win = winners.back();
  win.set(sideIn, card);
}


void Winners::set(
  const Card& north,
  const Card& south)
{
  Winners::reset();

  if (north.getRank() > south.getRank())
    Winners::set(WIN_NORTH, north);
  else if (north.getRank() < south.getRank())
    Winners::set(WIN_SOUTH, south);
  else
  {
    Winners::set(WIN_NORTH, north);
    Winners::set(WIN_SOUTH, south);
  }
}


bool Winners::operator != (const Winners& w2) const
{
  return ! (* this == w2);
}


bool Winners::operator == (const Winners& w2) const
{
  // This is a simple first implementation that assumes the same order.
  if (winners.size() != w2.winners.size())
    return false;

  auto iter = winners.begin();
  auto iter2 = w2.winners.begin();
  while (iter != winners.end())
  {
    if (* iter != * iter2)
      return false;
    
    iter++;
    iter2++;
  }

  return true;
}


void Winners::integrate(const Winner& swNew)
{
  // winners are a minimal set.
  // The new subwinner may dominate existing winners.
  // It may also be dominated by at least one existing winner.
  // If neither is true, then it is a new winner.

  auto switer = winners.begin();
  while (switer != winners.end())
  {
    const WinnerCompare cmp = switer->declarerPrefers(swNew);
    if (cmp == WIN_FIRST || cmp == WIN_EQUAL)
    {
      // The new subwinner is inferior.
      return;
    }
    else if (cmp == WIN_SECOND)
    {
      // The existing subwinner is inferior.
      switer = winners.erase(switer);
    }
    else
      switer++;
  }

  winners.push_back(swNew);
}


void Winners::operator *= (const Winners& w2)
{
  // The opponents have the choice.

  if (w2.winners.size() == 0)
  {
    // OK as is.
    return;
  }
  else if (winners.size() == 0)
  {
    * this = w2;
    return;
  }

  // All winner's of a winner are of the same rank.
  if (w2.rankExceeds(* this))
  {
    // OK as is: Stick with the lower rank.
    return;
  }
  else if (Winners::rankExceeds(w2))
  {
    * this = w2;
    return;
  }

  // This is surely inefficient.
  Winners w1 = * this;
  Winners::reset();

  for (auto& sw1: w1.winners)
  {
    for (auto& sw2: w2.winners)
    {
      Winner sw = sw1;
      sw *= sw2;
      Winners::integrate(sw);
    }
  }
}


void Winners::flip()
{
  for (auto& winner: winners)
    winner.flip();
}


void Winners::update(const Play& play)
{
  for (auto& winner: winners)
    winner.update(play);

  if (play.trickNS)
    * this *= play.currBest;
}


bool Winners::rankExceeds(const Winners& w2) const
{
  // This requires both winners to have winner's.
  // Each winner has consistent ranks.
  return (winners.front().rankExceeds(w2.winners.front()));
}


string Winners::strEntry() const
{
  string s = "";
  bool firstFlag = true;
  for (auto& winner: winners)
  {
    if (firstFlag)
    {
      firstFlag = false;
      s += winner.str();
    }
    else
      // Join with a comma in between.
      s += "," + winner.str();
  }
  return s;
}


string Winners::strDebug() const
{
  string s = "";
  for (auto& winner: winners)
    s += winner.strDebug();
  return s;
}


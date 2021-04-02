#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Winner.h"

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


Winner::Winner()
{
  Winner::reset();
}


Winner::~Winner()
{
}


void Winner::reset()
{
  subwinners.clear();
}


void Winner::set(
  const WinningSide sideIn,
  const unsigned rankIn,
  const unsigned depthIn,
  const unsigned numberIn,
  const char nameIn)
{
  // Makes a new subwinner every time.
  subwinners.emplace_back(Subwinner());
  Subwinner& sw = subwinners.back();
  sw.set(sideIn, rankIn, depthIn, numberIn, nameIn);
}


void Winner::set(
  const WinningSide sideIn,
  const Card& card)
{
  // Makes a new subwinner every time.
  subwinners.emplace_back(Subwinner());
  Subwinner& sw = subwinners.back();
  sw.set(sideIn, card);
}


bool Winner::operator != (const Winner& w2) const
{
  // This is a simple first implementation that assumes the same order.
  if (subwinners.size() != w2.subwinners.size())
    return false;

  auto iter = subwinners.begin();
  auto iter2 = w2.subwinners.begin();
  while (iter != subwinners.end())
  {
    if (* iter != * iter2)
      return false;
    
    iter++;
    iter2++;
  }

  return true;
}


void Winner::integrate(const Subwinner& swNew)
{
  // subwinners are a minimal set.
  // The new subwinner may dominate existing subwinners.
  // It may also be dominated by at least one existing subwinner.
  // If neither is true, then it is a new subwinner.

  auto switer = subwinners.begin();
  while (switer != subwinners.end())
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
      switer = subwinners.erase(switer);
    }
    else
      switer++;
  }

  subwinners.push_back(swNew);
}


void Winner::operator *= (const Winner& w2)
{
  // The opponents have the choice.

  if (w2.subwinners.size() == 0)
  {
    // OK as is.
    return;
  }
  else if (subwinners.size() == 0)
  {
    * this = w2;
    return;
  }

  // All subwinners of a winner are of the same rank.
  if (w2.rankExceeds(* this))
  {
    // OK as is: Stick with the lower rank.
    return;
  }
  else if (Winner::rankExceeds(w2))
  {
    * this = w2;
    return;
  }

  // This is surely inefficient.
  Winner w1 = * this;
  Winner::reset();

  for (auto& sw1: w1.subwinners)
  {
    for (auto& sw2: w2.subwinners)
    {
      Subwinner sw = sw1;
      sw *= sw2;
      Winner::integrate(sw);
    }
  }
}


void Winner::flip()
{
  for (auto& subwinner: subwinners)
    subwinner.flip();
}


void Winner::update(
  vector<Card> const * northOrderPtr,
  vector<Card> const * southOrderPtr,
  Winner const * currBestPtr)
{
  for (auto& subwinner: subwinners)
    subwinner.update(northOrderPtr, southOrderPtr);

  if (currBestPtr)
    * this *= * currBestPtr;
}


bool Winner::rankExceeds(const Winner& w2) const
{
  // This requires both winners to have subwinners.
  // Each winner has consistent ranks.
  return (subwinners.front().rankExceeds(w2.subwinners.front()));
}


string Winner::strEntry() const
{
  string s = "";
  bool firstFlag = true;
  for (auto& subwinner: subwinners)
  {
    if (firstFlag)
    {
      firstFlag = false;
      s += subwinner.str();
    }
    else
      // Join with a comma in between.
      s += "," + subwinner.str();
  }
  return s;
}


string Winner::strDebug() const
{
  string s = "";
  for (auto& subwinner: subwinners)
    s += subwinner.strDebug();
  return s;
}


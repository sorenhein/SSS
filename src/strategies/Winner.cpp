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
  const unsigned numberIn)
{
  // Makes a new subwinner every time.
  subwinners.emplace_back(Subwinner());
  Subwinner& sw = subwinners.back();
  sw.set(sideIn, rankIn, depthIn, numberIn);
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

  // For now:
  assert(subwinners.size() == 1);
  assert(w2.subwinners.size() == 1);

  subwinners.front() *= w2.subwinners.front();
}


void Winner::flip()
{
  for (auto& subwinner: subwinners)
    subwinner.flip();
}


void Winner::update(
  const vector<Subwinner>& northOrder,
  const vector<Subwinner>& southOrder,
  const Subwinner& currBest,
  const unsigned trickNS)
{
  for (auto& subwinner: subwinners)
    subwinner.update(northOrder, southOrder, currBest, trickNS);
}


string Winner::strEntry() const
{
  string s = "";
  for (auto& subwinner: subwinners)
    s += subwinner.str();
  return s;
}


string Winner::strDebug() const
{
  string s = "";
  for (auto& subwinner: subwinners)
    s += subwinner.strDebug();
  return s;
}


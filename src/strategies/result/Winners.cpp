/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Winners.h"

#include "../../plays/Play.h"
#include "../../utils/Comparer.h"
#include "../../const.h"

/*
   Winners consist of Winner's.  Each Winner may have N, S or both set.
   If set, they are both needed.  The Winners are at declarer's choice.
   We think of a Winner as a product and Winners as a sum.

   Adding Winners means concatenating them and checking that there is
   no domination.

   Multiplying Winners means multiplying together the individual sums.

   Winners are only used by MultiResult, not by Result.
*/


Winners::Winners()
{
  Winners::reset();
}


void Winners::reset()
{
  winners.clear();
  setFlag = false;
}


void Winners::set(const Winner& winner)
{
  Winners::reset();

  if (winner.empty())
  {
    // Winners is still empty, but is now considered set.
    setFlag = true;
    return;
  }
  else
  {
    winners.push_back(winner);
    setFlag = true;
  }
}


void Winners::set(
  const Card& north,
  const Card& south)
{
  Winners::reset();

  winners.emplace_back(Winner());
  winners.back().setHigherOf(north, south);

  // We don't check whether north and/or south is non-empty.
  setFlag = true;
}


bool Winners::empty() const
{
  // Returns true whether winners is unset or set to be empty.
  return winners.empty();
}


void Winners::fillComparer(
  Comparer& comparer,
  const Winners& winners2) const
{
  unsigned n1 = 0;
  for (auto& win1: winners)
  {
    unsigned n2 = 0;
    for (auto& win2: winners2.winners)
    {
      comparer.log(n1, n2, win1.compareNonEmpties(win2));
      n2++;
    }
    n1++;
  }
}


void Winners::addCore(const Winner& winner)
{
  // winners are a minimal set.
  // The new winner may dominate existing winners.
  // It may also be dominated by at least one existing winner.
  // If neither is true, then it is a new winner.

  // Both Winners and winner2 are non-empty and have the same rank.
  // Actually the rank condition is not required.

  auto witer = winners.begin();
  while (witer != winners.end())
  {
    const Compare cmp = witer->compareNonEmpties(winner);
    if (cmp == WIN_FIRST || cmp == WIN_EQUAL)
    {
      // The new subwinner is inferior.
      return;
    }
    else if (cmp == WIN_SECOND)
    {
      // The existing subwinner is inferior.
      witer = winners.erase(witer);
    }
    else
      witer++;
  }

  winners.push_back(winner);
}


void Winners::operator += (const Winner& winner)
{
  // This method is not completely analogous to += Winners,
  // as it is used to accumulate Winner's in a loop.  If Winners
  // starts out empty, that does not mean that declarer gets to
  // enjoy no constraints; it means that we haven't added any
  // constraints yet.  In contrast, if we are adding Winners,
  // an empty addend means no constraint.

  const unsigned r1 = Winners::rank();
  const unsigned r2 = winner.getRank();

  if (winner.empty())
  {
    // This part is normal: An empty input removes declarer's constraints.
    Winners::reset();
    setFlag = true;
    return;
  }
  else if (r1 > r2 && ! Winners::empty())
  {
    // This too is normal: A too-low input does nothing.
    return;
  }
  else if (r2 > r1 || ! setFlag)
  {
    // The unusual condition is ! setFlag, which we treat the same 
    // as if the new winner is preferable to declarer.
    Winners::reset();
    winners.push_back(winner);
    setFlag = true;
    return;
  }

  setFlag = true;

  Winners::addCore(winner);
}


void Winners::operator += (const Winners& winners2)
{
  // Declarer has the choice and prefers no constraints.  
  // This is complementary to *=.

  const unsigned r1 = Winners::rank();
  const unsigned r2 = winners2.rank();

  if (! winners2.setFlag)
  {
    // Ignore.
    return;
  }
  else if (! setFlag || r2 > r1)
  {
    // Switch to the only/higher rank.
    * this = winners2;
    return;
  }
  else if (Winners::empty() || r1 > r2)
  {
    // Declarer prefers no constraints.
    return;
  }

  setFlag = true;

  for (auto& win2: winners2.winners)
    Winners::addCore(win2);
}


void Winners::operator *= (const Winners& winners2)
{
  // The opponents have the choice.

  const unsigned r1 = Winners::rank();
  const unsigned r2 = winners2.rank();

  if (winners2.empty() || r1 < r2)
  {
    // Stick with the only/lower rank.
    setFlag = winners2.setFlag;
    return;
  }
  else if (r2 < r1)
  {
    // Switch to the lower rank.
    * this = winners2;
    return;
  }

  // This could be faster, but it's not that slow.
  Winners winners1 = move(* this);
  Winners::reset();
  setFlag = true;

  for (auto& win1: winners1.winners)
  {
    for (auto& win2: winners2.winners)
    {
      Winner wprod = win1;
      wprod *= win2;
      * this += wprod;
    }
  }
}


void Winners::operator *= (const Winner& winner)
{
  // The opponents have the choice.

  const unsigned r1 = Winners::rank();
  const unsigned r2 = winner.getRank();

  if (winner.empty() || r1 < r2)
  {
    // OK as is: Stick with the only/lower rank.
    // We consider that an empty winner is set.
    setFlag = true;
    return;
  }
  else if (r2 < r1)
  {
    winners.clear();
    winners.push_back(winner);
    setFlag = true;
    return;
  }

  // This could be faster, but it's not that slow.
  Winners winners1 = move(* this);
  Winners::reset();
  setFlag = true;

  for (auto& win1: winners1.winners)
  {
    Winner wprod = win1;
    wprod *= winner;
    * this += wprod;
  }
}


bool Winners::operator == (const Winners& winners2) const
{
  assert(setFlag && winners2.setFlag);

  const unsigned s1 = winners.size();
  const unsigned s2 = winners2.winners.size();

  if (s1 == 0)
    return winners2.empty();
  else if (s2 == 0)
    return false;
  else if (s1 == 1 && s2 == 1)
    return winners.front() == winners2.winners.front();
  else if (s1 != s2)
    return false;
  else
  {
    Comparer comparer;
    comparer.resize(s1, s2);
    Winners::fillComparer(comparer, winners2);
    return (comparer.compare() == WIN_EQUAL);
  }
}


bool Winners::operator != (const Winners& winners2) const
{
  return ! (* this == winners2);
}


Compare Winners::compare(const Winners& winners2) const
{
  assert(setFlag && winners2.setFlag);

  const unsigned s1 = winners.size();
  const unsigned s2 = winners2.winners.size();

  if (s1 == 0)
  {
    // Declarer prefers no restrictions.
    return (winners2.empty() ? WIN_EQUAL : WIN_FIRST);
  }
  else if (s2 == 0)
    return WIN_SECOND;
  else if (s1 == 1 && s2 == 1)
    return winners.front().compareNonEmpties(winners2.winners.front());
  else if (Winners::rank() > winners2.rank())
    return WIN_FIRST;
  else if (winners2.rank() > Winners::rank())
    return WIN_SECOND;
  else
  {
    Comparer comparer;
    comparer.resize(s1, s2);
    Winners::fillComparer(comparer, winners2);
    return comparer.compare();
  }
}


void Winners::flip()
{
  for (auto& winner: winners)
    winner.flip();
}


void Winners::expand(const char rankAdder)
{
  for (auto& winner: winners)
    winner.expand(rankAdder);
}


unsigned char Winners::rank() const
{
  if (Winners::empty())
    return UCHAR_NOT_SET;
  else
    return winners.front().getRank();
}


unsigned char Winners::absNumber() const
{
  if (Winners::empty())
    return 0;
  else
  {
    assert(winners.size() == 1);
    return winners.front().getAbsNumber();
  }
}


const Winner& Winners::constantWinner() const
{
  assert(winners.size() == 1);
  return winners.front();
}


string Winners::strEntry() const
{
  if (Winners::empty())
    return (setFlag ? "o" : "-");

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


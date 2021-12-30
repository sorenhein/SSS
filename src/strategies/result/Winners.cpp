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
*/


Winners::Winners()
{
  Winners::reset();
}


void Winners::reset()
{
  winners.clear();
}


// TODO May not be needed long-term
void Winners::addEmpty()
{
  // Makes a new winner every time.
  winners.emplace_back(Winner());
  Winner& win = winners.back();
  win.setEmpty();

assert(winners.size() == 1);
}


void Winners::set(const Winner& winner)
{
  winners.clear();
  winners.push_back(winner);
}


void Winners::set(
  const Side sideIn,
  const Card& card)
{
  // Makes a new subwinner every time.
  winners.emplace_back(Winner());
  Winner& win = winners.back();
  win.set(sideIn, card);

assert(winners.size() == 1);
}


void Winners::set(
  const Card& north,
  const Card& south)
{
  Winners::reset();

  winners.emplace_back(Winner());
  Winner& win = winners.back();
  win.setHigherOf(north, south);
}


bool Winners::empty() const
{
  const unsigned s = winners.size();
  return (s == 0 || (s == 1 && winners.front().empty()));
}


void Winners::push_back(const Winner& winner)
{
  winners.push_back(winner);
}


unsigned char Winners::rank() const
{
  if (winners.empty())
    return 0;
  else
    return winners.front().getRank();
}


unsigned char Winners::absNumber() const
{
  if (winners.empty())
    return 0;
  else
  {
    assert(winners.size() == 1);
    return winners.front().getAbsNumber();
  }
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


void Winners::operator += (const Winner& winner2)
{
  // winners are a minimal set.
  // The new winner may dominate existing winners.
  // It may also be dominated by at least one existing winner.
  // If neither is true, then it is a new winner.


// TODO Why does this lead to a fail?
/*
  if (Winners::empty())
  {
    // OK as is: Declarer wants no constraints.
    return;
  }
  else 
*/

  if (winner2.empty())
  {
    Winners::reset();
    return;
  }

  auto witer = winners.begin();
  while (witer != winners.end())
  {
    const Compare cmp = witer->compareNonEmpties(winner2);
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

  winners.push_back(winner2);

// It seems we do need to permit this for ranges
// assert(winners.size() == 1);
}


void Winners::operator += (const Winners& winners2)
{
  // Declarer has the choice.  This is complementary to *=.

  if (Winners::empty())
  {
    // OK as is: Declarer wants no constraints.
    return;
  }
  else if (winners2.empty())
  {
    Winners::reset();
    return;
  }

  // All winner's of a winner are of the same rank.
  if (winners2.rankExceeds(* this))
  {
    // Go with the higher rank.
    * this = winners2;
    return;
  }
  else if (Winners::rankExceeds(winners2))
  {
    // OK as is: Stick with the lower rank.
    return;
  }

  for (auto& win2: winners2.winners)
    * this += win2;
}


void Winners::operator *= (const Winners& winners2)
{
  // The opponents have the choice.

  if (winners2.empty())
  {
    // OK as is.
    return;
  }
  else if (Winners::empty())
  {
    * this = winners2;
    return;
  }

  // All winner's of a winner are of the same rank.
  if (winners2.rankExceeds(* this))
  {
    // OK as is: Stick with the lower rank.
    return;
  }
  else if (Winners::rankExceeds(winners2))
  {
    * this = winners2;
    return;
  }

  // This could be faster, but it's not that slow.
  Winners winners1 = move(* this);
  Winners::reset();

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

  if (winner.empty())
  {
    // OK as is.
    return;
  }
  else if (Winners::empty())
  {
    winners.clear();
    winners.push_back(winner);
    return;
  }

  // All winner's of a winner are of the same rank.
  if (winner.rankExceeds(winners.front()))
  {
    // OK as is: Stick with the lower rank.
    return;
  }
  else if (winners.front().rankExceeds(winner))
  {
    winners.clear();
    winners.push_back(winner);
    return;
  }

  // This could be faster, but it's not that slow.
  Winners winners1 = move(* this);
  Winners::reset();

  for (auto& win1: winners1.winners)
  {
    Winner wprod = win1;
    wprod *= winner;
    * this += wprod;
  }
}


bool Winners::operator != (const Winners& winners2) const
{
  return ! (* this == winners2);
}


bool Winners::operator == (const Winners& winners2) const
{
  const unsigned s1 = winners.size();
  const unsigned s2 = winners2.winners.size();

  if (Winners::empty())
    return winners2.empty();
  else if (winners2.empty())
    return false;
  else if (winners.size() == 1 && winners2.winners.size() == 1)
  {
    return winners.front() == winners2.winners.front();
  }
  else if (winners.size() != winners2.winners.size())
  {
    return false;
  }
  else
  {
    Comparer comparer;
    comparer.resize(s1, s2);
    Winners::fillComparer(comparer, winners2);
    return (comparer.compare() == WIN_EQUAL);
  }
}


Compare Winners::compare(const Winners& winners2) const
{
  const unsigned s1 = winners.size();
  const unsigned s2 = winners2.winners.size();

  if (Winners::empty())
    // Declarer prefers no restrictions.
    return (winners2.empty() ? WIN_EQUAL : WIN_FIRST);
  else if (winners2.empty())
    return WIN_SECOND;
  else if (s1 == 1 && s2 == 1)
    return winners.front().compareNonEmpties(winners2.winners.front());
  else if (Winners::rankExceeds(winners2))
    return WIN_FIRST;
  else if (winners2.rankExceeds(* this))
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


const Winner& Winners::front() const
{
  assert(! winners.empty());
  return winners.front();
}


unsigned Winners::size() const
{
  return winners.size();
}


bool Winners::rankExceeds(const Winners& winners2) const
{
  // This requires both winners to have winner's.
  // Each winner has consistent ranks.
  return (winners.front().rankExceeds(winners2.winners.front()));
}


string Winners::strEntry() const
{
  if (Winners::empty())
    return "-";

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


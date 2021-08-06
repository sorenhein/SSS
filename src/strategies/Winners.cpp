#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Winners.h"

#include "../plays/Play.h"

#include "../stats/Comparer.h"

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


bool Winners::empty() const
{
  const unsigned s = winners.size();
  return (s == 0 || (s == 1 && winners.front().empty()));
}


void Winners::fillComparer(
  Comparer& comparer,
  const Winners& w2) const
{
  unsigned n1 = 0;
  for (auto& win1: winners)
  {
    unsigned n2 = 0;
    for (auto& win2: w2.winners)
    {
      comparer.log(n1, n2, win1.declarerPrefers(win2));
      n2++;
    }
    n1++;
  }
}


bool Winners::operator != (const Winners& w2) const
{
  return ! (* this == w2);
}


bool Winners::operator == (const Winners& w2) const
{
  const unsigned s1 = winners.size();
  const unsigned s2 = w2.winners.size();

  if (Winners::empty())
    return w2.empty();
  else if (w2.empty())
    return false;
  else if (winners.size() == 1 && w2.winners.size() == 1)
  {
    return winners.front() == w2.winners.front();
  }
  else if (winners.size() != w2.winners.size())
  {
    return false;
  }
  else
  {
    cout << "w1 size " << winners.size() << endl;
    cout << Winners::strDebug();
    cout << "w2 size " << w2.winners.size() << endl;
    cout << w2.strDebug() << endl;

    Comparer comparer;
    comparer.resize(s1, s2);
    Winners::fillComparer(comparer, w2);

    /*
    cout << comparer.str();
    WinnerCompare c = comparer.compare();
    if (c == WIN_FIRST)
      cout << "WIN_FIRST\n";
    else if (c == WIN_SECOND)
      cout << "WIN_SECOND\n";
    else if (c == WIN_EQUAL)
      cout << "WIN_EQUAL\n";
    else
      cout << "WIN_DIFFERENT\n";

    return (c == WIN_EQUAL);
    */

    return (comparer.compare() == WIN_EQUAL);
  }
}


void Winners::operator += (const Winner& swNew)
{
  // winners are a minimal set.
  // The new subwinner may dominate existing winners.
  // It may also be dominated by at least one existing winner.
  // If neither is true, then it is a new winner.

  auto switer = winners.begin();
  while (switer != winners.end())
  {
// cout << "integrate:\n";
// cout << switer->strDebug();
// cout << "with\n";
// cout << swNew.strDebug();
    const WinnerCompare cmp = switer->declarerPrefers(swNew);
// cout << "cmp " << cmp << endl;
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

  if (w2.empty())
  {
    // OK as is.
    return;
  }
  else if (Winners::empty())
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

  // This could be faster, but it's not that slow.
  Winners w1 = move(* this);
  Winners::reset();

// cout << "Multiplying winners\n";
  for (auto& sw1: w1.winners)
  {
    for (auto& sw2: w2.winners)
    {
      Winner sw = sw1;
// cout << "LHS " << sw.strDebug();
// cout << "RHS " << sw2.strDebug();
      sw *= sw2;
// cout <<"Prd\n" << sw.strDebug();
      // Winners::integrate(sw);
      * this += sw;
// cout << "Winners after *=\n" << Winners::strDebug();
    }
  }
}


void Winners::operator += (const Winners& w2)
{
  assert(! Winners::empty());
  assert(! w2.empty());

  for (auto& sw2: w2.winners)
    * this += sw2;
    // Winners::integrate(sw2);

    /*
  if (winners.size() == 1 && w2.winners.size() == 1)
  {

    if (winners.front().consolidate(w2.winners.front()))
      return;
    else
    {
      cout << "Don't know how to consolidate these simple winners:\n";
      cout << "w1 " << Winners::strDebug();
      cout << "w2 " << w2.strDebug() << endl;
      assert(false);
    }
  }
  else
  {
    for (auto& sw2: w2.winners)
      Winners::integrate(sw2);
  }
    */
}


void Winners::operator |= (const Winners& w2)
{
  // Declarer has the choice.  This is complementary to *=.

  if (Winners::empty())
  {
    // OK as is: Declarer wants no constraints.
    return;
  }
  else if (w2.empty())
  {
    * this = w2;
    return;
  }

  // All winner's of a winner are of the same rank.
  if (w2.rankExceeds(* this))
  {
    // Go with the higher rank.
    * this = w2;
    return;
  }
  else if (Winners::rankExceeds(w2))
  {
    // OK as is: Stick with the lower rank.
    return;
  }

  for (auto& sw2: w2.winners)
    * this += sw2;
    // Winners::integrate(sw2);
}


WinnerCompare Winners::compareForDeclarer(const Winners& w2) const
{
  const unsigned s1 = winners.size();
  const unsigned s2 = w2.winners.size();

  if (Winners::empty())
    // Declarer prefers no restrictions.
    return (w2.empty() ? WIN_EQUAL : WIN_FIRST);
  else if (w2.empty())
    return WIN_SECOND;
  else if (s1 == 1 && s2 == 1)
    return winners.front().declarerPrefers(w2.winners.front());
  else if (winners.front().rankExceeds(w2.winners.front()))
    return WIN_FIRST;
  else if (w2.winners.front().rankExceeds(winners.front()))
    return WIN_SECOND;
  else
  {
    Comparer comparer;
    comparer.resize(s1, s2);
    Winners::fillComparer(comparer, w2);
    return comparer.compare();
  }
}


void Winners::flip()
{
  for (auto& winner: winners)
    winner.flip();
}


void Winners::limitByRank()
{
  // It can happen that the North and South ranks from the next trick
  // are of the same rank, but they are mapped to different ranks in
  // the current trick, and they are combined by declarer's choice.
  // So we check that all Winner's have the same rank and remove any
  // that are lower than the maximum rank.
  unsigned char min = numeric_limits<unsigned char>::max();
  unsigned char max = 0;

  // Find the range of ranks.
  for (auto& winner: winners)
  {
    const unsigned char rank = winner.rank();
    if (rank == numeric_limits<unsigned char>::max())
      continue;

    if (rank < min)
      min = rank;
    if (rank > max)
      max = rank;
  }

  if (min == max)
    return;

  // Remove Winner's of too-low rank.
  auto iter = winners.begin();
  while (iter != winners.end())
  {
    if (iter->rank() < max)
      iter = winners.erase(iter);
    else
      iter++;
  }
}


void Winners::consolidate()
{
  // It can also happen that Winner::update throws out one side,
  // e.g. the South rank, because the mapped rank is lower than
  // the mapped North rank.  That can mean that the reduced winner
  // dominates other Winner's.
  
  if (winners.size() == 2)
  {
    const WinnerCompare c = 
      winners.front().declarerPrefers(winners.back());
    if (c == WIN_FIRST || c == WIN_EQUAL)
      winners.pop_back();
    else if (c == WIN_SECOND)
      winners.pop_front();
  }
  else
  {
    // If this ever happens, we can probably make a Comparer and
    // get back the numbers of Winner's to erase.
    assert(false);
  }
}


void Winners::update(const Play& play)
{
  for (auto& winner: winners)
    winner.update(play);

  if (winners.size() >= 2)
  {
    Winners::limitByRank();
    if (winners.size() >= 2) // May have shrunk in limitByRank
      Winners::consolidate();
  }

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


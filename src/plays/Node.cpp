/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Node.h"
#include "Play.h"

#include "../Distribution.h"
#include "../Combination.h"

#include "../strategies/StratData.h"


Node::Node()
{
  Node::reset();
}


Node::~Node()
{
}


void Node::reset()
{
  parentPtr = nullptr;
  playPtr = nullptr;
  index = numeric_limits<unsigned>::max();

  Node::resetStrategies();
}


void Node::resetStrategies()
{
  strats.reset();
  simpleStrat.reset();
  constants.reset();
}


void Node::set(
  Node * parentPtrIn,
  Play * playPtrIn,
  Node const * prevNodePtr)
{
  parentPtr = parentPtrIn;
  playPtr = playPtrIn;
  strats.reset();
  simpleStrat.reset();
  index = (prevNodePtr ? prevNodePtr->index+1 : 0);
}


void Node::setCombPtr(const Combinations& combinations)
{
  playPtr->setCombPtr(combinations);
}


void Node::linkRhoToLead()
{
  // For plays where partner is void, we link the RHO node directly
  // to the lead node.
  parentPtr = parentPtr->parentPtr->parentPtr;
}


void Node::getNextStrategies(
  const Distribution& dist,
  const bool debugFlag)
{
  // This method should only be used for an RHO node.

  // Find the distribution numbers that are still possible.
  const Survivors& survivors = dist.survivors(* playPtr);

  // Get the strategy from the following combination.
  strats = playPtr->combPtr->strategies();
  if (debugFlag)
  {
    cout << Node::strPlay(LEVEL_RHO);
    cout << playPtr->strLine() << "\n";
    cout << strats.str("Strategy of next trick", true) << endl;
  }

  // Renumber and rotate the strategy.
  strats.adapt(* playPtr, survivors);
  if (debugFlag)
    cout << strats.str("Adapted strategy of next trick", true) << "\n";
}


void Node::propagateRanges()
{
  parentPtr->strats.propagateRanges(strats);
}


void Node::purgeRanges(const bool debugFlag)
{
  if (strats.empty())
    return;

  // Make a list of iterators -- one per Strategy.
  // The iterators later step through one "row" (distribution) of
  // all Strategy's in synchrony.
  StratData stratData;
  stratData.data.resize(strats.size());
  strats.getLoopData(stratData);
  stratData.riter = strats.getRanges().begin();

  constants.resize(parentPtr->strats.getRanges().size());
  auto citer = constants.begin();

  bool eraseFlag = false;
  if (debugFlag)
  {
    cout << "\nPurging ranges: " 
      << Node::strPlay(LEVEL_LHO);
    cout << strats.str("Starting point", true);
  }

  for (auto& parentRange: parentPtr->strats.getRanges())
  {
    // Get to the same distribution in each Strategy if it exists.
    const StratStatus status = stratData.advance(parentRange.dist());
    if (status == STRATSTATUS_END)
      break;
    else if (status == STRATSTATUS_FURTHER_DIST)
      continue;

    if (parentRange.constant())
    {
      * citer = parentRange.constantResult();
      stratData.eraseDominatedDist();
      eraseFlag = true;
      citer++;

      if (debugFlag)
      {
        cout << "Erased constant for parent range:\n";
        cout << parentRange.strHeader(true);
        cout << parentRange.str(true);
      }
    }
    else if (parentRange < * stratData.riter)
    {
      stratData.eraseDominatedDist();
      eraseFlag = true;

      if (debugFlag)
      {
        cout << "Erased dominated range for parent range:\n";
        cout << parentRange.strHeader(true);
        cout << parentRange.str(true);
      }
    }
  }

if (! parentPtr->strats.minimal())
{
  cout << "Non-minimal in purge *=\n";
  cout << endl;
  assert(false);
}

  // Shrink to the size used.
  constants.eraseRest(citer);
  parentPtr->constants *= constants;

  strats.scrutinize(parentPtr->strats.getRanges());

  // Some strategies may be dominated that weren't before.
  if (eraseFlag)
  {
    // TODO, perhaps: 9/1910
    // After erasing, d = 8 goes from 2-3 to 3 constant, and this
    // is now dominated by 2-3.  So we could potentially redo the
    // loop, only for dominance and not for constants this time.
    // We'd have to regenerate stratData first, at least the iter
    // and riter, to rewind.

    strats.consolidate();

    if (debugFlag)
    {
      cout << constants.str("\nNew constants", true) << "\n";
      cout << strats.str("Ranges after purging", true);
    }
  }

if (! parentPtr->strats.minimal())
{
  cout << "Non-minimal at end of purge *=\n";
  cout << endl;
  assert(false);
}
}


void Node::reactivate()
{
  simpleStrat *= constants;
  strats *= simpleStrat;

  // As simpleStrat is completely complementary to strats,
  // we often do not need to re-sort and consolidate.
  // But it can happen that the defenders can hold declarer to a
  // constant 2 tricks (10/16907, lead 7, then rising with the king),
  // or the defender can give declarer several choices in the range
  // of 1-3 tricks.  Since the defenders move after declarer 
  // publishes his strategy, he will never get 3 tricks.  When
  // the simple strategy is multiplied back here, weights can change
  // and therefore the ordering can also change.

  // if (! strats.ordered())
  if (! simpleStrat.empty())
  {
    // TODO Maybe only when real simpleStrats non-empty?
    // Constants don't change anything?
// cout << strats.str("consolidating", true);
    strats.consolidate();
// cout << strats.str("consolidated", true);
  }
}


void Node::cross(
  const Level level,
  const bool debugFlag)
{
  assert(level == LEVEL_RHO || level == LEVEL_LHO);

  if (debugFlag)
  {
    cout << Node::strPlay(level);
    const string s = (level == LEVEL_RHO ? "RHO" : "LHO");
    cout << strats.str("Crossing " + s + " strategy", true);
  }

  cout << parentPtr->strats.str("Before", true);
if (! parentPtr->strats.minimal())
{
  cout << "Non-minimal before *=\n";
  cout << parentPtr->strats.str("Before", true);
  cout << endl;
  assert(false);
}
  parentPtr->strats *= strats;
if (! parentPtr->strats.minimal())
{
  cout << "Non-minimal after *=\n";
  cout << parentPtr->strats.str("After", true);
  cout << endl;
  assert(false);
}

  if (debugFlag)
  {
    const string s = (level == LEVEL_RHO ? "partner" : "lead");
    cout << parentPtr->strats.str(
      "Cumulative " + s + " strategy after this trick", true);
  }
}


void Node::add(
  const Level level,
  const bool debugFlag)
{
  assert(level == LEVEL_LEAD || level == LEVEL_PARD);

  if (debugFlag)
  {
    cout << Node::strPlay(level);
    const string s = (level == LEVEL_LEAD ? "lead" : "partner");
    cout << strats.str("Adding " + s + " strategy", true);
  }

if (! parentPtr->strats.minimal())
{
  cout << "Non-minimal in add before +=\n";
  cout << endl;
  assert(false);
}
  parentPtr->strats += strats;

  if (debugFlag)
  {
    const string s = (level == LEVEL_LEAD ? "overall" : "LHO");
    cout << parentPtr->strats.str(
      "Cumulative " + s + " strategy after this trick", true);
  }

if (! parentPtr->strats.minimal())
{
  cout << "Non-minimal in add after +=\n";
  cout << endl;
  assert(false);
}
}


bool Node::removePlay()
{
  if (strats.empty())
    return true;
  else if (strats.size() == 1)
  {
    parentPtr->simpleStrat *= strats.front();
    return true;
  }
  else
    return false;
}


const Play& Node::play() const
{
  assert(playPtr != nullptr);
  return * playPtr;
}


Strategies& Node::strategies()
{
  return strats;
}


const Strategies& Node::strategies() const
{
  return strats;
}


string Node::strRanges(const string& title) const
{
  return strats.strRanges(title);
}


string Node::strPlay(const Level level) const
{
  assert(playPtr != nullptr);
  return "Node index " + to_string(index) + ", " +
    playPtr->strPartialTrick(level);
}


string Node::strSimple() const
{
  if (simpleStrat.empty())
    return "";
  else
    return simpleStrat.str("simple " + to_string(index));
}


string Node::strSimpleParent() const
{
  if (parentPtr->simpleStrat.empty())
    return "";
  else
    return parentPtr->simpleStrat.str("simple " + to_string(index), true);
}


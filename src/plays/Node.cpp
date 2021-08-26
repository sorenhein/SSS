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
  const bool eraseFlag = strats.purgeRanges(
    constants, parentPtr->strats.getRanges(), debugFlag);

  if (eraseFlag && debugFlag)
  { 
    cout << "\nPurging ranges: " << Node::strPlay(LEVEL_LHO);
    cout << strats.str("End point", true);
  }

  parentPtr->constants *= constants;
}


void Node::reactivate()
{
  strats.reactivate(simpleStrat, constants);
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

  parentPtr->strats *= strats;

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

  parentPtr->strats += strats;

  if (debugFlag)
  {
    const string s = (level == LEVEL_LEAD ? "overall" : "LHO");
    cout << parentPtr->strats.str(
      "Cumulative " + s + " strategy after this trick", true);
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


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
  simpleStrats.reset();
  bounds.reset();
}


void Node::set(
  Node * parentPtrIn,
  Play * playPtrIn,
  Node const * prevNodePtr)
{
  parentPtr = parentPtrIn;
  playPtr = playPtrIn;
  strats.reset();
  simpleStrats.reset();
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


void Node::getStrategies(
  const Distribution& dist,
  const bool debugFlag)
{
  // This method should only be used for an RHO node.

  // Find the distribution numbers that are still possible.
  const Survivors& survivors = dist.survivors(* playPtr);

  // Get the strategy from the following combination.
  strats = playPtr->combPtr->strategies();
  if (debugFlag)
    cout << strats.str("Strategy of next trick") << endl;

  // Renumber and rotate the strategy.
  strats.adapt(* playPtr, survivors);
  if (debugFlag)
    cout << strats.str("Adapted strategy of next trick", true);
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
    cout << strats.str("Crossing " + s + " strategy", false);
    // cout << strats.str("Crossing " + s + " strategy", true);
  }

  parentPtr->strats *= strats;

  if (debugFlag)
  {
    const string s = (level == LEVEL_RHO ? "partner" : "lead");
    cout << parentPtr->strats.str(
      "Cumulative " + s + " strategy after this trick", false);
      // "Cumulative " + s + " strategy after this trick", true);
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
    cout << strats.str("Adding " + s + " strategy", false);
    // cout << strats.str("Adding " + s + " strategy", true);
  }

  parentPtr->strats += strats;

  if (debugFlag)
  {
    const string s = (level == LEVEL_LEAD ? "overall" : "LHO");
    cout << parentPtr->strats.str(
      "Cumulative " + s + " strategy after this trick");
      // "Cumulative " + s + " strategy after this trick", true);
  }
}


bool Node::removePlay()
{
  if (strats.numDists() == 0)
    return true;
  else if (strats.size() == 0)
    return true;
  else if (strats.size() == 1)
  {
// cout << "parents->simpleStrats was\n";
// cout << parentPtr->simpleStrats.str();
// cout << "play is " << playPtr->strPartialTrick(LEVEL_LHO);
// cout << "The Node strats are\n";
// cout << strats.str();
    parentPtr->simpleStrats *= strats;
// cout << "simpleStrats is\n";
// cout << parentPtr->simpleStrats.str();
    assert(parentPtr->simpleStrats.size() == 1);
    return true;
  }
  else
    return false;
}


void Node::clearBounds()
{
  bounds.reset();
}


void Node::bound()
{
  strats.bound(bounds);
}


void Node::propagateBounds()
{
  parentPtr->bounds *= bounds;
}


void Node::augmentConstants(const Strategy& constants)
{
  bounds.constants *= constants;
}


void Node::constrainConstantsToMinima()
{
  bounds.minima.constrain(bounds.constants);
}


/*
void Node::getConstrainedParentMaxima(Strategy& max)
{
  // Get the parent maxima (who's your daddy now?) and keep the
  // ones that are <= the minima of the current node.  These are
  // candidates for purging, as the defenders will not enter this
  // node's strategy with the corresponding distribution.
  max = parentPtr->bounds.maxima;
cout << "Got parent maxima:\n";
cout << max.str();
cout << "Current node bounds:\n";
cout << bounds.str();
  bounds.minima.constrain(max);
}
*/


void Node::activateBounds()
{
  strats *= bounds.constants;
}


bool Node::purgeConstants()
{
  const unsigned sizeOld = strats.size();
  if (sizeOld == 0 || strats.numDists() == 0)
    return false;

  const auto& constants = parentPtr->bounds.constants;
// cout << "About to purge constants:\n";
// cout << constants.str();
// cout << "Strategies before are:\n";
// cout << strats.str();

  strats.purge(constants);
  bounds.minima.purge(constants);
  // bounds.maxima.purge(constants);
// cout << "Strategies after are:\n";
// cout << strats.str();
// cout << "Strat numbers " << sizeOld << " and now " << strats.size() << endl;

  const unsigned sizeNew = strats.size();

  // We only need to revisit a collapse to fewer, but still non-zero
  // strategies.
  /* */
  if (sizeNew == 0 || strats.numDists() == 0)
    return false;
  else
    return (strats.size() != sizeOld);
  /* */

  // return (strats.size() != sizeOld || strats.numDists() == 0);
}


void Node::makeRanges()
{
  // TODO Could potentially eliminate this and two next methods
  // by working with strategies()
  strats.makeRanges();
}


void Node::propagateRanges()
{
  parentPtr->strats.propagateRanges(strats);
}


void Node::purgeRanges()
{
  const unsigned sizeOld = strats.size();
  if (sizeOld == 0 || strats.numDists() == 0)
    return;

  strats.purgeRanges(parentPtr->strats);
}


Node * Node::getParentPtr()
{
  return parentPtr;
}


void Node::integrateSimpleStrategies()
{
// cout << "Simple strategy coming up\n";
// cout << simpleStrats.str();
  strats *= simpleStrats;
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


const Strategy& Node::constants() const
{
  return bounds.constants;
}


string Node::strBounds(const string& title) const
{
  return bounds.str(title);
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
  return simpleStrats.str("simple " + to_string(index));
}


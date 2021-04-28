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
  strats.reset();
}


void Node::set(
  Node * parentPtrIn,
  Play const * playPtrIn)
{
  parentPtr = parentPtrIn;
  playPtr = playPtrIn;
  strats.reset();
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
  if (debugFlag)
  {
    cout << Node::strPlay(level);

    string s;
    if (level == LEVEL_RHO)
      s = "RHO";
    else if (level == LEVEL_LHO)
      s = "LHO";
    else
      assert(false);

    cout << strats.str("Crossing " + s + "strategy", true);
  }

  parentPtr->strats *= strats;

  if (debugFlag)
  {
    string s;
    if (level == LEVEL_RHO)
      s = "partner";
    else if (level == LEVEL_LHO)
      s = "lead";
    else
      assert(false);

    cout << parentPtr->strats.str(
      "Cumulative " + s + " strategy after this trick", true);
  }
}


void Node::add(
  const Level level,
  const bool debugFlag)
{
  if (debugFlag)
  {
    cout << Node::strPlay(level);

    string s;
    if (level == LEVEL_PARD)
      s = "partner";
    else if (level == LEVEL_LEAD)
      s = "lead";
    else
      assert(false);

    cout << strats.str("Adding " + s + " strategy", true);
  }

  parentPtr->strats += strats;

  if (debugFlag)
  {
    string s;
    if (level == LEVEL_LEAD)
      s = "overall";
    else if (level == LEVEL_PARD)
      s = "LHO";

    cout << parentPtr->strats.str(
      "Cumulative " + s + " strategy after this trick", true);
  }
}


string Node::strPlay(const Level level) const
{
  assert(playPtr != nullptr);
  return playPtr->strPartialTrick(level);
}


string Node::str(
  const string& title,
  const bool rankFlag) const
{
  return strats.str(title, rankFlag);
}

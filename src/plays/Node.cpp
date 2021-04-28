#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Node.h"

#include "Play.h"


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
  strategies.reset();
}


void Node::set(
  Node * parentPtrIn,
  Play const * playPtrIn)
{
  parentPtr = parentPtrIn;
  playPtr = playPtrIn;
  strategies.reset();
}


void Node::operator *=(const Strategies strat2)
{
  strategies *= strat2;
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
  return strategies.str(title, rankFlag);
}

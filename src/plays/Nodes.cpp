#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Nodes.h"
#include "Play.h"
#include "Chunks.h"


Nodes::Nodes()
{
  Nodes::reset();
}


Nodes::~Nodes()
{
}


void Nodes::reset()
{
  level = LEVEL_SIZE;
  nodes.clear();
  chunkSize = 0;
  Nodes::clear();
}


void Nodes::clear()
{
  nextIter = nodes.begin();
  prevPtr = nullptr;
  prevPlayPtr = nullptr;
  nextEntryNumber = 0;
}


void Nodes::resize(
  const Level levelIn, 
  const unsigned cards)
{
  level = levelIn;

  const ChunkEntry& chunk = CHUNK_SIZE[cards];
  if (level == LEVEL_LEAD)
    chunkSize = chunk.lead;
  else if (level == LEVEL_LHO)
    chunkSize = chunk.lho;
  else if (level == LEVEL_PARD)
    chunkSize = chunk.pard;
  else if (level == LEVEL_RHO)
    chunkSize = chunk.rho;
  else
    assert(false);
  
  nodes.resize(chunkSize);
}


Node * Nodes::log(
  Node * parentPtr,
  Play * playPtr,
  bool& newFlag)
{
  // If the play doesn't differ in the relevant cards, no change.
  if (newFlag == false && 
      prevPlayPtr != nullptr &&
      playPtr->samePartial(* prevPlayPtr, level))
    return prevPtr;

  // If we have run out of space, make some more.
  if (nextIter == nodes.end())
    nextIter = nodes.insert(nextIter, chunkSize, Node());

  newFlag = true;
  prevPlayPtr = playPtr;

  Node& node = * nextIter;
  Node const * prevNodePtr = 
    (nextIter == nodes.begin() ? nullptr : &* prev(nextIter));
    
  nextIter++;
  nextEntryNumber++;

  node.set(parentPtr, playPtr, prevNodePtr);

  prevPtr = &node;
  return prevPtr;
}


list<Node>::iterator Nodes::begin()
{
  return nodes.begin();
}


list<Node>::iterator Nodes::end()
{
  // The end of the used nodes.
  return nextIter;
}


list<Node>::const_iterator Nodes::begin() const
{
  return nodes.begin();
}


list<Node>::const_iterator Nodes::end() const
{
  // The end of the used nodes.
  return nextIter;
}


void Nodes::removeAllLaterCollapses()
{
  // Look for rank collapses that happen "during the trick".
  // For example, with KJ975 missing 7 cards, if declarer leads the 5,
  // the trick might go 5 - T - 6 or 5 - T - x.  After the trick they will
  // be the same, but even during the trick declarer should not distinguish
  // between the 6 and the x.  We don't give up on the difference, but we
  // merge the strategies vector by vector, and not by cross product.
  // Declarer should not play differently based on a distinction that
  // the defense can create without a real difference.
  //
  // TODO Split into a general one for RHO only, and a void one
  // for LHO only.
  // TODO In order to hook this up instead of the corresponding
  // method in Plays, we first need to move Bounds to Nodes, I think.

  auto iter = nodes.begin();
  while (iter != nextIter)
  {
    auto& node = * iter;
    auto& play = node.play();

    if (! play.leadCollapse || play.trickNS)
    {
      // Skip plays that do not have a lead collapsing.
      // Also skip a trick won by declarer as the collapse cannot
      // involve two defenders' cards that are played in this trick.
      iter++;
      continue;
    }

    const unsigned lhoRank = play.lho();
    const unsigned rhoRank = play.rho();
    const unsigned leadRank = play.lead();
    const unsigned h3 = play.holding3;

    if (rhoRank+1 == leadRank)
    {
      // As the defenders win the trick:
      assert(lhoRank > leadRank);

      // Find matching RHO plays.  There can be more than one, as
      // we may have several plays of a given rank if that is what
      // we asked for from Ranks.  As the plays are in lexicographic
      // order, they will follow immediately.  As the LHO card may
      // be the only defenders' card that is exactly one rank above
      // the lead, there may be no matching plays.
      auto iter2 = next(iter);
      while (iter2 != nextIter &&
          iter2->play().holding3 == h3 &&
          iter2->play().lho() == lhoRank)
      {
        * iter |= * iter2;
        iter2 = nodes.erase(iter2);
      }
    }
    else if (lhoRank+1 == leadRank)
    {
      assert(rhoRank > leadRank);

      // Find matching LHO plays.  Unlike above, they will not be
      // in order following the current play.
      auto iter2 = next(iter);
      while (iter2 != nextIter)
      {
        if (iter2->play().holding3 == h3 &&
            iter2->play().lead() == leadRank &&
            iter2->play().rho() == rhoRank)
        {
          * iter |= * iter2;
          iter2 = nodes.erase(iter2);
        }
        else
          iter2++;
      }
    }
    iter++;
  }
}


unsigned Nodes::size() const
{
  return nodes.size();
}


unsigned Nodes::used() const
{
  return nextEntryNumber;
}


string Nodes::strCount() const
{
  stringstream ss;
  if (level == LEVEL_LEAD)
    ss << "Lead ";
  else if (level == LEVEL_LHO)
    ss << "LHO ";
  else if (level == LEVEL_PARD)
    ss << "Pard ";
  else if (level == LEVEL_RHO)
    ss << "RHO ";

  ss << nodes.size() << " " << nextEntryNumber << "\n";

  return ss.str();
}


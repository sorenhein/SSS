#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Nodes.h"
#include "Play.h"
#include "Chunks.h"

// TMP
#include "../stats/Timer.h"
extern vector<Timer> timersStrat;


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

timersStrat[17].start();
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
timersStrat[17].stop();
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


void Nodes::removeNodes()
{
  // Quite fast.
  auto iter = nodes.begin();
  while (iter != nextIter)
  {
    if (iter->removePlay())
    {
      iter = nodes.erase(iter);
      nextEntryNumber--;
    }
    else
      iter++;
  }
}


void Nodes::strategizeDeclarer(const bool debugFlag)
{
  assert(level == LEVEL_PARD || level == LEVEL_LEAD);

  // Add to the corresponding parent node.
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
    iter->add(LEVEL_PARD, debugFlag);
}


void Nodes::strategizeDeclarerAdvanced(const bool debugFlag)
{
  assert(level == LEVEL_PARD || level == LEVEL_LEAD);

  // Add back the simple strategies and the constants.
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
    iter->reactivate();

  Nodes::strategizeDeclarer(debugFlag);
}


void Nodes::strategizeDefenders(const bool debugFlag)
{
  assert(level == LEVEL_RHO || level == LEVEL_LHO);

  // Combine with the corresponding parent node by cross product.
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
    iter->cross(level, debugFlag);
}


void Nodes::strategizeDefendersAdvanced(const bool debugFlag)
{
  assert(level == LEVEL_RHO || level == LEVEL_LHO);

  // Derive bounds on RHO outcomes for each lead in order to find
  // constant or dominated outcomes, propagate them to the parent nodes 
  // (which are may be nodesLead if partner is void; see Plays), and 
  // remove them from the parent nodes.  This is quite fast.
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
  {
    iter->strategies().makeRanges();
    iter->propagateRanges();
  }

  // Remove constants into a separate strategy.
  //
  // Also, for a given lead and a given distribution, let's say the 
  // range of outcomes for a given defensive strategy is (min, max).  
  // Let's also say that the best global range is (MIN, MAX).  
  // "Best" means it has the lowest MAX, and for a given MAX it has the 
  // lowest MIN.
  // Then if MAX <= min and (max > min or MIN < MAX), the defenders will 
  // never enter that strategy with that distribution, so it can be 
  // removed from their options.
  // This has to be a separate loop, as all ranges have to propagate 
  // up first.
timersStrat[16].start();
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
    iter->purgeRanges();
timersStrat[16].stop();

  Nodes::removeNodes();

  if (debugFlag)
    cout << Nodes::strSimple();

  // Combine the plays into an overall strategy for each lead.
  // Note that the results may end up in nodesLead due to the relinking.
  Nodes::strategizeDefenders(debugFlag);
}


unsigned Nodes::size() const
{
  return nodes.size();
}


unsigned Nodes::used() const
{
  return nextEntryNumber;
}


string Nodes::strCountHeader() const
{
  stringstream ss;
  ss << 
    setw(6) << "Player" <<
    setw(8) << right << "Alloc" <<
    setw(8) << "Used" << "\n";

  return ss.str();
}


string Nodes::strCount() const
{
  stringstream ss;

  string p;
  if (level == LEVEL_LEAD)
    p = "Lead";
  else if (level == LEVEL_LHO)
    p = "LHO";
  else if (level == LEVEL_PARD)
    p = "Pard";
  else if (level == LEVEL_RHO)
    p = "RHO";

  ss << 
    setw(6) << left << p <<
    setw(8) << right << nodes.size() << 
    setw(8) << nextEntryNumber << "\n";

  return ss.str();
}


string Nodes::strSimple() const
{
  stringstream ss;
  for (auto& node: nodes)
    ss <<  node.strSimple();
  return ss.str();
}


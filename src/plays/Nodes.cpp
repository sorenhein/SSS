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


void Nodes::makeBounds(const bool debugFlag)
{
  // We currently only do this for RHO in Plays::strategizeVoid().
  // Derive bounds on the trick numbers for each play.
  // We can't iterate over (auto& node: nodes), as nodes is larger.
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
  {
// cout << "ABOUT TO BOUND\n";
// cout << iter->play().strPartialTrick(LEVEL_LHO);
// cout << iter->strategies().str();
    iter->bound();
    if (debugFlag)
    {
      // Only the right LEVEL if we start in the middle of the trick.
      // Could pass in level to do it properly.
      cout << iter->play().strPartialTrick(LEVEL_LHO);
      cout << iter->strBounds("Bounds");
    }
  }

  // Derive global bounds for each parent, ending up in nodesLead
  // in the case of RHO when partner is void, in nodesPard otherwise.
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
    iter->propagateBounds();

  // Only keep those constants (for a given parent play) that 
  // correspond to the minimum achievable outcome.
  Node * prevParentPtr = nullptr;
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
  {
    Node * parentPtr = iter->getParentPtr();
    if (parentPtr == prevParentPtr)
      continue;

// cout << "Parent node before constraining:\n";
// cout << parentPtr->play().strPartialTrick(LEVEL_LEAD);
// cout << parentPtr->strBounds("Constrained parent constants") << endl;
    prevParentPtr = parentPtr;
    parentPtr->constrainConstantsToMinima();
    if (debugFlag)
    {
      cout << "Parent node after constraining:\n";
      cout << parentPtr->play().strPartialTrick(LEVEL_LEAD);
      cout << parentPtr->strBounds("Constrained parent constants") <<
        endl;
    }
  }
}


void Nodes::makeBoundsSubset(
  list<ParentConstants>& parents,
  const bool debugFlag)
{
  // If we reduced the number of strategies in a node as a result of
  // purging the constants, we have to redo the bounds in that part of
  // the tree, i.e. for all nodes feeding into the parent as well as
  // for the parent itself.

  auto piter = parents.begin();
  auto iter = nodes.begin();
  while (true)
  {
    auto pptr = piter->parentPtr;
    while (iter != nextIter && iter->getParentPtr() != pptr) 
      iter++;

    while (iter != nextIter && iter->getParentPtr() == pptr)
    {
      iter->bound();
      iter->propagateBounds();

      if (debugFlag)
      {
        cout << "Redid bounds for " << 
          iter->play().strPartialTrick(LEVEL_LHO);
        cout << iter->strBounds("Bounds");
      }
      iter++;
    }

    // Clear the parent itself.
    pptr->clearBounds();

    if (++piter == parents.end())
      break;
  }

  for (auto& parent: parents)
  {
    auto parentPtr = parent.parentPtr;
    if (debugFlag)
    {
      cout << "Parent node before subset constraining:\n";
      cout << parentPtr->play().strPartialTrick(LEVEL_LEAD);
      cout << parentPtr->strBounds("Constrained parent constants") << endl;
    }

    parentPtr->constrainConstantsToMinima();

    if (debugFlag)
    {
      cout << "Parent node after subset constraining:\n";
      cout << parentPtr->play().strPartialTrick(LEVEL_LEAD);
      cout << parentPtr->strBounds("Constrained parent constants") <<
        endl;
    }
  }
}


void Nodes::removeConstants(const bool debugFlag)
{
  // Remove constant distributions (for a given lead) from each
  // play with that lead.  If a play strategy melts away completely,
  // remove it.  If there is only one strategy vector, also remove
  // it and put in a special simple set of strategies.

  list<ParentConstants> parents;

  while (true)
  {
    bool shrinkFlag = false;
    for (auto iter = nodes.begin(); iter != nextIter; iter++)
    {
      if (debugFlag)
      {
        cout << "purgeConstants loop: " <<
          iter->play().strPartialTrick(LEVEL_LHO);
      }

      if (iter->purgeConstants())
      {
        if (debugFlag)
          cout << "New instance of changed number of strategies\n";
        // Number of strategies shrank, so we have to redo the constants
        // for this group of strategies mapping the the same parent.
      
        Node * parentPtr = iter->getParentPtr();
        if (parents.empty() || parents.back().parentPtr != parentPtr)
        {
          // We have a new parent pointer to keep track of.
          if (debugFlag)
            cout << "New instance of affected parent\n";
          parents.emplace_back(ParentConstants());
          auto& pc = parents.back();
          pc.parentPtr = parentPtr;
          pc.constants = parentPtr->constants();
// cout << "Stored parent constants\n";
// cout <<pc.constants.str();
          shrinkFlag = true;
        }
      }
    }

    if (! shrinkFlag)
      break;

    // Clear the bounds for all play with affected parents.
    auto piter = parents.begin();
    auto pptr = piter->parentPtr;
    auto iter = nodes.begin();
    while (true)
    {
      while (iter != nextIter && iter->getParentPtr() != pptr)
        iter++;

      while (iter != nextIter &&iter->getParentPtr() == pptr)
      {
// cout << "Clearing " << iter->play().strPartialTrick(LEVEL_LHO);
        iter->clearBounds();
        iter++;
      }

      // Clear the parent itself.
// cout << "Clearing parent " << pptr->play().strPartialTrick(LEVEL_LEAD);
      pptr->clearBounds();

      if (++piter == parents.end())
        break;
    }

    // Now we have to redo makeBounds and propagateBounds
    Nodes::makeBoundsSubset(parents, debugFlag);
    
    // Finally we have to reinstate the parent constants we kept.
    for (auto& parent: parents)
    {
// cout << "Augmenting parent" << endl;
// cout << parent.parentPtr->play().strPartialTrick(LEVEL_LEAD);
// cout << parent.parentPtr->strBounds("Parent constants before") << endl;
// cout << "Augmenting with " << parent.constants.str();
      parent.parentPtr->augmentConstants(parent.constants);
// cout << parent.parentPtr->strBounds("Parent constants after") << endl;
    }
    parents.clear();
  }

  auto iter = nodes.begin();
  while (iter != nextIter)
  {
    if (iter->removePlay())
    {
      if (debugFlag)
      {
        cout << "Will remove play\n";
        cout << iter->play().strPartialTrick(LEVEL_LHO);
        cout << iter->strategies().str();
      }
      iter = nodes.erase(iter);
      nextEntryNumber--;
    }
    else
      iter++;
  }
}


void Nodes::removeDominatedDefenses(const bool debugFlag)
{
  // For a given lead and a given distribution, let's say the range of
  // outcomes for a given defensive strategy is (min, max).  Let's also
  // say that the lowest maximum that any strategy achieves is M.
  // Then if M <= min, the defenders will never enter that strategy
  // with that distribution, so it can be removed from their options.

  Strategy max;
  auto iter = nodes.begin();
  while (iter != nextIter)
  {
    if (debugFlag)
    {
      cout << "removeDominatedDefenses() loop\n";
      cout << iter->play().strPartialTrick(LEVEL_LHO);
    }
    auto& node = * iter;

    // Limit the maximum vector to those entries that are <= play.lower.
    node.getConstrainedParentMaxima(max);

    if (max.size() == 0)
    {
      // Nothing to purge.
      iter++;
      continue;
    }

    if (debugFlag)
       cout << "Got max: " << max.str() << endl;

    node.strategies().purge(max);
    // TODO Can this too shrink the number of strategies?

    if (node.removePlay())
    {
      if (debugFlag)
      {
        cout << "Will remove dominated defense\n";
        cout << iter->play().strPartialTrick(LEVEL_LHO);
        cout << iter->strategies().str();
      }
      iter = nodes.erase(iter);
      nextEntryNumber--;
    }
    else
      iter++;
  }

}


void Nodes::strategizeDeclarer(const bool debugFlag)
{
  // Add to the corresponding parent node.
  assert(level == LEVEL_PARD || level == LEVEL_LEAD);
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
    iter->add(LEVEL_PARD, debugFlag);
}


void Nodes::strategizeDeclarerAdvanced(const bool debugFlag)
{
  // Add back the simple strategies.
  assert(level == LEVEL_PARD || level == LEVEL_LEAD);
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
  {
// cout << "Before integrateSimple\n";
// cout << iter->play().strPartialTrick(LEVEL_LEAD);
// cout << iter->strategies().str();
    iter->integrateSimpleStrategies();
// cout << "After integrateSimple\n";
// cout << iter->strategies().str();
  }

  // Add back the lead-specific constants.
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
  {
// cout << "Before activateBounds\n";
// cout << iter->play().strPartialTrick(LEVEL_LEAD);
// cout << iter->strategies().str();
    iter->activateBounds();
// cout << "After activateBounds\n";
// cout << iter->strategies().str();
  }

  Nodes::strategizeDeclarer(debugFlag);
}


void Nodes::strategizeDefenders(const bool debugFlag)
{
  // Combine with the corresponding parent node by cross product.
  assert(level == LEVEL_RHO || level == LEVEL_LHO);
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
    iter->cross(level, debugFlag);
}


void Nodes::extractSimpleStrategies(const bool debugFlag)
{
  Nodes::removeConstants(debugFlag);
// cout << "Done removing constants\n";
  Nodes::removeDominatedDefenses(debugFlag);
// cout << "Done removing dominated defenses\n";
}


void Nodes::strategizeDefendersAdvanced(const bool debugFlag)
{
  // Derive bounds on RHO outcomes for each lead in order to find
  // constant outcomes, propagate them to the parent nodes (which are
  // may be nodesLead if partner is void; see Plays), and remove them 
  // from the parent nodes.
  assert(level == LEVEL_RHO || level == LEVEL_LHO);
  Nodes::makeBounds(debugFlag);

  // Remove the lead constants from the corresponding strategies.
  // Collect all strategies with a single vector into an overall strategy.
  // Some defenses can be removed -- see comment in method.
// cout << "Before extractSimpleStrategies\n";
  Nodes::extractSimpleStrategies(debugFlag);

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


string Nodes::strSimple() const
{
  stringstream ss;
  for (auto& node: nodes)
    ss <<  node.strSimple();
  return ss.str();
}


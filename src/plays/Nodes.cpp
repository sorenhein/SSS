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
cout << "ABOUT TO BOUND\n";
cout << iter->play().strPartialTrick(LEVEL_LHO);
cout << iter->strategies().str();
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

cout << "Parent node before constraining:\n";
cout << parentPtr->play().strPartialTrick(LEVEL_LEAD);
cout << parentPtr->strBounds("Constrained parent constants") << endl;
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


void Nodes::removeConstants()
{
  // Remove constant distributions (for a given lead) from each
  // play with that lead.  If a play strategy melts away completely,
  // remove it.  If there is only one strategy vector, also remove
  // it and put in a special simple set of strategies.
  auto iter = nodes.begin();
  while (iter != nextIter)
  {
cout << "removeConstants() loop\n";
cout << iter->play().strPartialTrick(LEVEL_LHO);
    iter->purgeConstants();

    if (iter->removePlay())
    {
cout << "Will remove play\n";
cout << iter->play().strPartialTrick(LEVEL_LHO);
cout << iter->strategies().str();
      iter = nodes.erase(iter);
      nextEntryNumber--;
    }
    else
      iter++;
  }
}


void Nodes::removeDominatedDefenses()
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
cout << "removeDominatedDefenses() loop\n";
cout << iter->play().strPartialTrick(LEVEL_LHO);
    auto& node = * iter;

    // Limit the maximum vector to those entries that are <= play.lower.
    node.getConstrainedParentMaxima(max);

    if (max.size() == 0)
    {
      // Nothing to purge.
      iter++;
      continue;
    }

cout << "Got max: " << max.str() << endl;

    node.strategies().purge(max);

    if (node.removePlay())
    {
cout << "Will remove dominated defense\n";
cout << iter->play().strPartialTrick(LEVEL_LHO);
cout << iter->strategies().str();
      iter = nodes.erase(iter);
      nextEntryNumber--;
    }
    else
      iter++;
  }

}


void Nodes::extractSimpleStrategies()
{
  Nodes::removeConstants();
cout << "Done removing constants\n";
  Nodes::removeDominatedDefenses();
cout << "Done removing dominated defenses\n";
}


void Nodes::removeCollapsesRHO()
{
  // Look for rank collapses that happen "during the trick".
  // For example, with KJ975 /void missing 7 cards, if declarer leads the 5,
  // the trick might go 5 - T - 6 or 5 - T - x.  After the trick they will
  // be the same, but even during the trick declarer should not distinguish
  // between the 6 and the x.  We don't give up on the difference, but we
  // merge the strategies vector by vector, and not by cross product.
  // Declarer should not play differently based on a distinction that
  // the defense can create without a real difference.

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

    // The test for the same pardRank is unnecessary if partner is
    // known to be void.  The method also works if partner has cards.
    const unsigned lhoRank = play.lho();
    const unsigned pardRank = play.pard();
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
          iter2->play().pard() == pardRank &&
          iter2->play().lho() == lhoRank)
      {
        iter->strategies() |= iter2->strategies();
        iter2 = nodes.erase(iter2);
        nextEntryNumber--;
      }
    }
    iter++;
  }
}


void Nodes::removeCollapsesVoidLHO()
{
  // This is similar to removeCollapsesRHO().  It is only used when
  // partner is void.

  auto iter = nodes.begin();
  while (iter != nextIter)
  {
    auto& node = * iter;
    auto& play = node.play();

    if (! play.leadCollapse || play.trickNS)
    {
      iter++;
      continue;
    }

    // We don't test for pardRank as it is always 0.
    const unsigned lhoRank = play.lho();
    const unsigned rhoRank = play.rho();
    const unsigned leadRank = play.lead();
    const unsigned h3 = play.holding3;

    if (lhoRank+1 == leadRank)
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
          iter->strategies() |= iter2->strategies();
          iter2 = nodes.erase(iter2);
          nextEntryNumber--;
        }
        else
          iter2++;
      }
    }
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
cout << "Before integrateSimple\n";
cout << iter->play().strPartialTrick(LEVEL_LEAD);
cout << iter->strategies().str();
    iter->integrateSimpleStrategies();
cout << "After integrateSimple\n";
cout << iter->strategies().str();
  }

  // Add back the lead-specific constants.
  for (auto iter = nodes.begin(); iter != nextIter; iter++)
  {
cout << "Before activateBounds\n";
cout << iter->play().strPartialTrick(LEVEL_LEAD);
cout << iter->strategies().str();
    iter->activateBounds();
cout << "After activateBounds\n";
cout << iter->strategies().str();
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
cout << "Before extractSimpleStrategies\n";
  Nodes::extractSimpleStrategies();

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


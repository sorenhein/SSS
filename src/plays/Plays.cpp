#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#include "Plays.h"
#include "Chunks.h"

#include "../Combinations.h"
#include "../Distribution.h"
#include "../Survivor.h"

#include "../ranks/Ranks.h"


Plays::Plays()
{
  playChunkSize = 0;
  Plays::clear();
}


Plays::~Plays()
{
}


void Plays::clear()
{
  nextPlaysIter = plays.begin();

  nodesLead.clear();
  nodesLho.clear();
  nodesPard.clear();
  nodesRho.clear();
  nodeMaster.reset();
}


void Plays::resize(const unsigned cardsIn)
{
  cards = cardsIn;

  playChunkSize = CHUNK_SIZE[cards].rho;
  plays.resize(playChunkSize);

  nodesLead.resize(LEVEL_LEAD, cardsIn);
  nodesLho.resize(LEVEL_LHO, cardsIn);
  nodesPard.resize(LEVEL_PARD, cardsIn);
  nodesRho.resize(LEVEL_RHO, cardsIn);
}


unsigned Plays::size() const
{
  return nodesRho.used();
}


void Plays::log(const Play& play)
{
  // We assume that Ranks does not go out of scope!

  // Make more play slots in chunks for efficiency.
  if (nextPlaysIter == plays.end())
    nextPlaysIter = plays.insert(nextPlaysIter, playChunkSize, Play());

  * nextPlaysIter = play;
  Play * playPtr = &* nextPlaysIter;
  nextPlaysIter++;

  bool newFlag = false;
  Node * leadNodePtr = nodesLead.log(&nodeMaster, playPtr, newFlag);
  Node * lhoNodePtr = nodesLho.log(leadNodePtr, playPtr, newFlag);
  Node * pardNodePtr = nodesPard.log(lhoNodePtr, playPtr, newFlag);
  (void) nodesRho.log(pardNodePtr, playPtr, newFlag);
}


void Plays::setCombPtrs(const Combinations& combinations)
{
  for (auto& nodeRho: nodesRho)
    nodeRho.setCombPtr(combinations);
}


void Plays::getStrategies(
  Distribution const * distPtr,
  const DebugPlay debugFlag)
{
  // For RHO nodes we have to populate the strategies first.
  const bool debug = ((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);
  for (auto& nodeRho: nodesRho)
    nodeRho.getStrategies(* distPtr, debug);
}


void Plays::strategizeRHO(const DebugPlay debugFlag)
{
  // Combine it with the corresponding partner node by cross product.
  const bool debug = ((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);
  for (auto& nodeRho: nodesRho)
    nodeRho.cross(LEVEL_RHO, debug);
}


void Plays::strategizePard(const DebugPlay debugFlag)
{
  // Add to the corresponding LHO node.
  const bool debug = ((debugFlag & DEBUGPLAY_PARD_DETAILS) != 0);
  for (auto& nodePard: nodesPard)
    nodePard.add(LEVEL_PARD, debug);
}


void Plays::strategizeLHO(const DebugPlay debugFlag)
{
  // Combine it with the corresponding lead node by cross product.
  const bool debug = ((debugFlag & DEBUGPLAY_LHO_DETAILS) != 0);
  for (auto& nodeLho: nodesLho)
    nodeLho.cross(LEVEL_LHO, debug);
}


void Plays::strategizeLead(const DebugPlay debugFlag)
{
  nodeMaster.reset();
  
  // Add up the lead strategies into an overall one.
  const bool debug = ((debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);
  for (auto& nodeLead: nodesLead)
    nodeLead.add(LEVEL_LEAD, debug);
}


void Plays::strategize(
  const Ranks& ranks,
  Distribution const * distPtr,
  Strategies& strategies,
  const DebugPlay debugFlag)
{
  // TODO
  // Tiered debugFlag

  // The plays are propagated backwards up to a strategy for the
  // entire trick.  When the defenders have the choice, strategies
  // are "multiplied" together.  The math for this is shown in 
  // the strategies/ files, but in general it creates more strategies
  // and the rank choices are made to the opponents' advantage,
  // so the defenders want to force declarer to use low ranks.
  // The choice is at the level of each distribution.
  // When declarer has the choice, the choice is at the level of
  // overall strategies, and these are "added" together.  Again,
  // the math is shown in the code, but the question is whether one
  // strategy dominates another (taking more tricks for some
  // distributions without taking fewer for others).
  //
  // This approach yields strategies where the defenders have 
  // "too much" choice, because they get to act after seeing 
  // declarer's plays up to that point.  Therefore the question is 
  // going to be whether the defenders can hold declarer to these
  // outcomes by spreading their probability mass well.
  // This will be examined subsequently.

  UNUSED(ranks);

  // Turn into a string method in Plays.
  // TODO If play is passed to Ranks, then this output will have
  // to be at the end of the method.
  if (debugFlag & DEBUGPLAY_NODE_COUNTS)
    cout << Plays::strNodeCounts();

  Plays::getStrategies(distPtr, debugFlag);
  Plays::strategizeRHO(debugFlag);
  Plays::strategizePard(debugFlag);
  Plays::strategizeLHO(debugFlag);
  Plays::strategizeLead(debugFlag);

  // TODO Can we pass out nodeMaster.strategies() directly?
  // Does it stay in scope?
  strategies = nodeMaster.strategies();
}


void Plays::studyRHO(
  Distribution const * distPtr,
  const DebugPlay debugFlag)
{
  unsigned playNo = 0;
  unsigned leadNo = 0;

  // TODO Do we make play() available from Node, or do we somehow
  // make a derived VoidNode from Node.
  unsigned leadLast = nodesRho.begin()->play().lead();
  const bool debug = ((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);

  rhoStudyNodes.resize(nodesRho.used());

  auto rhoStudyNextIter = rhoStudyNodes.begin();
  for (auto rhoIter = nodesRho.begin(); 
    rhoIter != nodesRho.end(); 
    rhoIter++, rhoStudyNextIter++, playNo++)
  {
    const auto& rhoNode = * rhoIter;
    auto& studyNode = * rhoStudyNextIter;
    const Play& play = rhoNode.play();

    assert(play.side == SIDE_NORTH);

    if (play.lead() != leadLast)
    {
      leadNo++;
      leadLast = play.lead();
    }

    studyNode.playPtr = &play;
    studyNode.playNo = playNo;
    studyNode.leadNo = leadNo;

assert(playNo == rhoNode.indexTMP());

// TODO This should work when we log void plays directly with
// lead as the parent of RHO.
// assert(leadNo == rhoNode.indexParent());

    studyNode.strategies = play.combPtr->strategies();
    
    // Renumber and rotate the strategy.
    const Survivors& survivors = distPtr->survivors(play);
    studyNode.strategies.adapt(play, survivors);

    if (debug)
      cout << studyNode.strategies.str("Strategies");
  }

  for (auto& studyNode: rhoStudyNodes)
  {
    studyNode.strategies.bound(studyNode.bounds);

    if (debug)
      cout << studyNode.bounds.str("Bounds " + to_string(studyNode.playNo));
  }

}


void Plays::studyGlobal(const DebugPlay debugFlag)
{
  // Derive global bounds across all plays.
  const bool debug = ((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);

  for (auto& nodeRho: rhoStudyNodesNew)
    nodeRho.node.propagateBounds();

  // Only keep those constants (for a given lead) that
  // correspond to the minimum achievable outcome.
  for (auto& nodeLead: nodesLead)
  {
    nodeLead.constrictConstantsToMinima();
    if (debug)
      cout << nodeLead.strBounds("Alt Constrained constants") << endl;
  }
}


bool Plays::removePlay(
  const Strategies& strategies,
  Strategies& simpleStrat) const
{
  if (strategies.numDists() == 0)
    return true;
  else if (strategies.size() == 0)
    return true;
  else if (strategies.size() == 1)
  {
    simpleStrat *= strategies;
    assert(simpleStrat.size() == 1);
    return true;
  }
  else
    return false;
}


void Plays::removeConstantsNew(vector<Strategies>& simpleStrats)
{
  // Remove constant distributions (for a given lead) from each 
  // play with that lead.  If a play strategy melts away completely,
  // remove it.  If there is only one strategy vector, also remove
  // it and put in a special simple set of strategies.

  auto iter = rhoStudyNodesNew.begin();
  while (iter != rhoStudyNodesNew.end())
  {
    auto& node = * iter;
    node.node.purgeConstants();

    const unsigned leadNo = node.node.indexParent();
    if (Plays::removePlay(node.node.strategies(), simpleStrats[leadNo]))
      iter = rhoStudyNodesNew.erase(iter);
    else
      iter++;
  }
}


void Plays::removeDominatedDefensesNew(
  vector<Strategies>& simpleStrats)
{
  // For a given lead and a given distribution, let's say the range of 
  // outcomes for a given defensive strategy is (min, max).  Let's also 
  // say that the lowest maximum that any strategy achieves is M.
  // Then if M <= min, the defenders will never enter that strategy
  // with that distribution, so it can be removed from their options.

  Strategy max;
  auto iter = rhoStudyNodesNew.begin();
  while (iter != rhoStudyNodesNew.end())
  {
    auto& node = * iter;

    // Limit the maximum vector to those entries that are <= play.lower.
    node.node.getConstrictedParentMaxima(max);

    if (max.size() == 0)
    {
      // Nothing to purge.
      iter++;
      continue;
    }

    node.node.purgeSpecific(max);

    const unsigned leadNo = node.node.indexParent();
    if (Plays::removePlay(node.node.strategies(), simpleStrats[leadNo]))
      iter = rhoStudyNodesNew.erase(iter);
    else
      iter++;
  }
}


void Plays::removeLaterCollapses()
{
  // Look for rank collapses that happen "during the trick".
  // For example, with KJ975 missing 7 cards, if declarer leads the 5,
  // the trick might go 5 - T - 6 or 5 - T - x.  After the trick they will
  // be the same, but even during the trick declarer should not distinguish
  // between the 6 and the x.  We don't give up on the difference, but we
  // merge the strategies vector by vector, and not by cross product.
  // Declarer should not play differently based on a distinction that
  // the defense can create without a real difference.

  auto iter = rhoStudyNodes.begin();
  while (iter != rhoStudyNodes.end())
  {
    auto& node = * iter;

    if (! node.playPtr->leadCollapse || node.playPtr->trickNS)
    {
      // Skip plays that do not have a lead collapsing.
      // Also skip a trick won by declarer as the collapse cannot
      // involve two defenders' cards that are played in this trick.
      iter++;
      continue;
    }

    const unsigned lhoRank = node.playPtr->lho();
    const unsigned rhoRank = node.playPtr->rho();
    const unsigned leadRank = node.playPtr->lead();
    const unsigned h3 = node.playPtr->holding3;

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
      while (iter2 != rhoStudyNodes.end() && 
          iter2->playPtr->holding3 == h3 &&
          iter2->playPtr->lho() == lhoRank)
      {
        iter->strategies |= iter2->strategies;
        iter2 = rhoStudyNodes.erase(iter2);
      }
    }
    else if (lhoRank+1 == leadRank)
    {
      assert(rhoRank > leadRank);

      // Find matching LHO plays.  Unlike above, they will not be
      // in order following the current play.
      auto iter2 = next(iter);
      while (iter2 != rhoStudyNodes.end())
      {
        if (iter2->playPtr->holding3 == h3 &&
            iter2->playPtr->lead() == leadRank &&
            iter2->playPtr->rho() == rhoRank)
        {
          iter->strategies |= iter2->strategies;
          iter2 = rhoStudyNodes.erase(iter2);
        }
        else
          iter2++;
      }
    }
    iter++;
  }
}


void Plays::strategizeVoid(
  Distribution const * distPtr,
  Strategies& strategies,
  const DebugPlay debugFlag)
{
  // The normal strategize() method also works for combinations
  // where partner is void.  But some of the most difficult,
  // exponentially exploding combinations arise in this way.
  // In particular, declarer may have roughly the same number of
  // cards as the defenders together, and the ranks alternate
  // without declarer having the ace; for example KJ975 missing
  // 7 cards.
  //
  // From an optimization point of view, both defenders can 
  // coordinate and play their cards without intrusion from dummy.
  // So there is really only one optimization step for both
  // defenders together and only one for declarer, and not two
  // each as in the general case.  This in itself does not reduce
  // complexity appreciably, but there are some ideas that are
  // perhaps expressed more naturally this way.

  if (debugFlag & DEBUGPLAY_NODE_COUNTS)
    cout << Plays::strNodeCounts();

  // We study the strategies in more detail before multiplying
  // and adding them together.  We start by deriving their minima
  // and maxima across distributions, as well as those distributions
  // that are constant within the set of strategies for a play.
  // The results go in rhoStudyNodes.
  // Plays::studyRHO(distPtr, debugFlag);

  const bool debug = ((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);

  Plays::getStrategies(distPtr, debugFlag);

  nodesRho.removeAllLaterCollapses();

  if (debugFlag & DEBUGPLAY_NODE_COUNTS)
  {
    cout << "Removed later collapses\n";
    cout << Plays::strNodeCounts();
  }

  // Set up the parallel nodes.  Should not be necessary once
  // Bounds migrate into Nodes.

  rhoStudyNodesNew.resize(nodesRho.used());
  auto iterNew = rhoStudyNodesNew.begin();

  for (auto rhoIter = nodesRho.begin(); rhoIter != nodesRho.end();
    rhoIter++, iterNew++)
  {
    iterNew->node = * rhoIter;
  }

  // Link RHO nodes directly with lead nodes.
  // Later on this will become a separate method.
  for (auto& nodeRhoNew: rhoStudyNodesNew)
    nodeRhoNew.node.linkRhoToLead();


  for (auto& nodeRhoNew: rhoStudyNodesNew)
  {
    // nodeRhoNew.node.strategies().bound(nodeRhoNew.bounds);
    nodeRhoNew.node.bound();

    if (debug)
      cout << nodeRhoNew.node.strBounds("Alt Bounds");
      // cout << nodeRhoNew.bounds.str("Alt Bounds");
    
    nodeRhoNew.node.bound();
  }

  // Then we derive the bounds for each lead separately.
  Plays::studyGlobal(debugFlag);

  // Remove those constants from the corresponding strategies.
  // Collect all strategies with a single vector into an overall vector.
  const unsigned numLeads = nodesLead.used();
  vector<Strategies> simpleStratsNew(numLeads);
  Plays::removeConstantsNew(simpleStratsNew);

  const bool debugRho = ((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);
  const bool debugLead = ((debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);

  // Some defenses can be removed -- see comment in method.
  Plays::removeDominatedDefensesNew(simpleStratsNew);

  if (debugRho)
  {
    cout << "Size New now " << rhoStudyNodesNew.size() << endl;
    for (unsigned s = 0; s < simpleStratsNew.size(); s++)
      cout << simpleStratsNew[s].str("simple New " + to_string(s));
  }

  // Combine the plays into an overall strategy for each lead.
  // TODO Use one of the strategize methods?
  const bool debugNew = ((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);
  for (auto& nodeRho: rhoStudyNodesNew)
    nodeRho.node.cross(LEVEL_RHO, debugNew);

  // Add back the lead-specific constants.
  for (auto& nodeLead: nodesLead)
    nodeLead.activateBounds();

  // TODO Could move propagate loop to here?

  // Combine the lead strategies into an overall strategy.
  // TODO Use one of the strategize methods?
  const bool debugNew2 = ((debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);
  for (auto& nodeLead: nodesLead)
    nodeLead.add(LEVEL_LEAD, debugNew2);

  strategies = nodeMaster.strategies();
  cout << strategies.str("Final strategy") << "\n";
  cout << endl;

  if (debugLead)
    cout << "Final size " << strategies.size() << endl;
  
  // So now we know for a given lead that certain distributions can
  // be factored out from the individual strategies: Those constants
  // that are also minima.

  // Manual combinations.
  /*
  Strategies tvs = playInfo[0].strategies;
  tvs *= playInfo[1].strategies;
  cout << tvs.str("Strategy 0 + 1") << "\n";

  tvs = playInfo[2].strategies;
  tvs *= playInfo[9].strategies;
  tvs *= playInfo[15].strategies;
  tvs *= playInfo[21].strategies;
  tvs *= playInfo[27].strategies;
  tvs *= playInfo[33].strategies;
  cout << tvs.str("Strategy 2 + 9 + 15 + 21 + 27 + 33") << "\n";

  tvs = playInfo[68].strategies;
  tvs *= playInfo[67].strategies;
  cout << tvs.str("Strategy 68 + 67") << "\n";
  
  strategies = playInfo[0].strategies;
  */

// assert(playInfo.size() > 0);
  // strategies = playInfo.front().strategies;
}


string Plays::strNodeCounts() const
{
  stringstream ss;

  ss << "Node counts:" << 
    nodesRho.strCount() <<
    nodesPard.strCount() <<
    nodesLho.strCount() <<
    nodesLead.strCount() << "\n";

  return ss.str();
}


string Plays::str() const
{
  stringstream ss;
  
  assert(nodesRho.begin() != nodesRho.end());

  ss << nodesRho.begin()->strPlayLineHeader();
  for (auto& nodeRho: nodesRho)
    ss << nodeRho.strPlayLine();

  return ss.str();
}


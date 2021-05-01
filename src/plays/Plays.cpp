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


void Plays::removeConstants(vector<Strategies>& simpleStrats)
{
  // Remove constant distributions (for a given lead) from each 
  // play with that lead.  If a play strategy melts away completely,
  // remove it.  If there is only one strategy vector, also remove
  // it and put in a special simple set of strategies.

  auto iter = nodesRho.begin();
  while (iter != nodesRho.end())
  {
    auto& node = * iter;
    node.purgeConstants();

    const unsigned leadNo = node.indexParent();
    if (Plays::removePlay(node.strategies(), simpleStrats[leadNo]))
      iter = nodesRho.erase(iter);
    else
      iter++;
  }
}


void Plays::removeDominatedDefenses(vector<Strategies>& simpleStrats)
{
  // For a given lead and a given distribution, let's say the range of 
  // outcomes for a given defensive strategy is (min, max).  Let's also 
  // say that the lowest maximum that any strategy achieves is M.
  // Then if M <= min, the defenders will never enter that strategy
  // with that distribution, so it can be removed from their options.

  Strategy max;
  auto iter = nodesRho.begin();
  while (iter != nodesRho.end())
  {
    auto& node = * iter;

    // Limit the maximum vector to those entries that are <= play.lower.
    node.getConstrictedParentMaxima(max);

    if (max.size() == 0)
    {
      // Nothing to purge.
      iter++;
      continue;
    }

    node.purgeSpecific(max);

    const unsigned leadNo = node.indexParent();
    if (Plays::removePlay(node.strategies(), simpleStrats[leadNo]))
      iter = nodesRho.erase(iter);
    else
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

  // When partner is void, both defenders can coordinate and play 
  // their cards without intrusion from dummy.  So there is really 
  // only one optimization step for both defenders together and only 
  // one for declarer, and not two each as in the general case.  
  // This in itself does not reduce complexity appreciably.

  if (debugFlag & DEBUGPLAY_NODE_COUNTS)
    cout << Plays::strNodeCounts("before void collapses");

  // Link RHO nodes directly with lead nodes, skipping partner and LHO.
  for (auto& nodeRhoNew: nodesRho)
    nodeRhoNew.linkRhoToLead();

  Plays::getStrategies(distPtr, debugFlag);

  // Combine some plays around a lead collapse, removing others
  // from the list of plays.  See comment in the method.
  nodesRho.removeAllLaterCollapses();

  if (debugFlag & DEBUGPLAY_NODE_COUNTS)
    cout << Plays::strNodeCounts("after void collapses");

  // Derive bounds on the trick numbers for each play.
  for (auto& node: nodesRho)
  {
    node.bound();
    if (debugFlag & DEBUGPLAY_RHO_DETAILS)
      cout << node.strBounds("Bounds");
  }

  // Derive global bounds for each lead, ending up in nodesLead.
  for (auto& nodeRho: nodesRho)
    nodeRho.propagateBounds();

  // Only keep those constants (for a given lead) that correspond to 
  // the minimum achievable outcome.
  for (auto& nodeLead: nodesLead)
  {
    nodeLead.constrictConstantsToMinima();
    if (debugFlag & DEBUGPLAY_RHO_DETAILS)
      cout << nodeLead.strBounds("Minimum-constrained lead constants") << 
        endl;
  }

  // Remove the lead constants from the corresponding strategies.
  // Collect all strategies with a single vector into an overall vector.
  vector<Strategies> simpleStrats(nodesLead.used());
  Plays::removeConstants(simpleStrats);

  // Some defenses can be removed -- see comment in method.
  Plays::removeDominatedDefenses(simpleStrats);

  if (debugFlag & DEBUGPLAY_RHO_DETAILS)
  {
    for (unsigned s = 0; s < simpleStrats.size(); s++)
      cout << simpleStrats[s].str("simple " + to_string(s));
  }

  // Combine the plays into an overall strategy for each lead.
  // Note that the results end up in nodesLead due to the relinking.
  Plays::strategizeRHO(debugFlag);

  // Add back the lead-specific constants.
  for (auto& nodeLead: nodesLead)
    nodeLead.activateBounds();

  // TODO Don't we have to add simpleStrats back in?!

  // Combine the lead strategies into an overall strategy.
  Plays::strategizeLead(debugFlag);
  strategies = nodeMaster.strategies();

  if (debugFlag & DEBUGPLAY_LEAD_DETAILS)
  {
    cout << strategies.str("Final strategy") << "\n\n";
    cout << "Final size " << strategies.size() << endl;
  }


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
}


string Plays::strNodeCounts(const string& title) const
{
  stringstream ss;

  ss << "Node counts " << title << ":" << 
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


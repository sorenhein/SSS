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


void Plays::clearStrategies()
{
  for (auto& node: nodesLead)
    node.resetStrategies();
  for (auto& node: nodesLho)
    node.resetStrategies();
  for (auto& node: nodesPard)
    node.resetStrategies();
  for (auto& node: nodesRho)
    node.resetStrategies();
  nodeMaster.resetStrategies();
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


void Plays::getNextStrategies(
  Distribution const * distPtr,
  const DebugPlay debugFlag)
{
  // For RHO nodes we have to populate the strategies first.
  const bool debug = ((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);
  for (auto& nodeRho: nodesRho)
    nodeRho.getNextStrategies(* distPtr, debug);
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

  Plays::getNextStrategies(distPtr, debugFlag);

  if (debugFlag & DEBUGPLAY_NODE_COUNTS)
    cout << Plays::strNodeCounts();

  nodesRho.strategizeDefenders((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);
  nodesPard.strategizeDeclarer((debugFlag & DEBUGPLAY_PARD_DETAILS) != 0);
  nodesLho.strategizeDefenders((debugFlag & DEBUGPLAY_LHO_DETAILS) != 0);
  nodesLead.strategizeDeclarer((debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);

  // TODO Can we pass out nodeMaster.strategies() directly?
  // Does it stay in scope?
  strategies = nodeMaster.strategies();
}


void Plays::strategizeNew(
  const Ranks& ranks,
  Distribution const * distPtr,
  Strategies& strategies,
  const DebugPlay debugFlag)
{
  UNUSED(ranks);

  Plays::getNextStrategies(distPtr, debugFlag);

  if (debugFlag & DEBUGPLAY_NODE_COUNTS)
    cout << Plays::strNodeCounts("Node counts");

  nodesRho.strategizeDefenders((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);
  nodesPard.strategizeDeclarer((debugFlag & DEBUGPLAY_PARD_DETAILS) != 0);

  nodesLho.strategizeDefendersAdvanced((debugFlag & DEBUGPLAY_LHO_DETAILS) != 0);

  nodesLead.strategizeDeclarerAdvanced((debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);

  // TODO Can we pass out nodeMaster.strategies() directly?
  // Does it stay in scope?
  strategies = nodeMaster.strategies();
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

  // Link RHO nodes directly with lead nodes, skipping partner and LHO.
  for (auto& nodeRhoNew: nodesRho)
    nodeRhoNew.linkRhoToLead();

  Plays::getNextStrategies(distPtr, debugFlag);

  if (debugFlag & DEBUGPLAY_NODE_COUNTS)
    cout << Plays::strNodeCounts("Void node counts");

  nodesLho.strategizeDefendersAdvanced(
    (debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);

  nodesLead.strategizeDeclarerAdvanced(
    (debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);

/*
  // Derive bounds on RHO outcomes for each lead in order to find
  // constant outcomes, propagate them to the parent nodes (which are 
  // nodesLead in this case), and remove them from the parent nodes.
  nodesRho.makeBounds((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);

  // Remove the lead constants from the corresponding strategies.
  // Collect all strategies with a single vector into an overall strategy.
  // Some defenses can be removed -- see comment in method.
  nodesRho.extractSimpleStrategies(
    (debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);

  if (debugFlag & DEBUGPLAY_RHO_DETAILS)
    cout << nodesLead.strSimple();

  // Combine the plays into an overall strategy for each lead.
  // Note that the results end up in nodesLead due to the relinking.
  nodesRho.strategizeDefenders((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);

  // Add back the simple strategies.
  for (auto& nodeLead: nodesLead)
    nodeLead.integrateSimpleStrategies();

  // Add back the lead-specific constants.
  for (auto& nodeLead: nodesLead)
    nodeLead.activateBounds();

  // Combine the lead strategies into an overall strategy.
  nodesLead.strategizeDeclarer((debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);
*/

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

  ss << nodesRho.begin()->play().strHeader();
  for (auto& nodeRho: nodesRho)
    ss << nodeRho.play().strLine();

  return ss.str();
}


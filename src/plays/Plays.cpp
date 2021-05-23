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


const Strategies& Plays::strategizeSimple(const DebugPlay debugFlag)
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

  nodesRho.strategizeDefenders((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);
  nodesPard.strategizeDeclarer((debugFlag & DEBUGPLAY_PARD_DETAILS) != 0);

  nodesLho.strategizeDefenders((debugFlag & DEBUGPLAY_LHO_DETAILS) != 0);
  nodesLead.strategizeDeclarer((debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);

  return nodeMaster.strategies();
}


const Strategies& Plays::strategizeAdvanced(const DebugPlay debugFlag)
{
  nodesRho.strategizeDefenders((debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);
  nodesPard.strategizeDeclarer((debugFlag & DEBUGPLAY_PARD_DETAILS) != 0);

  // Here we deal separately with constants and dominated plays.
  nodesLho.strategizeDefendersAdvanced(
    (debugFlag & DEBUGPLAY_LHO_DETAILS) != 0);

  nodesLead.strategizeDeclarerAdvanced(
    (debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);

  return nodeMaster.strategies();
}


const Strategies& Plays::strategizeVoid(const DebugPlay debugFlag)
{
  // When partner is void, both defenders can coordinate and play 
  // their cards without intrusion from dummy.  So there is really 
  // only one optimization step for both defenders together and only 
  // one for declarer, and not two each as in the general case.  
  // We can save a bit of data shuffling.

  // Link RHO nodes directly with lead nodes, skipping partner and LHO.
  for (auto& nodeRhoNew: nodesRho)
    nodeRhoNew.linkRhoToLead();

  nodesRho.strategizeDefendersAdvanced(
    (debugFlag & DEBUGPLAY_RHO_DETAILS) != 0);

  nodesLead.strategizeDeclarerAdvanced(
    (debugFlag & DEBUGPLAY_LEAD_DETAILS) != 0);

  return nodeMaster.strategies();
}


const Strategies& Plays::strategize(
  Distribution const * distPtr,
  const DebugPlay debugFlag)
{
  if (debugFlag & DEBUGPLAY_NODE_COUNTS)
    cout << Plays::strNodeCounts();

  Plays::getNextStrategies(distPtr, debugFlag);

  if (nodesRho.used() == nodesLho.used())
  {
    // Partner is void.  Empirically it is a bit faster only to
    // optimize when there is a large number of plays.
    if (nodesRho.used() <= 30)
    {
      return Plays::strategizeSimple(debugFlag);
    }
    else
    {
      // Link RHO nodes directly with lead nodes, skipping partner and LHO.
      for (auto& nodeRhoNew: nodesRho)
        nodeRhoNew.linkRhoToLead();
      
      return Plays::strategizeVoid(debugFlag);
    }
  }
  else if (nodesRho.used() <= 20)
  {
    // Same principle, slightly different threshold.
    return Plays::strategizeSimple(debugFlag);
  }
  else
  {
    return Plays::strategizeAdvanced(debugFlag);
  }
}


string Plays::strNodeCounts(const string& title) const
{
  stringstream ss;

  ss << "Node counts " << title << "\n" << 
    nodesRho.strCountHeader() <<
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


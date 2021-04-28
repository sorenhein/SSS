#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#include "Plays.h"

#include "../Combinations.h"
#include "../Distribution.h"
#include "../Survivor.h"

#include "../ranks/Ranks.h"

// It's not that important to get these exactly right, but this works
// For efficiency the lists of nodes are not cleared (for a given
// number of cards), and only a pointer to the last used element
// and its number are changed.  The lists themselves are reset once
// for each number of cards.  Therefore the lists will generally
// have more entries than are used, and have to iterate up to
// the stored end iterator and not up to end().

const vector<ChunkEntry> CHUNK_SIZE =
{
  {  1,   1,   1,   1}, //  0
  {  1,   1,   1,   1}, //  1
  {  1,   1,   1,   1}, //  2
  {  1,   1,   1,   1}, //  3
  {  4,   6,   6,  12}, //  4
  {  4,   8,   8,  16}, //  5
  {  6,  12,  12,  16}, //  6
  {  6,  12,  16,  32}, //  7
  {  8,  12,  20,  40}, //  8
  { 10,  18,  24,  72}, //  9
  { 10,  20,  32, 100}, // 10
  { 12,  24,  48, 144}, // 11
  { 12,  28,  60, 208}, // 12
  { 12,  36,  80, 300}, // 13
  { 12,  42, 100, 432}, // 14
  { 14,  50, 128, 600}  // 15
};


Plays::Plays()
{
  Plays::reset();
}


Plays::~Plays()
{
}


void Plays::reset()
{
  sidePrev = SIDE_NORTH; // Doesn't matter -- lead will be different
  leadPrev = numeric_limits<unsigned>::max();
  lhoPrev = numeric_limits<unsigned>::max();
  pardPrev = numeric_limits<unsigned>::max();

  leadNext = 0;
  lhoNext = 0;
  pardNext = 0;
  rhoNext = 0;

  leadNextIter = leadNodes.begin();
  lhoNextIter = lhoNodes.begin();
  pardNextIter = pardNodes.begin();
  rhoNextIter = rhoNodes.begin();

  leadPrevPtr = nullptr;
  lhoPrevPtr = nullptr;
  pardPrevPtr = nullptr;

  // TODO Add in chunks and leave in place
  plays.clear();

  nodesLead.clear();
  nodesLho.clear();
  nodesPard.clear();
  nodesRho.clear();
}


void Plays::resize(const unsigned cardsIn)
{
  cards = cardsIn;
  chunk = CHUNK_SIZE[cards];

  leadNodes.resize(chunk.lead);
  lhoNodes.resize(chunk.lho);
  pardNodes.resize(chunk.pard);
  rhoNodes.resize(chunk.rho);

  nodesLead.resize(LEVEL_LEAD, cardsIn);
  nodesLho.resize(LEVEL_LHO, cardsIn);
  nodesPard.resize(LEVEL_PARD, cardsIn);
  nodesRho.resize(LEVEL_RHO, cardsIn);
}


unsigned Plays::size() const
{
  return rhoNext;
}


Plays::LeadNode * Plays::logLead(
  const Play& play,
  bool& newFlag)
{
  // TODO Could we just make these right to begin with?
  const SidePosition side = (play.side == POSITION_NORTH ? SIDE_NORTH : SIDE_SOUTH);
  const unsigned lead = play.leadPtr->getRank();
// TODO Move to lead(true), so the number.  Call the variable
// something with number rather than lead.
assert(lead == play.lead());

  // We use the fact that plays arrive in order.
  if (side == sidePrev && lead == leadPrev)
  {
    newFlag = false;
    return leadPrevPtr;
  }

  if (leadNextIter == leadNodes.end())
    leadNextIter = leadNodes.insert(leadNextIter, chunk.lead, LeadNode());

  newFlag = true;
  sidePrev = side;
  leadPrev = lead;

  LeadNode& node = * leadNextIter;
  leadNext++;
  leadNextIter++;

  node.side = side;
  node.lead = lead;
  node.strategies.reset();

  leadPrevPtr = &node;
  return leadPrevPtr;
}


Plays::LhoNode * Plays::logLho(
  const Play& play,
  LeadNode * leadPtr,
  bool& newFlag)
{
  const unsigned lho = play.lhoPtr->getRank();
// TODO Move to lho(true).
assert(lho == play.lho());
  if (newFlag == false && lho == lhoPrev)
    return lhoPrevPtr;
  
  if (lhoNextIter == lhoNodes.end())
    lhoNextIter = lhoNodes.insert(lhoNextIter, chunk.lho, LhoNode());

  newFlag = true;
  lhoPrev = lho;

  LhoNode& node = * lhoNextIter;
  lhoNext++;
  lhoNextIter++;

  node.lho = lho;
  node.leadPtr = leadPtr;
  node.strategies.reset();

  lhoPrevPtr = &node;
  return lhoPrevPtr;
}


Plays::PardNode * Plays::logPard(
  const Play& play,
  LhoNode * lhoPtr,
  bool& newFlag)
{
  const unsigned pard = play.pardPtr->getRank();
// TODO Move to pard(true).
assert(pard == play.pard());
  if (newFlag == false && pard == pardPrev)
    return pardPrevPtr;

  if (pardNextIter == pardNodes.end())
    pardNextIter = pardNodes.insert(pardNextIter, chunk.pard, PardNode());

  newFlag = true;
  pardPrev = pard;

  PardNode& node = * pardNextIter;
  pardNext++;
  pardNextIter++;

  node.pard = pard;
  node.lhoPtr = lhoPtr;
  node.strategies.reset();

  pardPrevPtr = &node;
  return pardPrevPtr;
}


void Plays::logRho(
  const Play& play,
  PardNode * pardPtr)
{
  if (rhoNextIter == rhoNodes.end())
    rhoNextIter = rhoNodes.insert(rhoNextIter, chunk.rho, RhoNode());

  RhoNode& node = * rhoNextIter;
  rhoNext++;
  rhoNextIter++;

  node.pardPtr = pardPtr;

  // TMP
  node.play = play;
}


void Plays::log(const Play& play)
{
  // The pointers assume that the Ranks object still exists!

  // The new way.  TODO In chunks, too.
  plays.push_back(play); 
  Play& playLogged = plays.back();

  bool newFlag2 = false;
  Node * leadNodePtr = nodesLead.log(&nodeMaster, &playLogged, newFlag2);
  Node * lhoNodePtr = nodesLho.log(leadNodePtr, &playLogged, newFlag2);
  Node * pardNodePtr = nodesPard.log(lhoNodePtr, &playLogged, newFlag2);
  (void) nodesRho.log(pardNodePtr, &playLogged, newFlag2);

  // The old way.
  bool newFlag;
  LeadNode * leadPtr = Plays::logLead(play, newFlag);
  LhoNode * lhoPtr = Plays::logLho(play, leadPtr, newFlag);
  PardNode * pardPtr = Plays::logPard(play, lhoPtr, newFlag);

  Plays::logRho(play, pardPtr);

  assert(rhoNodes.size() == nodesRho.size());
  assert(pardNodes.size() == nodesPard.size());
  assert(lhoNodes.size() == nodesLho.size());
  assert(leadNodes.size() == nodesLead.size());

  if (rhoNext != nodesRho.used())
  {
    assert(rhoNext == nodesRho.used());
  }
  if (pardNext != nodesPard.used())
  {
    assert(pardNext == nodesPard.used());
  }
  if (lhoNext != nodesLho.used())
  {
    assert(lhoNext == nodesLho.used());
  }
  if (leadNext != nodesLead.used())
  {
    assert(leadNext == nodesLead.used());
  }
}


void Plays::setCombPtrs(const Combinations& combinations)
{
  // TODO That's probably too many -- rhoNextIter
  for (auto& rhoNode: rhoNodes)
    rhoNode.play.combPtr = 
      combinations.getPtr(rhoNode.play.cardsLeft, rhoNode.play.holding3);
  
  for (auto& nodeRho: nodesRho)
    nodeRho.setCombPtr(combinations);
}


void Plays::strategizeRHO(
  Distribution const * distPtr,
  const bool debugFlag)
{
  Strategies tvs;
  unsigned rno = 0;

  for (auto rhoIter = rhoNodes.begin(); rhoIter != rhoNextIter; 
      rhoIter++, rno++)
  {
    const auto& rhoNode = * rhoIter;
    if (debugFlag)
      // TODO Maybe add argument "RHO"
      // Actually even more semantics: If RHO,
      // Complete trick: a b c d
      // If pard:
      // Partial trick: a b c
      cout << rhoNode.play.strTrick(rno);

    // Find the distribution numbers that are still possible.
    const Survivors& survivors = distPtr->survivors(rhoNode.play);

    // Get the strategy from the following combination.  
    tvs = rhoNode.play.combPtr->strategies();
    if (debugFlag)
      cout << tvs.str("Strategy of next trick") << endl;

    // Renumber and rotate the strategy.
    tvs.adapt(rhoNode.play, survivors);
    if (debugFlag)
      cout << tvs.str("Adapted strategy of next trick", true);

    // Combine it with the partner node by cross product.
    rhoNode.pardPtr->strategies *= tvs;
    if (debugFlag)
      cout << rhoNode.pardPtr->strategies.str(
        "Cumulative partner strategy after this trick", true);
  }


  rno = 0;
  for (auto& nodeRho: nodesRho)
  {
    // For RHO nodes we have to populate the strategies first.
    nodeRho.getStrategies(* distPtr, debugFlag);

    // This prefixes the output in Node::cross.
    if (debugFlag)
      cout << "Play #" << rno << ", ";

    // Combine it with the partner node by cross product.
    nodeRho.cross(LEVEL_RHO, debugFlag);
    
    rno++;
  }
}


void Plays::strategizePard(const bool debugFlag)
{
  unsigned pno = 0;

  for (auto pardIter = pardNodes.begin(); pardIter != pardNextIter; 
      pardIter++, pno)
  {
    const auto& pardNode = * pardIter;
    if (debugFlag)
      // TODO See above
      cout << "pard node for " << pardNode.pard << endl;

    // This is the partner strategy.
    if (debugFlag)
      cout << pardNode.strategies.str("Adding partner strategy", true);

    // Add it to the LHO node.
    pardNode.lhoPtr->strategies += pardNode.strategies;
    if (debugFlag)
      cout << pardNode.lhoPtr->strategies.str(
        "Cumulative LHO strategy after this addition", true);
  }


  // Add to the corresponding LHO node.
  for (auto& nodePard: nodesPard)
    nodePard.add(LEVEL_PARD, debugFlag);
}


void Plays::strategizeLHO(const bool debugFlag)
{
  unsigned lno = 0;

  for (auto lhoIter = lhoNodes.begin(); lhoIter != lhoNextIter; 
      lhoIter++, lno)
  {
    const auto& lhoNode = * lhoIter;
    if (debugFlag)
      // TODO
      cout << "LHO node for " << lhoNode.lho << endl;

    // This is the LHO strategy.
    if (debugFlag)
      cout << lhoNode.strategies.str("Adding LHO strategy", true) << endl;

    // Combine the LHO strategy with the lead node by cross product.
    lhoNode.leadPtr->strategies *= lhoNode.strategies;
    if (debugFlag)
      cout << lhoNode.leadPtr->strategies.str(
        "Cumulative lead strategy after this multiplication", true);
  }


  // Combine it with the corresponding lead node by cross product.
  for (auto& nodeLho: nodesLho)
    nodeLho.cross(LEVEL_LHO, debugFlag);
}


void Plays::strategizeLead(
  Strategies& strategies,
  const bool debugFlag)
{
  strategies.reset();
  unsigned lno = 0;

  for (auto ldIter = leadNodes.begin(); ldIter != leadNextIter; 
      ldIter++, lno++)
  {
    const auto& leadNode = * ldIter;
    if (debugFlag)
      // TODO
      cout << "Lead node for " << leadNode.side << " | " << leadNode.lead << endl;

    // This is the lead strategy.
    if (debugFlag)
      cout << leadNode.strategies.str("Adding LHO strategy", true) << endl;
      
    // Add it to the overall strategy.
    strategies += leadNode.strategies;
    if (debugFlag)
      cout << strategies.str(
        "Cumulative lead strategy after this addition", true);
  }


  nodeMaster.reset();
  
  // Add up the lead strategies into an overall one.
  for (auto& nodeLead: nodesLead)
    nodeLead.add(LEVEL_LEAD, debugFlag);
}


void Plays::strategize(
  const Ranks& ranks,
  Distribution const * distPtr,
  Strategies& strategies,
  bool debugFlag)
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

  // TODO Maybe tiered debugFlag to control the amount of info.
  // (debugVector & DEBUG_RHO)
  UNUSED(ranks);

  // Turn into a string method in Plays.
  // TODO If play is passed to Ranks, then this output will have
  // to be at the end of the method.
  cout << "Node counts:" << endl;
  cout << nodesRho.strCount();
  cout << nodesPard.strCount();
  cout << nodesLho.strCount();
  cout << nodesLead.strCount();

  strategizeRHO(distPtr, debugFlag);
  strategizePard(debugFlag);
  strategizeLHO(debugFlag);
  strategizeLead(strategies, debugFlag);

  assert(strategies.size() == nodeMaster.strategies().size());
  assert(strategies == nodeMaster.strategies());
}


unsigned Plays::studyRHO(
  Distribution const * distPtr,
  const bool debugFlag)
{
  
  unsigned playNo = 0;
  unsigned leadNo = 0;
  unsigned leadLast = rhoNodes.front().play.lead();

  // TODO Just add them one by one?  Then we don't need NextIter
  //  and we can use end().
  rhoStudyNodes.resize(rhoNext);
  auto rhoStudyNextIter = rhoStudyNodes.begin();
  for (auto rhoIter = rhoNodes.begin(); 
    rhoIter != rhoNextIter; 
    rhoIter++, rhoStudyNextIter++, playNo++)
  {
    const auto& rhoNode = * rhoIter;
    auto& studyNode = * rhoStudyNextIter;
    assert(rhoNode.play.side == SIDE_NORTH);

    if (rhoNode.play.lead() != leadLast)
    {
      leadNo++;
      leadLast = rhoNode.play.lead();
    }

    studyNode.playPtr = &rhoNode.play;
    studyNode.playNo = playNo;
    studyNode.leadNo = leadNo;

    studyNode.strategies = rhoNode.play.combPtr->strategies();
    
    // Renumber and rotate the strategy.
    const Survivors& survivors = distPtr->survivors(rhoNode.play);
    studyNode.strategies.adapt(rhoNode.play, survivors);

    studyNode.strategies.bound(studyNode.bounds);

    if (debugFlag)
    {
      cout << studyNode.strategies.str("Strategies");
      cout << studyNode.bounds.str("Play " + to_string(studyNode.playNo));
    }
  }

  return leadNo+1;
}


void Plays::studyGlobal(
  vector<Bounds>& boundsLead,
  const bool debugFlag)
{
  // Derive global bounds across all plays.

  for (auto& node: rhoStudyNodes)
  {
    const unsigned leadNo = node.leadNo;
    boundsLead[leadNo].minima *= node.bounds.minima;
    boundsLead[leadNo].maxima *= node.bounds.maxima;
    boundsLead[leadNo].constants *= node.bounds.constants;
  }

  // Only keep those constants (for a given lead) that
  // correspond to the minimum achievable outcome.
  unsigned leadNo = 0;
  for (auto& bound: boundsLead)
  {
    bound.minima.constrict(bound.constants);

    if (debugFlag)
      cout << bound.constants.str("Constrained constants " +
        to_string(leadNo)) << endl;

    leadNo++;
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


void Plays::removeConstants(
  const vector<Bounds>& boundsLead,
  vector<Strategies>& simpleStrats)
{
  // Remove constant distributions (for a given lead) from each 
  // play with that lead.  If a play strategy melts away completely,
  // remove it.  If there is only one strategy vector, also remove
  // it and put in a special simple set of strategies.

  auto iter = rhoStudyNodes.begin();
  while (iter != rhoStudyNodes.end())
  {
    auto& node = * iter;
    const unsigned leadNo = node.leadNo;
    const auto& constants = boundsLead[leadNo].constants;

    node.strategies.purge(constants);
    node.bounds.minima.purge(constants);
    node.bounds.maxima.purge(constants);

    if (Plays::removePlay(node.strategies, simpleStrats[leadNo]))
      iter = rhoStudyNodes.erase(iter);
    else
      iter++;
  }
}


void Plays::removeDominatedDefenses(
  const vector<Bounds>& boundsLead,
  vector<Strategies>& simpleStrats)
{
  // For a given lead and a given distribution, let's say the range of 
  // outcomes for a given defensive strategy is (min, max).  Let's also 
  // say that the lowest maximum that any strategy achieves is M.
  // Then if M <= min, the defenders will never enter that strategy
  // with that distribution, so it can be removed from their options.

  auto iter = rhoStudyNodes.begin();
  while (iter != rhoStudyNodes.end())
  {
    auto& node = * iter;

    // Limit the maximum vector to those entries that are <= play.lower.
    Strategy max = boundsLead[node.leadNo].maxima;
    node.bounds.minima.constrict(max);
    if (max.size() == 0)
    {
      // Nothing to purge.
      iter++;
      continue;
    }

    node.strategies.purge(max);

    if (Plays::removePlay(node.strategies, simpleStrats[node.leadNo]))
      iter = rhoStudyNodes.erase(iter);
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
  const bool debugFlag)
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

cout << "Void node counts:" << endl;
cout << "RHO " << rhoNodes.size() << " " << rhoNext << endl;
cout << "Pard " << pardNodes.size() << " " << pardNext << endl;
cout << "LHO " << lhoNodes.size() << " " << lhoNext << endl;
cout << "Lead " << leadNodes.size() << " " << leadNext << endl;

  // We study the strategies in more detail before multiplying
  // and adding them together.  We start by deriving their minima
  // and maxima across distributions, as well as those distributions
  // that are constant within the set of strategies for a play.
  // The results go in rhoStudyNodes.
  const unsigned numLeads = Plays::studyRHO(distPtr, debugFlag);

  // Then we derive the bounds for each lead separately.
  vector<Bounds> boundsLead(numLeads);
  Plays::studyGlobal(boundsLead, debugFlag);

  // Remove those constants from the corresponding strategies.
  // Collect all strategies with a single vector into an overall vector.
  vector<Strategies> simpleStrats(numLeads);
  Plays::removeConstants(boundsLead, simpleStrats);

  cout << "Size now " << rhoStudyNodes.size() << endl;
  unsigned lno = 0;
  for (auto s: simpleStrats)
  {
    cout << "simpleStrats for lead number " << lno << endl;
    cout << s.str("simpleStrats");
    lno++;
  }

  // Some defenses can be removed -- see comment in method.
  Plays::removeDominatedDefenses(boundsLead, simpleStrats);

  cout << "Size now " << rhoStudyNodes.size() << endl;
  for (unsigned s = 0; s < simpleStrats.size(); s++)
    cout << simpleStrats[s].str("simple " + to_string(s));

  // Declarer should not get too clever about some defensive plays.
  Plays::removeLaterCollapses();

  // Combine the plays into an overall strategy for each lead.
  vector<Strategies> leadStrats;
  leadStrats.resize(leadNext);

  for (auto& node: rhoStudyNodes)
  {
    cout << "Multiplying play " << node.playNo << " for lead " <<
      node.leadNo << ", size " << leadStrats[node.leadNo].size() << endl;

    leadStrats[node.leadNo] *= node.strategies;

    cout << " Now " << node.playNo << " for lead " <<
      node.leadNo << ", size " << leadStrats[node.leadNo].size() << endl;
    cout << leadStrats[node.leadNo].str("Strategy") << "\n";
  }

  // Add back the lead-specific constants.
  for (unsigned l = 0; l < leadStrats.size(); l++)
    leadStrats[l] *= boundsLead[l].constants;

  // Combine the lead strategies into an overall strategy.
  for (auto& ls: leadStrats)
  {
    cout << "Adding " << ls.size() << " to " << strategies.size() << endl;
    cout << ls.str("Adding") << "\n";
    strategies += ls;
    cout << " Now " << strategies.size() << endl;
    cout << strategies.str("Added") << "\n";
  }

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


string Plays::str() const
{
  stringstream ss;

  assert(rhoNodes.begin() != rhoNextIter); // Later ! empty()

  ss << rhoNodes.front().play.strHeader();

  // Later for (auto& node: rhoNodes)
  for (auto rhoIter = rhoNodes.begin(); rhoIter != rhoNextIter; rhoIter++)
    ss << rhoIter->play.strLine();

  return ss.str();
}


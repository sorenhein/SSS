#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#include "Combinations.h"
#include "Distribution.h"
#include "Plays.h"
#include "Ranks.h"
#include "Survivor.h"


// It's not that important to hit these, but this works
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
}


void Plays::resize(const unsigned cardsIn)
{
  cards = cardsIn;
  chunk = CHUNK_SIZE[cards];

  leadNodes.resize(chunk.lead);
  lhoNodes.resize(chunk.lho);
  pardNodes.resize(chunk.pard);
  rhoNodes.resize(chunk.rho);
}


unsigned Plays::size() const
{
  return rhoNext;
}


Plays::LeadNode * Plays::logLead(
  const Play& play,
  bool& newFlag)
{
  const SidePosition side = (play.side == POSITION_NORTH ? SIDE_NORTH : SIDE_SOUTH);
  const unsigned lead = play.leadPtr->getRank();

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

  bool newFlag;
  LeadNode * leadPtr = Plays::logLead(play, newFlag);
  LhoNode * lhoPtr = Plays::logLho(play, leadPtr, newFlag);
  PardNode * pardPtr = Plays::logPard(play, lhoPtr, newFlag);

  Plays::logRho(play, pardPtr);
}


void Plays::setCombPtrs(const Combinations& combinations)
{
  for (auto& rhoNode: rhoNodes)
    rhoNode.play.combPtr = 
      combinations.getPtr(rhoNode.play.cardsLeft, rhoNode.play.holding3);
}


#include "const.h"


void Plays::strategizeRHO(
  Distribution const * distPtr,
  const bool debugFlag)
{
  Tvectors tvs;
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
}


void Plays::strategizeLead(
  Tvectors& strategies,
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
}


void Plays::strategize(
  const Ranks& ranks,
  Distribution const * distPtr,
  Tvectors& strategies,
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
  cout << "RHO " << rhoNodes.size() << " " << rhoNext << endl;
  cout << "Pard " << pardNodes.size() << " " << pardNext << endl;
  cout << "LHO " << lhoNodes.size() << " " << lhoNext << endl;
  cout << "Lead " << leadNodes.size() << " " << leadNext << endl;

  strategizeRHO(distPtr, debugFlag);
  strategizePard(debugFlag);
  strategizeLHO(debugFlag);
  strategizeLead(strategies, debugFlag);

}


void Plays::strategizeVoid(
  Distribution const * distPtr,
  Tvectors& strategies,
  bool debugFlag)
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
  // coordinate and play their cards without intrusion from partner.
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

  // For exploration we turn the plays back into a vector.
  // Each play is stored in a PlayInfo.

  struct PlayInfo
  {
    unsigned number;

    SidePosition side;
    unsigned lead;
    unsigned lho;
    unsigned rho;
    bool leadCollapse;
    unsigned holding3;
    unsigned leadNo;

    Tvectors strategies;

    Tvector lower;
    Tvector upper;

    string str(
      const string& header,
      const bool fullFlag = true) const
    {
      stringstream ss;
      ss << header << ": " << 
        lead << " " << lho << " void " << rho << " " <<
        setw(6) << holding3 << " (lead no " << leadNo << ")";
      if (leadCollapse)
        ss << " collapse lead";
      ss << "\n";
      if (fullFlag)
        ss << strategies.str("Strategy", true);
      return ss.str() + "\n";
    };
  };

  list<PlayInfo> playInfo;
  playInfo.resize(rhoNext);

  // Store vectors of extreme outcomes for each lead.
  vector<Tvector> minima, maxima;
  minima.resize(leadNodes.size());
  maxima.resize(leadNodes.size());

  vector<Tvector> constants;
  constants.resize(leadNodes.size());

  unsigned pno = 0;
  unsigned mno = 0;
  unsigned mlast = rhoNodes.front().pardPtr->lhoPtr->leadPtr->lead;
  auto piter = playInfo.begin();

  for (auto rhoIter = rhoNodes.begin(); rhoIter != rhoNextIter; 
      rhoIter++, piter++, pno++)
  {
    const auto& rhoNode = * rhoIter;
    auto& play = * piter;

    play.number = pno;
    play.side = rhoNode.pardPtr->lhoPtr->leadPtr->side;
    play.lead = rhoNode.pardPtr->lhoPtr->leadPtr->lead;
    play.lho = rhoNode.pardPtr->lhoPtr->lho;
    play.rho = rhoNode.play.rho();
    play.leadCollapse = rhoNode.play.leadCollapse;
    play.holding3 = rhoNode.play.holding3;

    assert(play.side == SIDE_NORTH);

    if (play.lead != mlast)
    {
      mno++;
      mlast = play.lead;
    }

    play.leadNo = mno;

    const Survivors& survivors = distPtr->survivors(rhoNode.play);

    play.strategies = rhoNode.play.combPtr->strategies();

    // adapt() renumbers distributions from combPtr.
    // It also renumbers winners within strategies.
    play.strategies.adapt(
      rhoNode.play,
      survivors);

    Tvector cst;
    play.strategies.bound(cst, play.lower, play.upper);

    constants[play.leadNo] *= cst;
    minima[play.leadNo] *= play.lower;
    maxima[play.leadNo] *= play.upper;
    
    if (debugFlag)
    {
      cout << play.str("Vector " + to_string(pno)) << endl;
      cout << cst.str("Constants") << endl;
      cout << minima[play.leadNo].str("Minima") << endl;
      cout << constants[play.leadNo].str("Cumul constants") << endl;
    }
  }

  // Only keep those constants (for a given lead) that
  // correspond to the minimum achievable outcome.

  for (unsigned i = 0; i < constants.size(); i++)
  {
    minima[i].constrict(constants[i]);
    cout << constants[i].str("Constrained constants " +
      to_string(i)) << endl;
  }

  // Remove those constants from the corresponding strategies.
  // Collect all strategies with a single vector into an overall vector.
  vector<Tvectors> simple;
  simple.resize(leadNodes.size());
  unsigned simpleCount = 0;

  piter = playInfo.begin();
  while (piter != playInfo.end())
  {
    auto& play = * piter;
    const unsigned p = play.number;
    cout << play.str("Purging constant play " + to_string(p), false) << 
      endl;

    const unsigned num0 = play.strategies.size();
    const unsigned dist0 = play.strategies.numDists();

    play.strategies.purge(constants[play.leadNo]);
    play.lower.purge(constants[play.leadNo]);
    play.upper.purge(constants[play.leadNo]);

    const unsigned num1 = play.strategies.size();
    const unsigned dist1 = play.strategies.numDists();

    cout << "(" << num0 << ", " << dist0 << ") -> (" <<
      num1 << ", " << dist1 << ")\n";
    cout << play.strategies.str("Purged constant strategy") << "\n";
    
    if (num1 == 0 || dist1 == 0)
    {
      // Nothing left.
      piter = playInfo.erase(piter);
      simpleCount++;
    }
    else if (num1 == 1)
    {
      // One strategy left.
cout << "About to grow simple" << endl;
cout << "size " << simple[play.leadNo].size() << endl;
cout << simple[play.leadNo].str("simple") << endl;

      simple[play.leadNo] *= play.strategies;
      piter = playInfo.erase(piter);
      simpleCount++;
    }
    else
      piter++;
  }

  cout << "Removed " << simpleCount << " strategies\n";
  cout << "Size now " << playInfo.size() << endl;

  for (auto s: simple)
    cout << s.str("simple");

  // Let's say the range of outcomes for a given strategy is
  // (min, max) for a given distribution.  Let's also say that
  // the lowest maximum that any strategy achieves is M.  This is
  // all for a given lead.  Then if M <= min, the defenders will
  // never enter that strategy, so the distribution can be removed
  // from the strategy.

  piter = playInfo.begin();
  while (piter != playInfo.end())
  {
    auto& play = * piter;
    const unsigned p = play.number;

    cout << play.str("Purging non-constant play " + to_string(p), false) << 
      endl;

    const unsigned num0 = play.strategies.size();
    const unsigned dist0 = play.strategies.numDists();

    // cout << play.strategies.str("Pre purging strategy") << endl;

    // Limit the maximum vector to those entries that are <= play.lower.
    Tvector max = maxima[play.leadNo];
    // cout << max.str("max") << endl;
    // cout << play.lower.str("play.lower") << endl;
    play.lower.constrict(max);

    if (max.size() == 0)
    {
      cout << "Nothing to purge\n";
      piter++;
      continue;
    }

    cout << max.str("to purge") << endl;

    play.strategies.purge(max);

    const unsigned num1 = play.strategies.size();
    const unsigned dist1 = play.strategies.numDists();

    cout << "(" << num0 << ", " << dist0 << ") -> (" <<
      num1 << ", " << dist1 << ")\n";
    cout << play.strategies.str("Purged non-constant strategy") << "\n";
    
    if (num1 == 0 || dist1 == 0)
    {
      // Nothing left.
      piter = playInfo.erase(piter);
      simpleCount++;
    }
    else if (num1 == 1)
    {
      // One strategy left.
      simple[play.leadNo] *= play.strategies;
      piter = playInfo.erase(piter);
      simpleCount++;
    }
    else
      piter++;
  }

  cout << "Removed a total of " << simpleCount << " strategies\n";
  cout << "Size now " << playInfo.size() << endl;
  for (unsigned s = 0; s < simple.size(); s++)
    cout << simple[s].str("simple " + to_string(s));

  cout << "Complex plays\n\n";
  for (auto& play: playInfo)
  {
    cout << setw(3) << right << play.number << ": " <<
      setw(2) << play.side << " " << 
      setw(2) << play.lead << " " << 
      setw(2) << play.lho << " " << 
      setw(2) << play.rho << ", " <<
      setw(2) << play.leadNo << ": " << 
      setw(3) << play.strategies.size() <<
      setw(3) << play.strategies.numDists() << endl;
  }

  // Look for rank collapses that happen "during the trick".
  // For example, with KJ975 missing 7 cards, if declarer leads the 5,
  // the trick might go 5 - T - 6 or 5 - T - x.  After the trick they will
  // be the same, but even during the trick declarer should not distinguish
  // between the 6 and the x.  We don't give up on the difference, but we
  // merge the strategies vector by vector, and not by cross product.

  piter = playInfo.begin();
  while (piter != playInfo.end())
  {
    auto& play = * piter;
    const unsigned p = play.number;
    if (! play.leadCollapse)
    {
      piter++;
      continue;
    }

    if (play.lho >= play.lead+3 && play.rho == play.lead-1)
    {
      // Find the matching RHO play.
      auto piter2 = next(piter);
      while (piter2 != playInfo.end() && 
          (piter2->lho != play.lho || piter2->rho != play.lead+1))
        piter2++;

      assert(piter2 != playInfo.end());
cout << piter->str("piter");
cout << piter2->str("piter2");
cout << endl;
      assert(piter->strategies.size() == piter2->strategies.size());
      assert(piter->holding3 == piter2->holding3);

      piter->strategies |= piter2->strategies;
      playInfo.erase(piter2);
    }
    else if (play.rho >= play.lead+3 && play.lho == play.lead-1)
    {
      // Find the matching LHO play.
      auto piter2 = next(piter);
      while (piter2 != playInfo.end() && 
          (piter2->rho != play.rho || piter2->lho != play.lead+1))
        piter2++;

      assert(piter2 != playInfo.end());
cout << piter->str("piter");
cout << piter2->str("piter2");
cout << endl;
      assert(piter->strategies.size() == piter2->strategies.size());
      assert(piter->holding3 == piter2->holding3);

      piter->strategies |= piter2->strategies;
      playInfo.erase(piter2);
      simpleCount++;
    }
    piter++;
  }

  cout << "Removed a total of " << simpleCount << " strategies\n";
  cout << "Size now " << playInfo.size() << endl;

  cout << "Complex plays\n\n";
  for (auto& play: playInfo)
  {
    cout << setw(3) << right << play.number << ": " <<
      setw(2) << play.side << " " << 
      setw(2) << play.lead << " " << 
      setw(2) << play.lho << " " << 
      setw(2) << play.rho << ", " <<
      setw(2) << play.leadNo << ": " << 
      setw(3) << play.strategies.size() <<
      setw(3) << play.strategies.numDists() << endl;
  }


  // Combine the plays into an overall strategy.

  vector<Tvectors> leadStrats;
  leadStrats.resize(leadNext);

  for (auto& play: playInfo)
  {
    cout << "Multiplying play " << play.number << " for lead " <<
      play.leadNo << ", size " << leadStrats[play.leadNo].size() << endl;

    leadStrats[play.leadNo] *= play.strategies;

    cout << " Now " << play.number << " for lead " <<
      play.leadNo << ", size " << leadStrats[play.leadNo].size() << endl;
    cout << leadStrats[play.leadNo].str("Strategy") << "\n";
  }

  for (unsigned l = 0; l < leadStrats.size(); l++)
  {
    cout << constants[l].str("constants " + to_string(l));
    leadStrats[l] *= constants[l];
    cout << leadStrats[l].str("Strategy with constants " + to_string(l)) << "\n";
  }

  strategies.reset();
  for (auto& ls: leadStrats)
  {
    cout << "Adding " << ls.size() << " to " << strategies.size() << endl;
    cout << ls.str("Adding") << "\n";
    strategies += ls;
    cout << " Now " << strategies.size() << " to " << strategies.size() << endl;
    cout << strategies.str("Added") << "\n";
  }

  cout << "Final size " << strategies.size() << endl;
  

  // So now we know for a given lead that certain distributions can
  // be factored out from the individual strategies: Those constants
  // that are also minima.

  // Manual combinations.
  /*
  Tvectors tvs = playInfo[0].strategies;
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
  strategies = playInfo.front().strategies;
}


// TODO Move these two to Play
string Plays::strHeader() const
{
  stringstream ss;
  ss << right <<
    setw(4) << "Side" <<
    setw(5) << "Lead" <<
    setw(5) << "LHO" <<
    setw(5) << "Pard" <<
    setw(5) << "RHO" <<
    setw(5) << "Win?" <<
    setw(5) << "W vd" <<
    setw(5) << "E vd" <<
    setw(10) << "Holding" <<
    endl;
  return ss.str();
}


string Plays::str() const
{
  stringstream ss;
  ss << Plays::strHeader();

  // for (unsigned rno = 0; rno < rhoNext; rno++)
  for (auto rhoIter = rhoNodes.begin(); rhoIter != rhoNextIter; rhoIter++)
  {
    // const auto& rhoNode = rhoNodes[rno];
    const auto& rhoNode = * rhoIter;
    PardNode const * pardPtr = rhoNode.pardPtr;
    LhoNode const * lhoPtr = pardPtr->lhoPtr;
    LeadNode const * leadPtr = lhoPtr->leadPtr;

    ss << right <<
      setw(4) << (leadPtr->side == SIDE_NORTH ? "N" : "S") <<
      setw(5) << leadPtr->lead <<
      setw(5) << (lhoPtr->lho == 0 ? "-" : to_string(lhoPtr->lho)) <<
      setw(5) << (pardPtr->pard == 0 ? "-" : to_string(pardPtr->pard)) <<
      setw(5) << (rhoNode.play.rho() == 0 ? "-" : to_string(rhoNode.play.rho())) <<
      setw(5) << (rhoNode.play.trickNS == 1 ? "+" : "") <<
      setw(5) << (lhoPtr->lho == 0 ? "yes" : "") <<
      setw(5) << (rhoNode.play.rho() == 0 ? "yes" : "") <<
      setw(10) << rhoNode.play.holding3 <<
      endl;
  }
  return ss.str();
}


#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#include "Combinations.h"
#include "Distribution.h"
#include "Plays.h"
#include "Ranks.h"
#include "struct.h"


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
  vector<Card> const * leadOrderPtr,
  vector<Card> const * pardOrderPtr,
  const Play& play,
  PardNode * pardPtr)
{
  if (rhoNextIter == rhoNodes.end())
    rhoNextIter = rhoNodes.insert(rhoNextIter, chunk.rho, RhoNode());

  RhoNode& node = * rhoNextIter;
  rhoNext++;
  rhoNextIter++;

  node.leadOrderPtr = leadOrderPtr;
  node.pardOrderPtr = pardOrderPtr;
  node.pardPtr = pardPtr;

  // TMP
  node.play = play;
}


void Plays::log(
  vector<Card> const * leadOrderPtr,
  vector<Card> const * pardOrderPtr,
  const Play& play)
{
  // The pointers assume that the Ranks object still exists!

  bool newFlag;
  LeadNode * leadPtr = Plays::logLead(play, newFlag);
  LhoNode * lhoPtr = Plays::logLho(play, leadPtr, newFlag);
  PardNode * pardPtr = Plays::logPard(play, lhoPtr, newFlag);

  Plays::logRho(leadOrderPtr, pardOrderPtr, play, pardPtr);
}


void Plays::setCombPtrs(const Combinations& combinations)
{
  for (auto& rhoNode: rhoNodes)
    rhoNode.play.combPtr = 
      combinations.getPtr(rhoNode.play.cardsLeft, rhoNode.play.holding3);
}


#include "const.h"

void Plays::strategize(
  const Ranks& ranks,
  Distribution const * distPtr,
  Tvectors& strategies,
  bool debugFlag)
{
  // This yields strategies where EW have "too much" choice.
  // Therefore the question is going to be whether EW can hold NS
  // to these outcomes by spreading their probability mass well.
  // This will be done subsequently.

  UNUSED(ranks);

cout << "Node counts:" << endl;
cout << "RHO " << rhoNodes.size() << " " << rhoNext << endl;
cout << "Pard " << pardNodes.size() << " " << pardNext << endl;
cout << "LHO " << lhoNodes.size() << " " << lhoNext << endl;
cout << "Lead " << leadNodes.size() << " " << leadNext << endl;

if (debugFlag)
{
  cout << "HERE1\n";
  Plays::printPointers();
}

  Tvectors tvs;
  unsigned rno = 0;
  for (auto rhoIter = rhoNodes.begin(); rhoIter != rhoNextIter; rhoIter++, rno++)
  // for (unsigned rno = 0; rno < rhoNext; rno++)
  {
    // const auto& rhoNode = rhoNodes[rno];
    const auto& rhoNode = * rhoIter;

if (debugFlag)
  cout << "Start of RHO node loop" << endl;
    // Find the distribution numbers that are still possible.
    // TODO We could possibly cache lho in RhoNode (saves looking it up).
    const unsigned pard0 = rhoNode.play.pard();
    const unsigned lead0 = rhoNode.play.lead();
    const unsigned lho = rhoNode.play.lho();
    const unsigned side = rhoNode.play.side;

// if (debugFlag)
  cout << rhoNode.play.strTrick(rno);

    vector<Card> const * northOrderPtr;
    vector<Card> const * southOrderPtr;
    // deque<Card const *> const * northDequePtr;
    // deque<Card const *> const * southDequePtr;
    unsigned first, second;
    if (rhoNode.play.side == POSITION_NORTH)
    {
      northOrderPtr = rhoNode.leadOrderPtr;
      southOrderPtr = rhoNode.pardOrderPtr;
      // northDequePtr = rhoNode.play.leaderCardsPtr;
      // southDequePtr = rhoNode.play.partnerCardsPtr;
      first = rhoNode.play.rho();
      second = lho;
    }
    else
    {
      southOrderPtr = rhoNode.leadOrderPtr;
      northOrderPtr = rhoNode.pardOrderPtr;
      // southDequePtr = rhoNode.play.leaderCardsPtr;
      // northDequePtr = rhoNode.play.partnerCardsPtr;
      first = lho;
      second = rhoNode.play.rho();
    }

    Survivors survivors;
    if (lho == 0 || rhoNode.play.rho() == 0)
      survivors = distPtr->survivors(first, second);
    else if (rhoNode.play.leadCollapse && rhoNode.play.pardCollapse)
    {
      const unsigned pard = rhoNode.pardPtr->pard;
      const unsigned lead = rhoNode.pardPtr->lhoPtr->leadPtr->lead;
      survivors = distPtr->survivorsCollapse2(first, second, pard+1, lead+1);
    }
    else if (rhoNode.play.leadCollapse)
    {
      const unsigned lead = rhoNode.pardPtr->lhoPtr->leadPtr->lead;
      survivors = distPtr->survivorsCollapse1(first, second, lead+1);
    }
    else if (rhoNode.play.pardCollapse)
    {
      const unsigned pard = rhoNode.pardPtr->pard;
      survivors = distPtr->survivorsCollapse1(first, second, pard+1);
    }
    else
      survivors = distPtr->survivors(first, second);

// for (auto v: survivors.distNumbers)
  // cout << "survivor " << v.fullNo << ", " << v.reducedNo << endl;
    
    // Get the strategy from the following combination.  This will
    // have to be renumbered and possibly rotated.
    tvs = rhoNode.play.combPtr->strategies();
// cout << tvs.str("Tvectors") << endl;

    // TODO Just pass in the RHO node in some form?
    tvs.adapt(
      rhoNode.play,
      survivors, 
      rhoNode.play.trickNS, 
      northOrderPtr,
      southOrderPtr,
      rhoNode.play.currBestPtr,
      first == 0,
      second == 0,
      rhoNode.play.rotateFlag);
if (debugFlag)
  cout << tvs.str("Tvectors after adapt", true);

    // Add it to the partner node by cross product.
    rhoNode.pardPtr->strategies *= tvs;
cout << rhoNode.pardPtr->strategies.str("Cum. Tvectors after cross-product", true);
  }

if (debugFlag)
  cout << "Done with RHO nodes" << endl << endl;

  // for (unsigned pno = 0; pno < pardNext; pno++)
  for (auto pardIter = pardNodes.begin(); pardIter != pardNextIter; pardIter++)
  {
    // const auto& pardNode = pardNodes[pno];
    const auto& pardNode = * pardIter;

if (debugFlag)
 cout << "pard node for " << pardNode.pard << endl;

    // Add the partner strategy to the LHO node.
if (debugFlag)
  cout << pardNode.strategies.str("Adding", true);
    pardNode.lhoPtr->strategies += pardNode.strategies;
if (debugFlag)
  cout << pardNode.lhoPtr->strategies.str("LHO node", true);
  }

if (debugFlag)
  cout << "Done with pard nodes" << endl << endl;
  // for (unsigned lno = 0; lno < lhoNext; lno++)
  for (auto lhoIter = lhoNodes.begin(); lhoIter != lhoNextIter; lhoIter++)
  {
    // const auto& lhoNode = lhoNodes[lno];
    const auto& lhoNode = * lhoIter;

if (debugFlag)
  cout << "LHO node for " << lhoNode.lho << endl;
    // Add the LHO strategy to the lead node by cross product.
if (debugFlag)
  cout << lhoNode.strategies.str("Adding", true) << endl;
    lhoNode.leadPtr->strategies *= lhoNode.strategies;
if (debugFlag)
  cout << lhoNode.leadPtr->strategies.str("Lead node", true);
  }

if (debugFlag)
  cout << "Done with LHO nodes" << endl << endl;
  // Add up the lead strategies.
  strategies.reset();
  // for (unsigned ldno = 0; ldno < leadNext; ldno++)
  for (auto ldIter = leadNodes.begin(); ldIter != leadNextIter; ldIter++)
  {
    // const auto& leadNode = leadNodes[ldno];
    const auto& leadNode = * ldIter;

if (debugFlag)
  cout << "Lead node for " << leadNode.side << " | " << leadNode.lead << endl;
    strategies += leadNode.strategies;
if (debugFlag)
  cout << strategies.str("Final", true);
  }

  cout << strategies.str("Strategy", true);
}


void Plays::strategizeVoid(
  Distribution const * distPtr,
  Tvectors& strategies,
  bool debugFlag)
{
  // This yields strategies where EW have "too much" choice.
  // Therefore the question is going to be whether EW can hold NS
  // to these outcomes by spreading their probability mass well.
  // This will be done subsequently.

  // This version of strategize() makes use of the fact that 
  // partner is void.  Therefore the EW plays are in a sense part
  // of a coordinated play that partner cannot disrupt.

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

    Survivors survivors;
    if (play.lho == 0 || play.rho == 0)
      survivors = distPtr->survivors(play.rho, play.lho);
    else if (play.leadCollapse)
    {
      survivors = distPtr->survivorsCollapse1(play.rho, play.lho, 
        play.lead + 1);
    }
    else
      survivors = distPtr->survivors(play.rho, play.lho);

    play.strategies = rhoNode.play.combPtr->strategies();

    // adapt() renumbers distributions from combPtr.
    // It also renumbers winners within strategies.
    play.strategies.adapt(
      rhoNode.play,
      survivors, 
      rhoNode.play.trickNS, 
      rhoNode.leadOrderPtr,
      rhoNode.pardOrderPtr,
      rhoNode.play.currBestPtr,
      play.rho == 0,
      play.lho == 0,
      rhoNode.play.rotateFlag);

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


// TMP

void Plays::printPointers() const
{
  unsigned rno = 0;
  for (auto& rhoNode: rhoNodes)
  {
    cout << setw(2) << rno << " ";
    if (rhoNode.leadOrderPtr)
      cout << rhoNode.leadOrderPtr << setw(4) << rhoNode.leadOrderPtr->size();
    else
      cout << "null";
    cout << " ";
    if (rhoNode.pardOrderPtr)
      cout << rhoNode.pardOrderPtr << setw(4) << rhoNode.pardOrderPtr->size();
    else
      cout << "null";
    cout << "\n";
    rno++;
  }
}

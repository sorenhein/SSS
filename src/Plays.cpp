#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#include "Combinations.h"
#include "Distribution.h"
#include "Plays.h"


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
  const SidePosition side,
  const unsigned lead,
  bool& newFlag)
{
  // We use the fact that plays arrive in order.
  if (side == sidePrev && lead == leadPrev)
  {
    newFlag = false;
    return leadPrevPtr;
  }

  if (leadNext >= leadNodes.size())
    leadNodes.resize(leadNodes.size() + chunk.lead);

  newFlag = true;
  sidePrev = side;
  leadPrev = lead;

  LeadNode& node = leadNodes[leadNext++];
  node.side = side;
  node.lead = lead;
  node.strategies.reset();

  leadPrevPtr = &node;
  return leadPrevPtr;
}


Plays::LhoNode * Plays::logLho(
  const unsigned lho,
  LeadNode * leadPtr,
  bool& newFlag)
{
  if (newFlag == false && lho == lhoPrev)
    return lhoPrevPtr;
  
  if (lhoNext >= lhoNodes.size())
    lhoNodes.resize(lhoNodes.size() + chunk.lho);

  newFlag = true;
  lhoPrev = lho;

  LhoNode& node = lhoNodes[lhoNext++];
  node.lho = lho;
  node.leadPtr = leadPtr;
  node.strategies.reset();

  lhoPrevPtr = &node;
  return lhoPrevPtr;
}


Plays::PardNode * Plays::logPard(
  const unsigned pard,
  LhoNode * lhoPtr,
  bool& newFlag)
{
  if (newFlag == false && pard == pardPrev)
    return pardPrevPtr;

  if (pardNext >= pardNodes.size())
    pardNodes.resize(pardNodes.size() + chunk.pard);

  newFlag = true;
  pardPrev = pard;

  PardNode& node = pardNodes[pardNext++];
  node.pard = pard;
  node.lhoPtr = lhoPtr;
  node.strategies.reset();

  pardPrevPtr = &node;
  return pardPrevPtr;
}


void Plays::logRho(
  const unsigned rho,
  const bool leadCollapse,
  const bool lhoCollapse,
  const bool pardCollapse,
  const bool rhoCollapse,
  const unsigned holding3,
  const bool rotateFlag,
  const unsigned trickNS,
  const bool knownVoidLho,
  const bool knownVoidRho,
  const bool voidPard,
  PardNode * pardPtr)
{
  if (rhoNext >= rhoNodes.size())
    rhoNodes.resize(rhoNodes.size() + chunk.rho);

  RhoNode& node = rhoNodes[rhoNext++];
  node.rho = rho;
  node.leadCollapse = leadCollapse;
  node.lhoCollapse = lhoCollapse;
  node.pardCollapse = pardCollapse;
  node.rhoCollapse = rhoCollapse;
  node.cardsNew = cards +
    (knownVoidLho ? 1 : 0) + 
    (knownVoidRho ? 1 : 0) +
    (voidPard ? 1 : 0) - 4;
  node.holdingNew = holding3;
  node.rotateNew = rotateFlag;
  node.trickNS = trickNS;
  node.knownVoidLho = knownVoidLho;
  node.knownVoidRho = knownVoidRho;
  node.voidPard = voidPard;
  node.pardPtr = pardPtr;
}


void Plays::log(
  const SidePosition side,
  const unsigned lead,
  const unsigned lho,
  const unsigned pard,
  const unsigned rho,
  const bool leadCollapse,
  const bool lhoCollapse,
  const bool pardCollapse,
  const bool rhoCollapse,
  const unsigned holding3,
  const bool rotateFlag)
{
  bool newFlag;
  LeadNode * leadPtr = Plays::logLead(side, lead, newFlag);
  LhoNode * lhoPtr = Plays::logLho(lho, leadPtr, newFlag);
  PardNode * pardPtr = Plays::logPard(pard, lhoPtr, newFlag);

  const unsigned trickNS = (max(lead, pard) > max(lho, rho) ? 1 : 0);
  const bool knownVoidLho = (lho == 0);
  const bool knownVoidRho = (rho == 0);
  const bool voidPard = (pard == 0);

  Plays::logRho(rho,
    leadCollapse, lhoCollapse, pardCollapse, rhoCollapse,
    holding3, rotateFlag,
    trickNS, knownVoidLho, knownVoidRho, voidPard,
    pardPtr);
}


void Plays::setCombPtrs(const Combinations& combinations)
{
if (rhoNext > 0 && rhoNodes[0].pardPtr->pard > 32)
  cout << "HERE4" << endl;
  for (auto& rhoNode: rhoNodes)
    rhoNode.combPtr = 
      combinations.getPtr(rhoNode.cardsNew, rhoNode.holdingNew);
if (rhoNext > 0 && rhoNodes[0].pardPtr->pard > 32)
  cout << "HERE5" << endl;
}


#include "const.h"

void Plays::strategize(
  Distribution const * distPtr,
  Tvectors& strategies)
{
  // This yields strategies where EW have "too much" choice.
  // Therefore the question is going to be whether EW can hold NS
  // to these outcomes by spreading their probability mass well.
  // This will be done subsequently.

cout << "Node counts:" << endl;
cout << "RHO " << rhoNodes.size() << " " << rhoNext << endl;
cout << "Pard " << pardNodes.size() << " " << pardNext << endl;
cout << "LHO " << lhoNodes.size() << " " << lhoNext << endl;
cout << "Lead " << leadNodes.size() << " " << leadNext << endl;

  Tvectors tvs;
  for (unsigned rno = 0; rno < rhoNext; rno++)
  {
    const auto& rhoNode = rhoNodes[rno];
cout << "Start of RHO node loop" << endl;
    // Find the distribution numbers that are still possible.
    // TODO We could possibly cache lho in RhoNode (saves looking it up).
    const unsigned lho = rhoNode.pardPtr->lhoPtr->lho;
    const unsigned side = rhoNode.pardPtr->lhoPtr->leadPtr->side;
cout << "side " << side << " LHO " << lho << " RHO " << rhoNode.rho << endl;

    unsigned first, second;
    if (side == SIDE_NORTH)
    {
      first = rhoNode.rho;
      second = lho;
    }
    else
    {
      first = lho;
      second = rhoNode.rho;
    }

    const auto& survivors = distPtr->survivors(first, second);

for (auto v: survivors)
  cout << "survivor " << v << endl;
    
    // Get the strategy from the following combination.  This will
    // have to be renumbered and possibly rotated.
    tvs = rhoNode.combPtr->strategies();
cout << tvs.str("Tvectors") << endl;
    tvs.adapt(survivors, 
      rhoNode.trickNS, 
      first == 0,
      second == 0,
      rhoNode.rotateNew);
cout << tvs.str("Tvectors after adapt");

    // Add it to the partner node by cross product.
    rhoNode.pardPtr->strategies *= tvs;
cout << rhoNode.pardPtr->strategies.str("Cum. Tvectors after cross-product");
cout << "Address" << rhoNode.pardPtr << endl;
  }

cout << "Done with RHO nodes" << endl << endl;

  for (unsigned pno = 0; pno < pardNext; pno++)
  {
    const auto& pardNode = pardNodes[pno];
cout << "pard node for " << pardNode.pard << endl;
cout << "Address" << &pardNode << endl;

    // Add the partner strategy to the LHO node.
cout << pardNode.strategies.str("Adding");
    pardNode.lhoPtr->strategies += pardNode.strategies;
cout << pardNode.lhoPtr->strategies.str("LHO node");
  }

cout << "Done with pard nodes" << endl << endl;
  for (unsigned lno = 0; lno < lhoNext; lno++)
  {
    const auto& lhoNode = lhoNodes[lno];
cout << "LHO node for " << lhoNode.lho << endl;
    // Add the LHO strategy to the lead node by cross product.
cout << lhoNode.strategies.str("Adding");
    lhoNode.leadPtr->strategies *= lhoNode.strategies;
cout << lhoNode.leadPtr->strategies.str("Lead node");
  }

cout << "Done with LHO nodes" << endl << endl;
  // Add up the lead strategies.
  strategies.reset();
  for (unsigned ldno = 0; ldno < leadNext; ldno++)
  {
    const auto& leadNode = leadNodes[ldno];
cout << "Lead node for " << leadNode.side << " | " << leadNode.lead << endl;
    strategies += leadNode.strategies;
cout << strategies.str("Final");
  }

  cout << strategies.str("Done with all nodes");
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

  for (unsigned rno = 0; rno < rhoNext; rno++)
  {
    const auto& rhoNode = rhoNodes[rno];
    PardNode const * pardPtr = rhoNode.pardPtr;
    LhoNode const * lhoPtr = pardPtr->lhoPtr;
    LeadNode const * leadPtr = lhoPtr->leadPtr;

    ss << right <<
      setw(4) << (leadPtr->side == SIDE_NORTH ? "N" : "S") <<
      setw(5) << leadPtr->lead <<
      setw(5) << (lhoPtr->lho == 0 ? "-" : to_string(lhoPtr->lho)) <<
      setw(5) << (pardPtr->pard == 0 ? "-" : to_string(pardPtr->pard)) <<
      setw(5) << (rhoNode.rho == 0 ? "-" : to_string(rhoNode.rho)) <<
      setw(5) << (rhoNode.trickNS == 1 ? "+" : "") <<
      setw(5) << (rhoNode.knownVoidLho ? "yes" : "") <<
      setw(5) << (rhoNode.knownVoidRho ? "yes" : "") <<
      setw(10) << rhoNode.holdingNew <<
      endl;
  }
  return ss.str();
}


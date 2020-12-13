#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#include "Combinations.h"
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
  for (auto& rhoNode: rhoNodes)
    rhoNode.combPtr = 
      combinations.getPtr(rhoNode.cardsNew, rhoNode.holdingNew);
}


#include "const.h"

void Plays::strategize(
  Distribution const * distPtr,
  Tvectors& strategies)
{
  strategies.reset();
  UNUSED(distPtr);

  // For each rhoNode
  //   Note rho, lho (two play levels up) and rotateNew
  //   distPtr->survivors(lho, rho, rotateNew) -- rotate not implemented
  //   Gives us list of distribution numbers at our play level
  //
  //   combPtr->strategies() gives a Tvector
  //   Should be able to rotate it
  //   Probably Combination should cache a rotated copy
  //   A Tvectors should be able to mirror itself
  //   Fail if 0 strategies
  //
  //   Make a new Tvectors with the right distribution numbers
  //   (assert the right number of distributions)
  //
  //   The PardNode should have a Tvectors
  //   *= the new Tvectors onto it
  // end
  //
  // For each pard node
  //   The LhoNode should have a Tvectors
  //   += the new Tvectors onto it
  //
  // For each LHO node
  //   The LeadNode should have a Tvectors
  //   *= the new Tvectors onto it
  //
  // For each lead node
  //   strategies += the new Tvectors
  //
  // When do the Tvectors get reset at the beginning of a Plays?
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


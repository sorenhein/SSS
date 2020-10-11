#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#include "Plays.h"


// TODO Refine
const vector<ChunkEntry> CHUNK_SIZE =
{
  {  1,   1,   1,   1}, //  0
  {  2,   2,   2,   2}, //  1
  {  2,   2,   2,   2}, //  2
  {  2,   2,   2,   2}, //  3
  {  4,   4,   4,   4}, //  4
  {  6,   6,   6,   6}, //  5
  { 10,  10,  10,  10}, //  6
  { 15,  15,  15,  15}, //  7
  { 25,  25,  25,  25}, //  8
  { 40,  40,  40,  40}, //  9
  { 50,  50,  50,  50}, // 10
  { 70,  70,  70,  70}, // 11
  { 90,  90,  90,  90}, // 12
  {110, 110, 110, 110}, // 13
  {130, 130, 130, 130}, // 14
  {150, 150, 150, 150}  // 15
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

  leadPrevPtr = nullptr;
  lhoPrevPtr = nullptr;
  pardPrevPtr = nullptr;
}


void Plays::resize(const unsigned cards)
{
  chunk = CHUNK_SIZE[cards];

  leadNodes.resize(chunk.lead);
  lhoNodes.resize(chunk.lho);
  pardNodes.resize(chunk.pard);
  rhoNodes.resize(chunk.rho);

  leadNext = 0;
  lhoNext = 0;
  pardNext = 0;
  rhoNext = 0;
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
  
  if (lhoNext > lhoNodes.size())
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

  if (pardNext > pardNodes.size())
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
  PardNode * pardPtr)
{
  if (rhoNext > rhoNodes.size())
    rhoNodes.resize(rhoNodes.size() + chunk.rho);

  RhoNode& node = rhoNodes[rhoNext++];
  node.rho = rho;
  node.leadCollapse = leadCollapse;
  node.lhoCollapse = lhoCollapse;
  node.pardCollapse = pardCollapse;
  node.rhoCollapse = rhoCollapse;
  node.holdingNew = holding3;
  node.rotateNew = rotateFlag;
  node.trickNS = trickNS;
  node.knownVoidLho = knownVoidLho;
  node.knownVoidRho = knownVoidRho;
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

  Plays::logRho(rho,
    leadCollapse, lhoCollapse, pardCollapse, rhoCollapse,
    holding3, rotateFlag,
    trickNS, knownVoidLho, knownVoidRho,
    pardPtr);
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

  for (const auto& rhoNode: rhoNodes)
  {
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

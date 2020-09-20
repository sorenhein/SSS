#ifndef SSS_STRUCT_H
#define SSS_STRUCT_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "const.h"

using namespace std;


// CombEntry is used to map a given holding to a canonical combination,
// where only the ranks within a suit matter.

struct CombEntry
{
  bool canonicalFlag;
  unsigned canonicalHolding3; // Trinary
  unsigned canonicalHolding2; // Binary
  unsigned canonicalIndex;
  bool rotateFlag;
  // Once we have a Combination, probably
  // Combination * combinationPtr;
};

struct PlayEntry
{
  SidePosition side;
  unsigned lead;
  unsigned lho;
  unsigned pard;
  unsigned rho;
  unsigned trickNS;
  bool knownVoidWest;
  bool knownVoidEast;
  unsigned holdingNew;

  void update(
    const SidePosition sideIn,
    const unsigned leadIn,
    const unsigned lhoIn,
    const unsigned pardIn,
    const unsigned rhoIn)
  {
    side = sideIn;
    lead = leadIn;
    lho = lhoIn;
    pard = pardIn;
    rho = rhoIn;
    trickNS = (max(lead, pard) > max(lho, rho) ? 1 : 0);
    if (side == SIDE_NORTH)
    {
      knownVoidWest = (rho == 0);
      knownVoidEast = (lho == 0);
    }
    else
    {
      knownVoidWest = (lho == 0);
      knownVoidEast = (rho == 0);
    }
  }

  string strHeader() const
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

  string str() const
  {
    stringstream ss;
    ss << right << 
      setw(4) << (side == SIDE_NORTH ? "N" : "S") <<
      setw(5) << lead <<
      setw(5) << (lho == 0 ? "-" : to_string(lho)) <<
      setw(5) << (pard == 0 ? "-" : to_string(pard)) <<
      setw(5) << (rho == 0 ? "-" : to_string(rho)) <<
      setw(5) << (trickNS == 1 ? "+" : "") <<
      setw(5) << (knownVoidWest ? "yes" : "") <<
      setw(5) << (knownVoidEast ? "yes" : "") <<
      setw(10) << holdingNew <<
      endl;
    return ss.str();
  }
};

#endif

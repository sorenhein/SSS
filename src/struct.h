#ifndef SSS_STRUCT_H
#define SSS_STRUCT_H

#include <vector>

#include "const.h"

using namespace std;


// CombEntry is used to map a given holding to a canonical combination,
// where only the ranks within a suit matter.

struct CombEntry
{
  bool canonicalFlag;
  unsigned canonicalHolding;
  unsigned canonicalIndex;
  bool rotateFlag;
  vector<char> canonical2comb;
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
  unsigned holdingNew3;
  unsigned holdingNew2;

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
};

struct RankInfo
{
  unsigned count;
  vector<char> cards;

  RankInfo()
  {
    RankInfo::clear();
  }

  void clear()
  {
    count = 0;
  }

  void add(const char card)
  {
    cards[count] = card;
    count++;
  }
};

#endif

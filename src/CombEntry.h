/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBENTRY_H
#define SSS_COMBENTRY_H

#include <list>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


// CombEntry is used to map a given holding to a canonical combination,
// where only the ranks within a suit matter.


struct CombReference
{
  unsigned holding3; // Trinary
  unsigned holding2; // Binary
  unsigned index;
  bool rotateFlag;
  // Once we have a Combination, probably
  // Combination * combinationPtr;
  
  bool operator < (const CombReference& cr2) const
  {
    return (holding3 < cr2.holding3);
  }

  bool operator == (const CombReference& cr2) const
  {
    return (holding3 == cr2.holding3);
  }

  string str() const
  {
    stringstream ss;

    ss <<
      holding3 << " | " <<
      "0x" << hex << holding3 << " / " <<
      dec << holding2;

    return ss.str();
  }
};


struct CombEntry
{
  bool canonicalFlag;
  CombReference canonical;
  
  bool minimalFlag;
  list<CombReference> minimals;
};

#endif

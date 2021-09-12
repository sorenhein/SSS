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

#include "../utils/CombinationType.h"

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
    // Used narrowly to sort minimal holdings.
    // Must be called == (or define custom comparator).
    return (holding3 == cr2.holding3);
  }

  bool equal(const CombReference& cr2) const
  {
    // This is are more complete comparison suitable for checking
    // file read/write.
    return (holding3 == cr2.holding3 &&
        holding2 == cr2.holding2 &&
        rotateFlag == cr2.rotateFlag);
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
  // TODO Currently not yet used
  CombinationType type;

  bool canonicalFlag;
  CombReference canonical;
  
  bool minimalFlag;
  list<CombReference> minimals;


  bool operator == (const CombEntry& ce2)
  {
    if (type != ce2.type)
      return false;

    if (canonicalFlag != ce2.canonicalFlag)
      return false;

    if (minimalFlag != ce2.minimalFlag)
      return false;
    
    if (! canonicalFlag && ! (canonical.equal(ce2.canonical)))
      return false;

    if (! minimalFlag)
    {
      if (minimals.size() != ce2.minimals.size())
        return false;
      
      auto iter1 = minimals.begin();
      auto iter2 = ce2.minimals.begin();
      while (iter1 != minimals.end())
      {
        if (! iter1->equal(* iter2))
          return false;
        iter1++;
        iter2++;
      }
    }

    return true;
  }
};

#endif

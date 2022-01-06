/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBENTRY_H
#define SSS_COMBENTRY_H

#include <list>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Combination.h"

#include "../utils/CombinationType.h"

#include "../const.h"

using namespace std;


// CombEntry is used to map a given holding to a canonical combination,
// where only the ranks within a suit matter.


struct CombReference
{
  unsigned holding3; // Trinary
  bool rotateFlag;
  // Once we have a Combination, probably
  // Combination * combinationPtr;
  
  bool operator < (const CombReference& cr2) const
  {
    return (holding3 < cr2.holding3);
  }

  bool operator == (const CombReference& cr2) const
  {
    return (holding3 == cr2.holding3 && rotateFlag == cr2.rotateFlag);
  }

  bool operator != (const CombReference& cr2) const
  {
    return ! (* this == cr2);
  }

  string str() const
  {
    stringstream ss;
    ss << holding3 << " | " << "0x" << hex << holding3;
    return ss.str();
  }

  string strSimple() const
  {
    return to_string(holding3) + " " +
      (rotateFlag ? "(rot)" : "(nonrot)");
  }
};


struct CombEntry
{
  CombinationType type;

  bool referenceFlag;
  CombReference reference;
  unsigned refIndex;
  unsigned refHolding2;
  
  bool minimalFlag;
  list<CombReference> minimals;

  unsigned char winRankLow;


  bool operator == (const CombEntry& ce2)
  {
    if (type != ce2.type)
      return false;

    if (referenceFlag != ce2.referenceFlag)
      return false;

    if (minimalFlag != ce2.minimalFlag)
      return false;
    
    if (! referenceFlag && reference != ce2.reference)
      return false;

    if (! minimalFlag)
    {
      if (minimals.size() != ce2.minimals.size())
        return false;
      
      auto iter1 = minimals.begin();
      auto iter2 = ce2.minimals.begin();
      while (iter1 != minimals.end())
      {
        if (* iter1 != * iter2)
          return false;
        iter1++;
        iter2++;
      }
    }

    return true;
  }


  bool fixMinimals(
    const unsigned ownHolding3,
    const vector<CombEntry>& centries)
  {
  UNUSED(ownHolding3);
    // Check that each non-minimal holding refers to minimal ones.
    // We actually follow through and change the minimals.
    // Once ranks are good, this method should no longer be needed.
    // Returns true if nothing is modified.
 
    bool changeFlag = false;
    auto iter = minimals.begin();
 
    while (iter != minimals.end())
    {
      const CombEntry& centry = centries[iter->holding3];
 
      if (centry.minimalFlag)
      {
        cout << "Is minimal: " << iter->str() << endl;
        iter++;
      }
      else if (iter->holding3 == centry.minimals.front().holding3)
      {
        // Is a partial self-reference, so we already have its minimals.
        iter++;
      }
      else
      {
        // Erase the non-minimal one and add the ones it points to.
        // Take into account the rotation flag -- we want the product
        // of all rotations to be the really minimal holding.
        for (auto& min: centry.minimals)
        {
            minimals.push_back(min);
            CombReference& cr = minimals.back();
            cr.rotateFlag ^= iter->rotateFlag;
        }
 
          iter = minimals.erase(iter);
          changeFlag = true;
      }
    }

    if (changeFlag)
    {
      minimals.sort();
      minimals.unique();
    }

    return ! changeFlag;
  }


  string str() const
  {
    string s;
    if (referenceFlag)
      s = "reference, ";
    else
      s = "non-reference, ";

    s += reference.strSimple() + "\n";

    if (minimalFlag)
      s += "minimal\n";
    else
    {
      s += "non-minimal, size " + to_string(minimals.size()) + "\n";
      for (auto& m: minimals)
        s += m.str() + "\n";
    }
    
    return s;
  }
};

#endif

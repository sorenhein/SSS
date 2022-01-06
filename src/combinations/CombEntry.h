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

#include "CombReference.h"

#include "../utils/CombinationType.h"

#include "../const.h"

using namespace std;


// CombEntry is used to map a given holding to a canonical combination,
// where only the ranks within a suit matter.


struct CombEntry
{
  CombinationType type;

  bool referenceFlag;
  CombReference reference;
  unsigned refIndex;
  unsigned refHolding2;
  
  bool minimalFlag;
  list<CombReference> minimals;


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


  void consolidateMinimals()
  {
    minimals.sort();
    minimals.unique();
  }


  bool fixMinimals(const vector<CombEntry>& centries)
  {
    // Check that each non-minimal holding refers to minimal ones.
    // We actually follow through and change the minimals.
    // Once ranks are good, this method should no longer be needed.
    // Returns true if nothing is modified.
 
    bool changeFlag = false;
    auto iter = minimals.begin();
 
    while (iter != minimals.end())
    {
      const CombEntry& centry = centries[iter->getHolding3()];
 
      if (centry.minimalFlag)
      {
        cout << "Is minimal: " << iter->strHolding() << endl;
        iter++;
      }
      else if (iter->getHolding3() == centry.minimals.front().getHolding3())
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
          cr.rotateBy(* iter);
        }
 
        iter = minimals.erase(iter);
        changeFlag = true;
      }
    }

    if (changeFlag)
      consolidateMinimals();

    return ! changeFlag;
  }


  void setReference(
    const unsigned holding3In,
    const unsigned holding2In,
    const bool rotateFlagIn)
  {
    reference.set(holding3In, rotateFlagIn);
    refHolding2 = holding2In;
  }

  unsigned getHolding3() const
  {
    return reference.getHolding3();
  }


  void addMinimal(
    const unsigned holding3In,
    const bool rotateFlagIn)
  {
    minimals.emplace_back(CombReference());
    CombReference& combRef = minimals.back();
    combRef.set(holding3In, rotateFlagIn);
  }


  void addMinimalSelf()
  {
    // We rely in this being the front one.
    minimals.push_front(reference);
  }


  bool minimalsEmpty() const
  {
    return minimals.empty();
  }


  string strHolding() const
  {
    return reference.strHolding();
  }


  string strMinimals() const
  {
    string s;
    for (auto& m: minimals)
      s += "Minimal holding: " + m.strSimple() + "\n";
    return s + "\n";
  }


  string str() const
  {
    string s;

    s = "Flags: ";
    s += (referenceFlag ? "reference" : "non-reference");
    s += + ", ";
    s += (minimalFlag ? "minimal" : 
      "non-minimal(" + to_string(minimals.size()) + ")\n");

    s += "Holding: " + reference.strSimple() + " / " + 
      to_string(refHolding2) + "\n";

    if (! minimalFlag)
      s += strMinimals();
    
    return s;
  }
};

#endif

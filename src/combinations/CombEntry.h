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


class CombEntry
{
  private:

  CombinationType type;

  bool referenceFlag;
  CombReference reference;
  unsigned refIndex;
  unsigned refHolding2;
  
  // A combination is minimal if it references itself (and maybe others)
  bool minimalFlag;
  list<CombReference> minimals;


  public:

  list<CombReference>::iterator begin() 
    { return minimals.begin(); };
  list<CombReference>::iterator end() 
    { return minimals.end(); };
  list<CombReference>::const_iterator begin() const
    { return minimals.begin(); };
  list<CombReference>::const_iterator end() const
    { return minimals.end(); };

  void setReference(
    const unsigned holding3In,
    const unsigned holding2In,
    const bool rotateFlagIn)
  {
    reference.set(holding3In, rotateFlagIn);
    refHolding2 = holding2In;
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


  void setType(const CombinationType typeIn)
  {
    type = typeIn;
  }


  CombinationType getType() const
  {
    return type;
  }


  unsigned getHolding3() const
  {
    return reference.getHolding3();
  }


  unsigned getHolding2() const
  {
    return refHolding2;
  }


  void setIndex(const unsigned indexIn)
  {
    refIndex = indexIn;
  }


  unsigned getIndex() const
  {
    return refIndex;
  }


  void setReference(const bool referenceFlagIn = true)
  {
    // TODO Can we just set this with no further consequences?
    referenceFlag = referenceFlagIn;
  }


  bool isReference() const
  {
    return referenceFlag;
  }


  void setCanonical()
  {
    // canonicalFlag = true;
  }


  bool isCanonical() const
  {
    return referenceFlag; // TODO canonicalFlag
  }


  void setMinimal()
  {
    minimalFlag = true;

    // TODO minimals.clear()?
  }


  bool isMinimal() const
  {
    return minimalFlag;
  }


  bool minimalsEmpty() const
  {
    return minimals.empty();
  }


  unsigned char packFlags() const
  {
    // Bit layout:
    // 7  : Unused
    // 6  : minimalFlag
    // 5  : canonicalFlag
    // 4  : referenceFlag
    // 0-3: combination type
    
    return
      (minimalFlag << 6) |
      // (canonicalFlag << 5) |
      (referenceFlag << 4) |
      (static_cast<unsigned char>(type));
  }


  void unpackFlags(const unsigned data)
  {
    minimalFlag = ((data & 0x40) ? true : false);
    // canonicalFlag = ((data & 0x20) ? true : false);
    referenceFlag = ((data & 0x10) ? true : false);
    type = static_cast<CombinationType>(data & 0xf);
  }


  void packSelf(
    vector<unsigned>& vstream,
    unsigned& pos) const
  {
    reference.pack(vstream, pos);
  }


  void unpackSelf(
    const vector<unsigned>& vstream,
    unsigned& pos)
  {
    reference.unpack(vstream, pos);
  }


  void packMinimals(
    vector<unsigned>& vstream,
    unsigned& pos) const
  {
    for (auto& min: minimals)
      min.pack(vstream, pos);
  }


  void unpackMinimals(
    const vector<unsigned>& vstream,
    unsigned& pos)
  {
    for (auto& min: minimals)
      min.unpack(vstream, pos);
  }


  string strHolding() const
  {
    return reference.strHolding() + " / " + to_string(refHolding2);
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

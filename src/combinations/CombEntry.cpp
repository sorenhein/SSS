/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CombMemory.h"
#include "CombEntry.h"

using namespace std;


CombEntry::CombEntry()
{
  CombEntry::reset();
}


void CombEntry::reset()
{
  // Higher bits (flags) are zero.
  bitvector = COMB_SIZE;  

  refIndex = 0;
  refHolding2 = 0;
  minimals.clear();
}


void CombEntry::setReferenceHolding(
  const unsigned holding3In,
  const unsigned holding2In,
  const bool rotateFlagIn)
{
  reference.set(holding3In, rotateFlagIn);
  refHolding2 = holding2In;
}


void CombEntry::addMinimal(
  const unsigned holding3In,
  const bool rotateFlagIn)
{
  minimals.emplace_back(CombReference());
  CombReference& combRef = minimals.back();
  combRef.set(holding3In, rotateFlagIn);
}


void CombEntry::addMinimalSelf()
{
  // We rely in this being the front one.
  minimals.push_front(reference);
}


bool CombEntry::operator == (const CombEntry& ce2)
{
  if (bitvector != ce2.bitvector)
    return false;
    
  if (! CombEntry::isReference() && reference != ce2.reference)
    return false;

  if (! CombEntry::isMinimal())
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


void CombEntry::consolidateMinimals()
{
  minimals.sort();
  minimals.unique();
}


bool CombEntry::fixMinimals(
  const CombMemory& combMemory,
  const unsigned char cards)
{
  // Check that each non-minimal holding refers to minimal ones.
  // We actually follow through and change the minimals.
  // Returns true if nothing is modified.
  // TODO
  // Once ranks are good, this method should no longer be needed.
 
  bool changeFlag = false;
  auto iter = minimals.begin();
 
  while (iter != minimals.end())
  {
    // const CombEntry& centry = centries[iter->getHolding3()];
    const CombEntry& centry = combMemory.getEntry(cards, iter->getHolding3());

    if (centry.isMinimal())
{
      iter++;
}
    else if (iter->getHolding3() == 
        centry.minimals.front().getHolding3())
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


void CombEntry::setType(const CombinationType typeIn)
{
  bitvector &= 0xf0;
  bitvector |= typeIn;
}


CombinationType CombEntry::getType() const
{
  return static_cast<CombinationType>(bitvector & 0xf);
}


unsigned CombEntry::getHolding3() const
{
  return reference.getHolding3();
}


unsigned CombEntry::getHolding2() const
{
  return refHolding2;
}


void CombEntry::setIndex(const unsigned indexIn)
{
  refIndex = indexIn;
}


unsigned CombEntry::getIndex() const
{
  return refIndex;
}


void CombEntry::setReference(const bool referenceFlagIn)
{
  if (referenceFlagIn)
    bitvector |= 0x10;
  else
    bitvector &= 0xef;
}

bool CombEntry::isReference() const
{
  return ((bitvector & 0x10) != 0);
}


void CombEntry::setCanonical(const bool canonicalFlagIn)
{
  if (canonicalFlagIn)
    bitvector |= 0x20;
  else
    bitvector &= 0xdf;
}


bool CombEntry::isCanonical() const
{
  return ((bitvector & 0x20) != 0);
}


void CombEntry::setMinimal(const bool minimalFlagIn)
{
  if (minimalFlagIn)
    bitvector |= 0x40;
  else
    bitvector &= 0xbf;

  // TODO minimals.clear()?
}


bool CombEntry::isMinimal() const
{
  return ((bitvector & 0x40) != 0);
}


bool CombEntry::minimalsEmpty() const
{
  return minimals.empty();
}


unsigned char CombEntry::packFlags() const
{
  return bitvector;
}


void CombEntry::unpackFlags(const unsigned char data)
{
  bitvector = data;
}


void CombEntry::packSelf(
  vector<unsigned>& vstream,
  unsigned& pos) const
{
  reference.pack(vstream, pos);
}


void CombEntry::unpackSelf(
  const vector<unsigned>& vstream,
  unsigned& pos)
{
  reference.unpack(vstream, pos);
}


void CombEntry::packMinimals(
  vector<unsigned>& vstream,
  unsigned& pos) const
{
  for (auto& min: minimals)
    min.pack(vstream, pos);
}


void CombEntry::unpackMinimals(
  const vector<unsigned>& vstream,
  unsigned& pos)
{
  for (auto& min: minimals)
    min.unpack(vstream, pos);
}


string CombEntry::strHolding() const
{
  return reference.strHolding() + " / " + to_string(refHolding2);
}


string CombEntry::strMinimals() const
{
  string s;
  for (auto& m: minimals)
    s += "Minimal holding: " + m.strSimple() + "\n";
  return s + "\n";
}


string CombEntry::str() const
{
  string s;

  s = "Type : " + CombinationNames[CombEntry::getType()] + "\n";
  s += "Flags: ";
  s += (CombEntry::isReference() ? "reference" : "non-reference");
  s += ", ";
  s += (CombEntry::isCanonical() ? "canonical" : "non-canonical");
  s += ", ";
  s += (CombEntry::isMinimal() ? "minimal" : "non-minimal");
  s += " (" + to_string(minimals.size()) + ")\n";

  if (! CombEntry::isMinimal())
    s += strMinimals();
    
  return s;
}


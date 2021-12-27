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

#include "Combination.h"

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

    return this->equal(cr2);
    // return (holding3 == cr2.holding3);
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

  string strSimple() const
  {
    return to_string(holding3) + " " +
      (rotateFlag ? "(rot)" : "(nonrot)");
  }
};


struct CombEntry
{
  // TODO Currently not yet used
  CombinationType type;

  CombReference own;

  bool referenceFlag;
  CombReference reference;
  
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
    
    if (! referenceFlag && ! (reference.equal(ce2.reference)))
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


  bool getMinimalSpans(
    const vector<CombEntry>& centries,
    const vector<Combination>& uniqs,
    list<unsigned char>& winRanksLow,
    unsigned char& span,
    unsigned& length) const
  {
    // In general, the strategies in checkReductions may have more
    // strategy's than it should because a play was not considered.
    // For example, 9/1232 (AKT9/Q8) has two strategies with the same
    // tricks, one with 4N'S (KQ) and one with 2N (K).  This has
    // a pseudo-minimal version 9/1288 (AKT7/Q6) with only one
    // strategy, but still 6 distributions too, 5N'S (KQ).  This has
    // a real minimal version 9/1432 (AK87/Q6) with 3N'S (KQ).
    // This method would then return (5, 3) and 0, where 0 is the
    // difference between the highest rank (whether or not it's ever 
    // a minimal winner) and lowest winner among the minimal 
    // strategy's (so 5-5 or 3-3).  They should be the same for 
    // all minimals.

    unsigned char rankHigh;
    bool firstFlag = true;
    for (auto& min: minimals)
    {
      const auto& ceMin = centries[min.holding3];
      if (! ceMin.minimalFlag)
      {
        // This should not happen long-term, and short-term it is
        // addressed in checkMinimals().
cout << "WARNSKIP: Skipping non-minimal entry\n";
        continue;
      }
  
      assert(ceMin.reference.index < uniqs.size());
      const Combination& comb = uniqs[ceMin.reference.index];

      rankHigh = comb.getMaxRank();
      winRanksLow.push_back(ceMin.winRankLow);
  
      if (firstFlag)
      {
        span = rankHigh - ceMin.winRankLow;
        length = comb.strategies().size();
        firstFlag = false;
      }
      else
      {
        if (rankHigh - ceMin.winRankLow != span)
          return false;

        if (comb.strategies().size() != length)
          return false;
      }
    }
    return true;
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
      const CombEntry& centry = centries[iter->holding3];
 
      if (centry.minimalFlag)
        iter++;
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

    s += own.str() + "\n";

    if (minimalFlag)
      s += "minimal\n";
    else
    {
      s += "non-minimal, size " + to_string(minimals.size()) + "\n";
      for (auto& m: minimals)
        s += m.str();
    }
    
    return s;
  }
};

#endif

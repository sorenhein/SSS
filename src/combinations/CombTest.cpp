/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "../distributions/Distributions.h"
#include "../strategies/Strategies.h"
#include "Combination.h"
#include "CombEntry.h"

#include "CombTest.h"

#include "../const.h"


void CombTest::checkAllMinimals(vector<CombEntry>& centries) const
{
  for (unsigned holding = 0; holding < centries.size(); holding++)
  {
    for (auto& min: centries[holding].minimals)
    {
      // TODO Should be only if completely non-minimal
      if (! centries[min.holding3].minimalFlag)
      {
        cout << "ERROR: holding " << holding << " uses non-minimals\n";
        break;
      }
    }
  }
}


void CombTest::dumpInputs(
  const string& title,
  const vector<CombEntry>& centries,
  const CombEntry& centry,
  const Strategies& strategies,
  const unsigned char maxRank) const
{
  cout << title << "\n";

  cout << "centry:\n";
  cout << centry.str();

  cout << "Minimals:\n";
  for (auto& min: centry.minimals)
  {
    const auto& ceMin = centries[min.holding3];
    cout << ceMin.str();
  }

  cout << strategies.str("strategies", true);

  cout << "maxRank " << +maxRank << "\n\n";
}


void CombTest::checkReductions(
  const vector<CombEntry>& centries,
  const vector<Combination>& uniqs,
  const CombEntry& centry,
  const Strategies& strategies,
  const unsigned char maxRank,
  const Distribution& distribution) const
{
  Strategies stratsCumul;

  for (auto& min: centry.minimals)
  {
    const auto& ceMin = centries[min.holding3];
    assert(ceMin.refIndex < uniqs.size());
    const Combination& comb = uniqs[ceMin.refIndex];
    Strategies stratsMin = comb.strategies();

    if (min.holding3 == centry.reference.holding3)
    {
      // Special case.  This should be at the front of the list.
      // TODO This and the below are a lot of copying of Strategies.
      stratsCumul += stratsMin;
      continue;
    }

    // This may or may not be a completely minimal entry, but it
    // should not be completely non-minimal.

    const unsigned char maxRankUnadjusted = comb.getMaxRank();

    const char rankAdder = static_cast<char>(maxRank) -
      static_cast<char>(maxRankUnadjusted);

    const unsigned char winRankLow = comb.strategies().winRankLow();

    // Get the reduction that underlies the method.
    const unsigned char rankCritical = 
      (winRankLow == UCHAR_NOT_SET ? 0 : winRankLow + rankAdder);

    const auto& reduction = distribution.getReduction(rankCritical);

    stratsMin.expand(reduction, rankAdder, min.rotateFlag);
    stratsCumul += stratsMin;
    
  }

  // TODO The sum check must take into account whether the combination
  // refers to itself among others.  In this case we need to add itself
  // and not just the minimals.

  if (! (stratsCumul == strategies))
  {
    cout << "Checking: " << centry.reference.str() << endl;
    CombTest::dumpInputs("MINIMUM MISMATCH", 
      centries, centry, strategies, maxRank);
    cout << stratsCumul.str("Cumulative", true);
  }


  // TODO Checks:
  // If our strategies has N"S and NS", we also expect N'S' to exist,
  // and we don't expect N"S".
}


void CombTest::checkAllReductions(
  const unsigned cards,
  const vector<CombEntry>& centries,
  const vector<Combination>& uniqs,
  const Distributions& distributions) const
{
  for (unsigned holding = 0; holding < centries.size(); holding++)
  {
    // Only look at non-minimal combinations.
    const CombEntry& centry = centries[holding];
    if (! centry.referenceFlag || centry.minimalFlag)
      continue;

    const Combination& comb = uniqs[centry.refIndex];
    CombTest::checkReductions(centries, uniqs, centry, 
      comb.strategies(), 
      comb.getMaxRank(),
      * distributions.ptrNoncanonical(cards, centry.refHolding2));
  }
}


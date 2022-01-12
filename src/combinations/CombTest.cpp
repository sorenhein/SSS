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
#include "CombMemory.h"
#include "CombTest.h"

#include "../const.h"


void CombTest::checkAllMinimals(
  const CombMemory& combMemory,
  const unsigned cards) const
{
  for (unsigned holding = 0; holding < combMemory.size(cards); holding++)
  {
    const CombEntry& centry = combMemory.getEntry(cards, holding);
    for (auto& min: centry)
    {
      if (! combMemory.getEntry(cards, min.getHolding3()).isMinimal())
      {
        cout << "ERROR: holding " << holding << " uses non-minimals\n";
        break;
      }
    }
  }
}


void CombTest::checkReductions(
  const unsigned cards,
  const CombMemory& combMemory,
  const CombEntry& centry,
  const Strategies& strategies,
  const unsigned char maxRank,
  const Distribution& distribution) const
{
  Strategies stratsCumul;

  for (auto& min: centry)
  {
    const Combination& comb = combMemory.getComb(cards, min.getHolding3());
    Strategies stratsMin = comb.strategies();

    if (min.getHolding3() == centry.getHolding3())
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

    stratsMin.expand(reduction, rankAdder, min.getRotateFlag());
    stratsCumul += stratsMin;
    
  }

  // TODO The sum check must take into account whether the combination
  // refers to itself among others.  In this case we need to add itself
  // and not just the minimals.

  if (! (stratsCumul == strategies))
  {
    cout << "MISMATCH: " << centry.strHolding() << endl;
    cout << centry.str();
    cout << strategies.str("strategies", true);
    cout << "maxRank " << +maxRank << "\n\n";
    cout << stratsCumul.str("Cumulative", true);
  }

/*
if (centry.getHolding3() == 2072)
{
    cout << "DEBUG: " << centry.strHolding() << endl;
    cout << centry.str();
    cout << strategies.str("strategies", true);
    cout << "maxRank " << +maxRank << "\n\n";
    cout << stratsCumul.str("Cumulative", true);
}
*/


  // TODO Checks:
  // If our strategies has N"S and NS", we also expect N'S' to exist,
  // and we don't expect N"S".
}


void CombTest::checkAllReductions(
  const unsigned cards,
  const CombMemory& combMemory,
  const Distributions& distributions) const
{
  for (unsigned holding = 0; holding < combMemory.size(cards); holding++)
  {
    const CombEntry& centry = combMemory.getEntry(cards, holding);
    if (! centry.isReference())
      continue;

    const Combination& comb = combMemory.getComb(cards, holding);

    CombTest::checkReductions(
      cards,
      combMemory,
      centry, 
      comb.strategies(), 
      comb.getMaxRank(),
      * distributions.ptrNoncanonical(cards, centry.getHolding2()));
  }
}


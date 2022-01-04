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
  cout << "centry.winRankLow " << +centry.winRankLow << "\n";

  cout << "Minimals:\n";
  for (auto& min: centry.minimals)
  {
    const auto& ceMin = centries[min.holding3];
    cout << ceMin.str();
  }

  cout << strategies.str("strategies", true);

  cout << "maxRank " << +maxRank << "\n\n";
}


void CombTest::dumpSpans(
  list<unsigned char> ranksHigh,
  list<unsigned char> winRanksLow) const
{
  cout << "ranksHigh:\n";
  for (auto& r: ranksHigh)
    cout << +r << "\n";
  cout << "\n";

  cout << "winRanksLow:\n";
  for (auto& r: winRanksLow)
    cout << +r << "\n";
  cout << "\n";
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
    assert(ceMin.reference.index < uniqs.size());
    const Combination& comb = uniqs[ceMin.reference.index];
    Strategies stratsMin = comb.strategies();

    if (min.holding3 == centry.own.holding3)
    {
      // Special case.  This should be at the front of the list.
      // TODO This and the below are a lot of copying of Strategies.
// cout << "Adding ourselves\n";
      stratsCumul += stratsMin;
// cout << stratsCumul.str("Cumul after ourselves", true);
      continue;
    }

    // This may or may not be a completely minimal entry, but it
    // should not be completely non-minimal.

    const unsigned char maxRankUnadjusted = comb.getMaxRank();
// cout << "Adding holding3 " << min.holding3 << ", " << +maxRank << endl;
// cout << stratsMin.str("Unadjusted lookup", true);

    const char rankAdder = static_cast<char>(maxRank) -
      static_cast<char>(maxRankUnadjusted);

    // Get the reduction that underlies the method.
    const unsigned char rankCritical = 
      (ceMin.winRankLow == UCHAR_NOT_SET ? 0 : ceMin.winRankLow + rankAdder);

// cout << "Its max rank " << +maxRankUnadjusted << "\n";
// cout << "Rank adder " << +rankAdder << endl;
// cout << "Our critical Rank " << +rankCritical << endl;

    const auto& reduction = distribution.getReduction(rankCritical);


    stratsMin.expand(reduction, rankAdder, min.rotateFlag);

// cout << "Reduction:\n";
// cout << reduction.str() << endl;
// cout << stratsMin.str("Expansion", true);
    stratsCumul += stratsMin;
// cout << stratsCumul.str("Cumul", true);
    
  }

  // TODO The sum check must take into account whether the combination
  // refers to itself among others.  In this case we need to add itself
  // and not just the minimals.

  if (stratsCumul == strategies)
  {
    // cout << "MINIMUM MATCH" << endl;
  }
  else
  {
    cout << "Checking: " << centry.reference.str() << endl;

    cout << "MINIMUM MISMATCH" << endl;
    CombTest::dumpInputs("r > 16", centries, centry, strategies, maxRank);
    // CombTest::dumpSpans(winRanksLow, ranksHigh);
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

    const Combination& comb = uniqs[centry.reference.index];
    CombTest::checkReductions(centries, uniqs, centry, comb.strategies(), 
      comb.getMaxRank(),
      * distributions.ptrNoncanonical(cards, centry.reference.holding2));
  }
}


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
  cout << "centry.winRankLow " << centry.winRankLow << "\n";

  cout << "Minimals:\n";
  for (auto& min: centry.minimals)
  {
    const auto& ceMin = centries[min.holding3];
    cout << ceMin.str();
  }

  cout << strategies.str("strategies", true);

  cout << "maxRank " << maxRank << "\n\n";
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

  list<unsigned char> ranksHigh;
  list<unsigned char> winRanksLow;
  centry.getMinimalSpans(centries, uniqs, ranksHigh, winRanksLow);

  // Each minimal may have a different reduction.
  auto iterHigh = ranksHigh.begin();
  auto iterLow = winRanksLow.begin();
  Strategies stratsCumul;

  for (auto& min: centry.minimals)
  {
    const auto& ceMin = centries[min.holding3];
    if (! ceMin.minimalFlag)
    {
      // This should not happen long-term, and short-term it is
      // addressed in checkMinimals().
      cout << "Skipping non-minimal entry\n";
      continue;
    }

    // Get the reduction that underlies the method.
    unsigned char rankCritical;
    if (* iterLow == UCHAR_NOT_SET)
      rankCritical = UCHAR_NOT_SET;
    else
    {
      rankCritical = maxRank + * iterLow - * iterHigh;
      assert(rankCritical > 0 && rankCritical < 16);
    }

    const auto& reduction = distribution.getReduction(rankCritical);

    const char rankAdder = static_cast<char>(maxRank) -
      static_cast<char>(* iterHigh);

    assert(ceMin.reference.index < uniqs.size());
    Strategies stratsMin = uniqs[ceMin.reference.index].strategies();
    stratsMin.expand(reduction, rankAdder, min.rotateFlag);

    stratsCumul += stratsMin;
    
    iterHigh++;
    iterLow++;
  }

  // TODO The sum check must take into account whether the combination
  // refers to itself among others.  In this case we need to add itself
  // and not just the minimals.

  if (stratsCumul == strategies)
  {
    cout << "MINIMUM MATCH" << endl;
  }
  else
  {
    cout << "MINIMUM MISMATCH" << endl;
    CombTest::dumpInputs("r > 16", centries, centry, strategies, maxRank);
    CombTest::dumpSpans(winRanksLow, ranksHigh);
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

cout << "Checking:  cards " << cards << ", " <<
  centry.reference.str() << endl;

if (cards == 9 && holding == 13649)
{
  cout << "HERE\n";
}

    const Combination& comb = uniqs[centry.reference.index];
    CombTest::checkReductions(centries, uniqs, centry, comb.strategies(), 
      comb.getMaxRank(),
      * distributions.ptrNoncanonical(cards, centry.reference.holding2));
  }
}


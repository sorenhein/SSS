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


void CombTest::checkReductions(
  const vector<CombEntry>& centries,
  const vector<Combination>& uniqs,
  const CombEntry& centry,
  const Strategies& strategies,
  const unsigned char maxRank,
  const Distribution& distribution) const
{
  // Get the reduction that underlies the whole method.

  list<unsigned char> winRanksLow;
  unsigned char span;
  unsigned len;

  if (! centry.getMinimalSpans(centries, uniqs, winRanksLow, span, len))
  {
    cout << "WARNRANGE2: The range across minimals is not unique." << endl;

    /*
    for (auto& r: winRanksLow)
      cout << "winRanksLow entry: " << +r << endl;

    cout << strategies.str("strategies");

    for (auto& min: centry.minimals)
    {
      const auto& ceMin = centries[min.holding3];
      cout << ceMin.str();
    }

    cout << endl;
    */

    return;
  }


  unsigned char rankCritical;
  // TODO On the way to a different no-winner encoding.
  if (maxRank - centry.winRankLow != span && 
      (centry.winRankLow != 0 && centry.winRankLow != UCHAR_NOT_SET))
  {
    rankCritical = maxRank - span;

    cout << "WARNCRITICAL2: Moving rank from " << 
      +centry.winRankLow << " to " << +rankCritical << endl;

if (rankCritical > 16)
{
cout << "Entered CR with centry:\n";
cout << centry.str();

for (auto& r: winRanksLow)
  cout << "winRanksLow entry: " << +r << endl;

cout << strategies.str("strategies", true);

cout << "Minimals:\n";
for (auto& min: centry.minimals)
{
  const auto& ceMin = centries[min.holding3];
  cout << ceMin.str();
}

cout << "\nspan " << +span << endl;
cout << "len  " << len << endl;
cout << "maxRank " << +maxRank << endl;


    cout << "Original strategy has different range than minima:\n";
    cout << 
      "Original range " << +maxRank << 
      " - " << +centry.winRankLow <<
      " = " << maxRank - centry.winRankLow << "\n";
    cout << "Minimal span " << +span << endl;
    for (auto& r: winRanksLow)
      cout << "winRanksLow entry: " << +r << endl;

    cout << strategies.str("strategies");

    for (auto& min: centry.minimals)
    {
      const auto& ceMin = centries[min.holding3];
      cout << ceMin.str();
    }
  assert(false);

}
  }
  else
  {
    // Void stays void.
    rankCritical = centry.winRankLow;
  }

  // TODO If specialFlag, should strategies.size() exceed the
  // minimal size?  If not, not?


  Result resultLowest;
  strategies.getResultLowest(resultLowest);

  const auto& reduction = distribution.getReduction(rankCritical);

if (reduction.full2reducedDist.size() != distribution.size())
{
cout << "About to fail on reduction size" << endl;
cout << "reduction size " << reduction.full2reducedDist.size() << endl;
cout << "distribution size " << distribution.size() << endl;
assert(reduction.full2reducedDist.size() == distribution.size());
}

  list<Strategies> strategiesExpanded;
  // Result resultCheck;
  // bool firstFlag = true;

  for (auto& min: centry.minimals)
  {
cout << "Starting min loop " << min.str() << endl;
    const auto& ceMin = centries[min.holding3];
    if (! ceMin.minimalFlag)
    {
      // This should not happen long-term, and short-term it is
      // addressed in checkMinimals().
// cout << "Skipping non-minimal entry\n";
      continue;
    }

cout << "Making space for min " << min.str() << endl;
    strategiesExpanded.emplace_back(Strategies());
    Strategies& strategiesMin = strategiesExpanded.back();

    // Get the strategies for the minimal reference.
    // TODO Assert can be erased later
    assert(ceMin.reference.index < uniqs.size());
    strategiesMin = uniqs[ceMin.reference.index].strategies();
Strategies scopy = strategiesMin;

// cout << "Getting result for min " << min.str() << endl;
    // Expand the strategies up using the reduction.
    const char rankAdder = static_cast<char>(rankCritical) -
      static_cast<char>(ceMin.winRankLow);

// cout << "About to expand min " << min.strSimple() << endl;
/*
if (min.holding3 == 1432)
{
      cout << "MINIMUM BEFORE" << endl;
      cout << "resultLowest " << resultLowest.str(true) << endl;
      cout << "rankCritical " << +rankCritical << endl;
      cout << "rankAdder " << +rankAdder << endl;
      cout << "Reduction" << endl;
      cout << reduction.str() << endl;
      cout << strategies.str("full strategy", true);
      cout << "  minimum: " << min.str() << ", adder " << +rankAdder << endl;
      cout << "  " << scopy.str("before expansion", true) << endl;
      cout << "  " << strategiesMin.str("expansion", true) << endl;
}
*/


    strategiesMin.expand(reduction, rankAdder, min.rotateFlag);
// cout << "Expanded min " << min.str() << endl;
// cout << "  " << strategiesMin.str("expansion", true) << endl;
// cout << "  " << strategies.str("original", true) << endl;

    // The minimums have changed in general.
    Result resultMinNew;
    strategiesMin.getResultLowest(resultMinNew);

// cout << "adding resultMinNew " << resultMinNew.str(true);
    /*
    if (firstFlag)
    {
      // An empty result is better than anything, so we have to
      // start this way.
      resultCheck = resultMinNew;
      firstFlag = false;
    }
    else
      resultCheck += resultMinNew;
      */
// cout << "resultCheck now " << resultCheck.str(true) << endl;


    if (strategiesMin.equalPrimary(strategies, false))
    {
      cout << "MINIMUM MATCH" << endl;
    }
    else
    {
      cout << "MINIMUM MISMATCH" << endl;
      cout << "resultLowest " << resultLowest.str(true);
      cout << "rankCritical " << +rankCritical << endl;
      cout << "Reduction" << endl;
      cout << reduction.str() << endl;
      cout << strategies.str("full strategy", true);
      // cout << strategiesReduced.str("reduced strategy", true);
      cout << "  minimum: " << min.str() << ", adder " << +rankAdder << endl;
      cout << "  " << scopy.str("before expansion", true) << endl;
      cout << "  " << strategiesMin.str("expansion", true) << endl;
    }


  }

  /*
  if (resultLowest != resultCheck)
  {
    cout << "WINNER MISMATCH, size " << centry.minimals.size() << endl;
    cout << "resultLowest " << resultLowest.str(true) << endl;
    cout << "resultCheck  " << resultCheck.str(true) << endl;
  }
  else
    cout << "WINNER MATCH, size " << centry.minimals.size() << endl;
    */

// cout << "Loop fertig" << endl;

  // TODO Checks:
  // * The sum of the minimal winners should be the overall winner.
  //   In other words, the minimal winner should be what the top says.
  // * Each minimal strategy should agree in tricks with the top one.
  // * The minimal ones should merge (strategy by strategy) into
  //   the top one, including ranks in some sense.
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


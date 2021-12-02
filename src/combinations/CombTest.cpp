#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "../distributions/Distributions.h"
#include "../strategies/Strategies.h"
#include "Combination.h"
#include "CombEntry.h"

#include "CombTest.h"


bool CombTest::checkAndFixMinimals(
  const vector<CombEntry>& centries,
  list<CombReference>& minimals) const
{
  // Check that each non-minimal holding refers to minimal ones.
  // We actually follow through and change the minimals.
  // Once ranks are good, this method should no longer be needed.

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


bool CombTest::checkMinimals(
  const vector<CombEntry>& centries,
  const list<CombReference>& minimals) const
{
  for (auto& min: minimals)
    if (! centries[min.holding3].minimalFlag)
      return false;

  return true;
}


void CombTest::checkAllMinimals(vector<CombEntry>& centries)
{
  // TODO Could also use checkMinimals here.
  // Then centries can be const (and the whole method, too).
  // But they have to be correct then...

  for (unsigned holding = 0; holding < centries.size(); holding++)
  {
    if (! CombTest::checkAndFixMinimals(
      centries,
      centries[holding].minimals))
    {
      cout << "ERROR: holding " << holding << " uses non-minimals\n";
    }
  }
}


bool CombTest::getMinimalRanges(
  const vector<CombEntry>& centries,
  const vector<Combination>& uniqs,
  const CombEntry& centry,
  list<unsigned char>& rankLowest,
  unsigned char& range) const
{
  // In general, the strategies in checkReductions may have more
  // strategy's than it should because some play was not considered.
  // For example, 9/1232 (AKT9/Q8) has two strategies with the same
  // tricks, one wiht 4N's (KQ) and one with 2N (K).  This has
  // a pseudo-minimal version 9/1288 (AKT7/Q6) with only one
  // strategy, but still 6 distributions too, 5N'S (KQ).  This has
  // a real minimal version 9/1432 (AK87/Q6) with 3N'S (KQ).
  // This method would then return (5, 3) and 0, where 0 is the
  // difference between the highest and lowest winner among the
  // minimal strategy's (so 5-5 or 3-3).  They should be the same
  // for all minimals.

  unsigned char rankLow, rankHigh;
  bool firstFlag = true;
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

    assert(ceMin.canonical.index < uniqs.size());
    const Strategies& strategiesMin = 
      uniqs[ceMin.canonical.index].strategies();

    strategiesMin.getResultRange(rankLow, rankHigh);
    rankLowest.push_back(rankLow);

    if (firstFlag)
    {
      range = rankHigh - rankLow;
      firstFlag = false;
    }
    else if (rankHigh - rankLow != range)
    {
      cout << "Odd rank arrangement\n";
      return false;
    }
  }
  return true;
}


void CombTest::checkReductions(
  const vector<CombEntry>& centries,
  const vector<Combination>& uniqs,
  const CombEntry& centry,
  const Strategies& strategies,
  const Distribution& distribution) const
{
  // Get the reduction that underlies the whole method.
  // TODO Could store the rank in CombEntry?

  list<unsigned char> rankLowest;
  unsigned char range;
  if (! CombTest::getMinimalRanges(centries, uniqs, centry,
    rankLowest, range))
  {
    cout << "The range across minimals is apparently not unique.\n";
    assert(false);
  }

  unsigned char rankLow, rankHigh, rankCritical;
  strategies.getResultRange(rankLow, rankHigh);
  bool specialFlag;

  if (rankHigh - rankLow != range)
  {
    cout << "Original strategy has different range than minima:\n";
    cout << "Original range " << +rankHigh << " - " << +rankLow <<
      " = " << rankHigh - rankLow << "\n";
    cout << "Minimal range  " << +range << endl;
    for (auto& r: rankLowest)
      cout << "rankLowest entry: " << +r << endl;

    cout << strategies.str("strategies");

    for (auto& min: centry.minimals)
    {
      const auto& ceMin = centries[min.holding3];
      cout << ceMin.str();
    }

    rankCritical = rankHigh - range;
    specialFlag = true;

    cout << "Moving critical rank from " << +rankLow << "to " <<
      +rankCritical << endl;
  }
  else
  {
    rankCritical = rankLow;
    specialFlag = false;
  }

  // TODO If specialFlag, should strategies.size() exceed the
  // minimal size?  If not, not?


  

  Result resultLowest;
  strategies.getResultLowest(resultLowest);
  /*
  const unsigned char rankCritical = resultLowest.rank();

  Result resultHighest;
  strategies.getResultHighest(resultHighest);

cout << "Result lowest  " << resultLowest.str(true);
cout << "Result highest " << resultHighest.str(true);
*/

  const auto& reduction = distribution.getReduction(rankCritical);

if (reduction.full2reducedDist.size() != distribution.size())
{
cout << "About to fail on reduction size" << endl;
assert(reduction.full2reducedDist.size() == distribution.size());
}

  list<Strategies> strategiesExpanded;
  Result resultCheck;
  bool firstFlag = true;

  for (auto& min: centry.minimals)
  {
cout << "Starting min loop " << min.str() << endl;
    const auto& ceMin = centries[min.holding3];
    if (! ceMin.minimalFlag)
    {
      // This should not happen long-term, and short-term it is
      // addressed in checkMinimals().
cout << "Skipping non-minimal entry\n";
      continue;
    }

cout << "Making space for min " << min.str() << endl;
    strategiesExpanded.emplace_back(Strategies());
    Strategies& strategiesMin = strategiesExpanded.back();

    // Get the strategies for the minimal reference.
    // TODO Assert can be erased later
    assert(ceMin.canonical.index < uniqs.size());
    strategiesMin = uniqs[ceMin.canonical.index].strategies();
Strategies scopy = strategiesMin;

cout << "Getting result for min " << min.str() << endl;
    // Expand the strategies up using the reduction.
    Result resultMinLowest;
    strategiesMin.getResultLowest(resultMinLowest);
    const char rankAdder = static_cast<char>(rankCritical) -
      static_cast<char>(resultMinLowest.rank());

cout << "About to expand min " << min.strSimple() << endl;
/* */
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
/* */


    strategiesMin.expand(reduction, rankAdder, min.rotateFlag);
cout << "Expanded min " << min.str() << endl;
cout << "  " << strategiesMin.str("expansion", true) << endl;

    // The minimums have changed in general.
    Result resultMinNew;
    strategiesMin.getResultLowest(resultMinNew);

cout << "adding resultMinNew " << resultMinNew.str(true);
    if (firstFlag)
    {
      // An empty result is better than anything, so we have to
      // start this way.
      resultCheck = resultMinNew;
      firstFlag = false;
    }
    else
      resultCheck += resultMinNew;
cout << "resultCheck now " << resultCheck.str(true) << endl;


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

  if (resultLowest != resultCheck)
  {
    cout << "WINNER MISMATCH, size " << centry.minimals.size() << endl;
    cout << "resultLowest " << resultLowest.str(true) << endl;
    cout << "resultCheck  " << resultCheck.str(true) << endl;
  }
  else
    cout << "WINNER MATCH, size " << centry.minimals.size() << endl;

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
    if (! centry.canonicalFlag || centry.minimalFlag)
      continue;

cout << "Checking:  cards " << cards << ", " <<
  centry.canonical.str() << endl;

if (cards == 9 && holding == 1232)
{
  cout << "HERE\n";
}

    const Combination& comb = uniqs[centry.canonical.index];
    CombTest::checkReductions(centries, uniqs, centry, comb.strategies(), 
      * distributions.ptrNoncanonical(cards, centry.canonical.holding2));
  }
}


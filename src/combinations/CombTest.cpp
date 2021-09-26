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


void CombTest::checkReductions(
  const vector<CombEntry>& centries,
  const vector<Combination>& uniqs,
  const CombEntry& centry,
  const Strategies& strategies,
  const Distribution& distribution) const
{
  // Get the reduction that underlies the whole method.
  // TODO Could store the rank in CombEntry?
  Result resultLowest;
  strategies.getResultLowest(resultLowest);
  const unsigned char rankCritical = resultLowest.rank();
  const auto& reduction = distribution.getReduction(rankCritical);
if (reduction.full2reducedDist.size() != distribution.size())
{
cout << "About to fail on reduction size" << endl;
assert(reduction.full2reducedDist.size() == distribution.size());
}

  // Delete Strategy's where the number of tricks is not constant
  // within each reduction group.  The number of distributions is
  // unchanged.
  Strategies strategiesReduced = strategies;
  strategiesReduced.reduceByTricks(reduction);

  list<Strategies> strategiesExpanded;

  for (auto& min: centry.minimals)
  {
    const auto& ceMin = centries[min.holding3];
    if (! ceMin.minimalFlag)
    {
      // This should not happen long-term, and short-term it is
      // addressed in checkMinimals().
      continue;
    }

    strategiesExpanded.emplace_back(Strategies());
    Strategies& strategiesMin = strategiesExpanded.back();

    // Get the strategies for the minimal reference.
    // TODO Assert can be erased later
    assert(ceMin.canonical.index < uniqs.size());
    strategiesMin = uniqs[ceMin.canonical.index].strategies();
Strategies scopy = strategiesMin;

    // Expand the strategies up using the reduction.
    Result resultMinLowest;
    strategiesMin.getResultLowest(resultMinLowest);
    const char rankAdder = static_cast<char>(rankCritical) -
      static_cast<char>(resultMinLowest.rank());

    strategiesMin.expand(reduction, rankAdder, min.rotateFlag);

    if (strategiesMin == strategiesReduced)
    {
      cout << "MINIMUM MATCH\n";
    }
    else
    {
      cout << "MINIMUM MISMATCH\n";
      cout << "resultLowest " << resultLowest.str(true) << endl;
      cout << "rankCritical " << +rankCritical << endl;
      cout << "Reduction\n";
      cout << reduction.str() << endl;
      cout << strategies.str("full strategy", true);
      cout << strategiesReduced.str("reduced strategy", true);
      cout << "  minimum: " << min.str() << ", adder " << +rankAdder << endl;
      cout << "  " << scopy.str("before expansion", true) << endl;
      cout << "  " << strategiesMin.str("expansion", true) << endl;
    }


  }

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

cout << "Checking: " <<
  centry.canonical.str() << endl;

if (cards == 5 && holding == 164)
{
  cout << "HERE\n";
}

    const Combination& comb = uniqs[centry.canonical.index];
    CombTest::checkReductions(centries, uniqs, centry, comb.strategies(), 
      * distributions.ptrNoncanonical(cards, centry.canonical.holding2));
  }
}


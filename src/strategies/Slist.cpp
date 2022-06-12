/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Slist.h"
#include "StratData.h"

#include "result/Ranges.h"
#include "../plays/Play.h"

#include "../distributions/Distribution.h"
#include "../distributions/covers/Covers.h"
#include "../distributions/covers/tableau/CoverTableau.h"

#include "../combinations/DepthStats.h"

#include "../inputs/Control.h"

// TODO TMP
#include "../utils/Timer.h"

extern Control control;


Slist::Slist()
{
  Slist::clear();
}


/************************************************************
 *                                                          *
 * Simple methods                                           *
 *                                                          *
 ************************************************************/

void Slist::clear()
{
  strategies.clear();
  resultWestVoid.clear();
  resultEastVoid.clear();
}


void Slist::resize(const size_t len)
{
  strategies.resize(len);
}


const Strategy& Slist::front() const
{
  return strategies.front();
}


void Slist::push_back(const Strategy& strategy)
{
  strategies.push_back(strategy);
}


size_t Slist::size() const
{
  return strategies.size();
}


bool Slist::empty() const
{
  return (strategies.empty() || strategies.front().empty());
}


/************************************************************
 *                                                          *
 * Ways in which a new Slist arises: setTrivial and adapt   *
 *                                                          *
 ************************************************************/

void Slist::setTrivial(
  const Result& trivial,
  const unsigned char len)
{
  // Repeat the trivial result len times.

  Slist::clear();
  strategies.emplace_back(Strategy());
  Strategy& strat = strategies.back();

  strat.logTrivial(trivial, len);
  strat.studyOppsVoid(resultWestVoid, resultEastVoid);
}


void Slist::adapt(
  const Play& play,
  const SurvivorList& survivors,
  const size_t distSize,
  const bool symmOnlyFlag)
{
  // Adapt the Slist of a following play to this trick by
  // rotating, mapping etc.  This is a somewhat expensive method.

  bool westVoidFlag, eastVoidFlag;
  play.setVoidFlags(westVoidFlag, eastVoidFlag);
  assert(! westVoidFlag || ! eastVoidFlag);

  if (westVoidFlag || eastVoidFlag)
  {
    // Make use of the pre-stored results when one opponent is void.

    strategies.clear();
    strategies.emplace_back(Strategy());
    Strategy& strat = strategies.front();

    strat.setAndAdaptVoid(
      play,
      resultWestVoid, 
      resultEastVoid,
      westVoidFlag, 
      eastVoidFlag,
      survivors.front().fullNo,
      distSize,
      symmOnlyFlag);
  }
  else
  {
    for (auto& strat: strategies)
      strat.adapt(play, survivors, distSize, symmOnlyFlag);

    if (strategies.size() > 1)
      Slist::consolidate(&Strategy::lessEqualCompleteBasic);
  } 
}


void Slist::forgetDetails()
{
  // Forget the strategies, but keep the void results.

  strategies.clear();
}


/************************************************************
 *                                                          *
 * Consolidating an Slist                                   *
 *                                                          *
 ************************************************************/

unsigned Slist::consolidateAnyTwo(
  list<Strategy>::iterator& iter1,
  list<Strategy>::iterator& iter2,
  ComparatorType lessEqualMethod)
{
  // Check whether to swap the two and whether one is dominated.

  if (iter1->weight() < iter2->weight())
  {
    if (((* iter1).*lessEqualMethod)(* iter2))
    {
      iter1 = strategies.erase(iter1);
      return 1; // Erased #1
    }
    else
    {
      iter_swap(iter1, iter2);
      return 3; // Swapped
    }
  }
  else if (iter2->weight() < iter1->weight())
  {
    if (((* iter2).*lessEqualMethod)(* iter1))
    {
      iter2 = strategies.erase(iter2);
      return 2;
    }
  }
  else
  {
    // The weights are the same, but the trick distribution could
    // still be different.  So we can't use compareSecondary and we
    // have to use compareCompleteBasic.
    const Compare c = iter1->compareCompleteBasic(* iter2);
    if (c == WIN_FIRST || c == WIN_EQUAL)
    {
      iter2 = strategies.erase(iter2);
      return 2;
    }
    else if (c == WIN_SECOND)
    {
      iter1 = strategies.erase(iter1);
      return 1;
    }
  }
  return 0;
}


void Slist::consolidateTwo(ComparatorType lessEqualMethod)
{
  auto iter1 = strategies.begin();
  auto iter2 = next(iter1);
  Slist::consolidateAnyTwo(iter1, iter2, lessEqualMethod);
}


void Slist::consolidateGeneral(ComparatorType lessEqualMethod)
{
  // Used when a strategy may have gone out of order or
  // dominations may have arisen.

  auto oldStrats = move(strategies);
  strategies.clear();

  for (auto& strat: oldStrats)
    Slist::addStrategy(strat, lessEqualMethod);
}


void Slist::consolidate(ComparatorType lessEqualMethod)
{
  for (auto& strategy: strategies)
    strategy.study();

  const size_t s = strategies.size();

  if (s <= 1 || Slist::empty())
    return;
  else if (s == 2)
    Slist::consolidateTwo(lessEqualMethod);
  else
  {
    Slist::consolidateGeneral(lessEqualMethod);
  }
}


void Slist::symmetrize()
{
  // This method is mainly used to reduce complexity in cases where
  // declarer has a void.
  auto iter = strategies.begin();
  while (iter != strategies.end())
  {
    // TODO For now we only compare tricks.  Longer-term the ranks 
    // should match as well in fully symmetric strategies such as
    // 10/45260.  Here we would currently only get 6 and not 8
    // symmetric strategies if we insist on rank equality.
    if (iter->symmetricPrimary())
      iter++;
    else
      iter = strategies.erase(iter);
  }
}


/************************************************************
 *                                                          *
 * Rank-check help methods, mostly                          *
 *                                                          *
 ************************************************************/

bool Slist::reduceByResults(const Distribution& dist)
{
  // Delete Strategy's where the number of tricks is not constant
  // within each reduction group.  The number of distributions is
  // unchanged.  Returns true if a change was made.

  bool changeFlag = false;
  for (auto& strat: strategies)
  {
    if (! strat.reduceByResults(dist))
      changeFlag = true;

    strat.study();
  }

  if (changeFlag)
  {
    ComparatorType lessEqualMethod = (control.runRankComparisons() ?
      &Strategy::lessEqualCompleteStudied :
      &Strategy::lessEqualPrimaryStudied);

    Slist::consolidate(lessEqualMethod);
  }

  return changeFlag;
}


void Slist::expand(
  const Reduction& reduction,
  const char rankAdder,
  const bool rotateFlag)
{
  // Expand the strategies up using the reduction.
  for (auto& strategy: strategies)
    strategy.expand(reduction, rankAdder, rotateFlag);

  if (! Slist::ordered())
  {
    // This is a bit overkill, as they just need to be sorted by weight.
    ComparatorType lessEqualMethod = (control.runRankComparisons() ?
      &Strategy::lessEqualCompleteStudied :
      &Strategy::lessEqualPrimaryStudied);

    Slist::consolidate(lessEqualMethod);
  }
}


/************************************************************
 *                                                          *
 * operator == and two helper methods                       *
 *                                                          *
 ************************************************************/

bool Slist::sameOrdered(
  const Slist& slist2,
  ComparatorType equalMethod) const
{
  // This assumes the same ordering.

  auto iter2 = slist2.strategies.begin();

  for (auto iter1 = strategies.begin(); iter1 != strategies.end(); 
      iter1++, iter2++)
  {
    if (! ((* iter1).*equalMethod)(* iter2))
      return false;
  }

  return true;
}


bool Slist::sameUnordered(
  const Slist& slist2,
  ComparatorType equalMethod) const
{
  for (auto iter1 = strategies.begin(); iter1 != strategies.end(); iter1++)
  {
    bool sameFlag = false;
    for (auto iter2 = slist2.strategies.begin(); 
        iter2 != slist2.strategies.end(); iter2++)
    {
      if (((* iter1).*equalMethod)(* iter2))
      {
        sameFlag = true;
        break;
      }
    }

    if (! sameFlag)
      return false;
  }

  return true;
}


bool Slist::operator == (const Slist& slist2) const
{
  return Slist::equalByMethod(slist2, &Strategy::equalCompleteBasic);
}


bool Slist::equalByMethod(
  const Slist& slist2,
  ComparatorType equalMethod,
  const bool enforceSizeFlag) const
{
  // TODO
  // This is dreadfully slow, but it is mainly used for debugging.
  // If we did want to speed this up, we would loop over i and then
  // over j >= i (upper triangle of matrix).  By starting at j = i
  // we would get the frequent case where the lists are ordered.
  // Then we'd add some methods in Comparer where we would stop
  // if a row had no equal, or a column had more than one equal.

  if (enforceSizeFlag && Slist::size() != slist2.size())
    return false;
  else if (enforceSizeFlag && Slist::sameOrdered(slist2, equalMethod))
    return true;
  else
    return Slist::sameUnordered(slist2, equalMethod);
}


/************************************************************
 *                                                          *
 * addStrategy (+= strat) and helper methods                *
 *                                                          *
 ************************************************************/

bool Slist::addendDominatedHeavier(
  list<Strategy>::iterator& iter,
  ComparatorType lessEqualMethod,
  const Strategy& addend) const
{
  while (iter != strategies.end() && iter->weight() > addend.weight())
  {
    // Only the new strat may be dominated.
    // Only a trick comparison is needed.
    if ((addend.*lessEqualMethod)(* iter))
      return true;
    else
      iter++;
  }
  return false;
}


bool Slist::processSameWeights(
  list<Strategy>::iterator& iter,
  list<Strategy>::iterator& iterEnd,
  ComparatorType lessEqualMethod,
  const Strategy& addend)
{
  // TODO I'm not entirely convinced that the code was not intended
  // for primary comparisons.  But it lessEqualMethod already considers
  // secondaries, do we need compareSecondary?

  while (iter != iterEnd && iter->weight() == addend.weight())
  {
    // Here it could go either way, and we have to look in detail.
    if ((addend.*lessEqualMethod)(* iter))
    {
      // They are the same weight and the tricks are identical.
      // The dominance could go either way, or they may be different.
      const Compare c = iter->compareSecondary(addend);
      if (c == WIN_FIRST || c == WIN_EQUAL)
        return true;
      else if (c == WIN_SECOND)
        iter = strategies.erase(iter);
      else
        iter++;
    }
    else if (((* iter).*lessEqualMethod)(addend))
    {
      // TODO So effectively we always do this as long as the trick
      // profiles are different?

      const Compare c = iter->compareSecondary(addend);
      if (c == WIN_FIRST || c == WIN_EQUAL)
        return true;
      else if (c == WIN_SECOND)
        iter = strategies.erase(iter);
      else
        iter++;
    }
    else
      iter++;
  }
  return false;
}


void Slist::eraseDominatedLighter(
  list<Strategy>::iterator& iter,
  ComparatorType lessEqualMethod,
  const Strategy& addend)
{
  while (iter != strategies.end())
  {
    if (((* iter).*lessEqualMethod)(addend))
      iter = strategies.erase(iter);
    else
      iter++;
  }
}


void Slist::addStrategy(
  const Strategy& strat,
  ComparatorType lessEqualMethod)
{
  // The strategies list is in descending order of weights.
  // The new Strategy might dominate everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  
  if (strategies.empty())
  {
    // This happens in consolidate() with the first entry.
    strategies.push_back(strat);
    return;
  }

  auto iter = strategies.begin();

  if (Slist::addendDominatedHeavier(iter, lessEqualMethod, strat))
    // The new strat is dominated by a Strategy with more weight.
    return;

  auto iterEnd = strategies.end();
  if (Slist::processSameWeights(iter, iterEnd, lessEqualMethod, strat))
    // The new strat is dominated by a Strategy with equal weight.
    return;

  // The new vector must be inserted.  This consumes about a third
  // of the time of the overall method.
  iter = next(strategies.insert(iter, strat));

  // The new vector may dominate lighter vectors. This only consumes
  // 5-10% of the overall time.
  Slist::eraseDominatedLighter(iter, lessEqualMethod, strat);
}


void Slist::addStrategyInplace(ComparatorType lessEqualMethod)
{
  // For efficiency the product was placed straight into the back
  // of slist.  It may dominate or be dominated by others, or it
  // may be in the wrong place.

  if (strategies.size() == 1)
  {
    // Keep the product, so make a new scratch-pad element.
    strategies.emplace_back(Strategy());
    return;
  }

  // The strategies list is in descending order of weights.
  // The new Strategy might dominate everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  // This checking costs about one third of the overall method time.
  
  auto iter = strategies.begin();
  auto piter = prev(strategies.end());

  if (Slist::addendDominatedHeavier(iter, lessEqualMethod, * piter))
    // piter is dominated by a Strategy with more weight.
    return;


  if (Slist::processSameWeights(iter, piter, lessEqualMethod, * piter))
    // piter is dominated by a Strategy with equal weight.
    return;


  if (iter == piter)
  {
    // Already in the right place at the end.
    // Keep the product, so make a new scratch-pad element.
    strategies.emplace_back(Strategy());
    return;
  }

  // The new vector must be inserted, i.e. spliced in.
  // This is super-fast.
  strategies.splice(iter, strategies, piter);
  piter = prev(iter);

  // The new vector may dominate lighter vectors.  This is also
  // quite efficient and doesn't happen so often.
  Slist::eraseDominatedLighter(iter, lessEqualMethod, * piter);

  // Make a new scratch-pad element.
  strategies.emplace_back(Strategy());
}


/************************************************************
 *                                                          *
 * addStrategies (+= strats) and helper methods             *
 *                                                          *
 ************************************************************/

void Slist::markChanges(
  const Slist& slist2,
  list<Addition>& additions,
  list<list<Strategy>::const_iterator>& deletions,
  ComparatorType lessEqualMethod) const
{
  // A simple way is to add an individual strategy to the LHS if it is 
  // not dominated.  Then the following strategies from the RHS are 
  // also compared to this, but we already know that there is no point.  
  // This is a more complicated way to do the comparisons up front 
  // between all LHS and RHS pairs, but only updating LHS at the end.

  vector<unsigned> ownDeletions(strategies.size(), 0);

  for (auto& strat: slist2.strategies)
  {
    auto iter = strategies.begin();
    unsigned stratNo = 0;
    bool doneFlag = false;
    while (iter != strategies.end() && iter->weight() > strat.weight())
    {
      if (ownDeletions[stratNo] ||
          ! (strat.*lessEqualMethod)(* iter))
      {
        iter++;
        stratNo++;
        continue;
      }
      else
      {
        doneFlag = true;
        break;
      }
    }

    if (doneFlag)
      continue;

    while (iter != strategies.end() && iter->weight() == strat.weight())
    {
      if (ownDeletions[stratNo] ||
          ! (strat.*lessEqualMethod)(* iter))
      {
        iter++;
        stratNo++;
        continue;
      }

      // Now the trick vectors are identical.
      const Compare c = strat.compareSecondary(* iter);
      if (c == WIN_EQUAL || c == WIN_SECOND)
      {
        doneFlag = true;
        break;
      }
      else if (c == WIN_FIRST)
      {
        deletions.push_back(iter);
        ownDeletions[stratNo] = 1;
      }

      iter++;
      stratNo++;
    }

    if (doneFlag)
      continue;

    // Note for insertion.
    additions.emplace_back(Addition());
    auto& addition = additions.back();
    addition.ptr = &strat;
    addition.iter = iter;

    // The new vector may dominate lighter vectors.
    while (iter != strategies.end())
    {
      if (((* iter).*lessEqualMethod)(strat) &&
          iter->lessEqualCompleteBasic(strat))
      {
        if (ownDeletions[stratNo] == 0)
        {
          deletions.push_back(iter);
          ownDeletions[stratNo] = 1;
        }
      }
      iter++;
      stratNo++;
    }
  }
}


void Slist::processChanges(
  list<Addition>& additions,
  list<list<Strategy>::const_iterator>& deletions)
{
  for (auto& addition: additions)
    strategies.insert(addition.iter, *(addition.ptr));

  for (auto& deletion: deletions)
    strategies.erase(deletion);
}


void Slist::addStrategies(
  const Slist& slist2,
  ComparatorType lessEqualMethod)
{
  list<Addition> additions;
  list<list<Strategy>::const_iterator> deletions;

  Slist::markChanges(slist2, additions, deletions, lessEqualMethod);

  Slist::processChanges(additions, deletions);
}


void Slist::plusOneByOne(const Slist& slist2)
{
  // Simple version when both Slist's are known to have size 1,
  // as happens very frequently.

  const auto& str1 = strategies.front();
  const auto& str2 = slist2.strategies.front();

  const Compare c = str1.compareCompleteStudied(str2);
  
  if (c == WIN_FIRST || c == WIN_EQUAL)
    return;
  else if (c == WIN_SECOND)
    * this = slist2;
  else
  {
    if (str1.weight() >= str2.weight())
      strategies.push_back(str2);
    else
      strategies.push_front(str2);
  }
}


/************************************************************
 *                                                          *
 * operator *= Strategy                                     *
 *                                                          *
 ************************************************************/

void Slist::multiply(
  const Strategy& strat,
  ComparatorType lessEqualMethod,
  const bool consolidateFlag)
{
  // Only turn off consolidateFlag if we're sure that multiplying
  // by strat does not cause Slist to unconsolidate.
  // The input method is only used for consolidating.

  if (strategies.empty())
    strategies.push_back(strat);
  else
  {
    for (auto& strat1: strategies)
      strat1 *= strat;
    
    if (consolidateFlag)
      Slist::consolidate(lessEqualMethod);
  }
}


/************************************************************
 *                                                          *
 * operator *= Strategies                                   *
 *                                                          *
 ************************************************************/

void Slist::multiplyOneByOne(const Slist& slist2)
{
  strategies.front() *= slist2.front();
}


void Slist::multiply(
  const Slist& slist2,
  const Ranges& ranges,
  ComparatorType lessEqualMethod)
{
  // This is the only place where scrutinize is used, and it's a
  // bit annoying, but hard to avoid.  We'd have to have this whole
  // loop in Strategies.
  auto strategiesOwn = move(strategies);
  strategies.clear();
  strategies.emplace_back(Strategy());

  for (auto& strat1: strategiesOwn)
    for (auto& strat2: slist2.strategies)
    {
      auto& product = strategies.back();
      product.multiply(strat1, strat2);
      if (! ranges.empty())
        product.scrutinize(ranges);

      Slist::addStrategyInplace(lessEqualMethod);
    }

  strategies.pop_back();
}


/************************************************************
 *                                                          *
 * Splits                                                   *
 *                                                          *
 ************************************************************/

void Slist::splitDistributions(
  const Strategy& counterpart,
  Slist& own,
  Slist& shared)
{
  // Split our strategies by distribution into one group (own) with
  // those distributions that are unique to us, and another (shared)
  // with distributions that overlap.  This is relative to counterpart.

  // List of iterators to a Result of each Strategy in stratsToSplit.
  // All are in sync to point to a given distribution.
  // They move in sync down across the Strategy's.
  StratData stratData;
  stratData.fill(strategies);

  auto riter = counterpart.begin();
  while (true)
  {
    if (riter == counterpart.end())
    {
      // A unique distribution.
      stratData.pushDistributionOnto(own.strategies);

      if (stratData.advance() == STRATSTATUS_END)
        break;
      else
        continue;
    }

    if (riter->getDist() < stratData.dist())
    {
      // Distribution that is only in strat2.
      riter++;
      continue;
    }

    bool endFlag = false;
    while (stratData.dist() < riter->getDist())
    {
      // A unique distribution.
      stratData.pushDistributionOnto(own.strategies);

      if (stratData.advance() == STRATSTATUS_END)
      {
        endFlag = true;
        break;
      }
    }

    if (endFlag)
      break;

    if (riter->getDist() == stratData.dist())
    {
      // A shared distribution.
      stratData.pushDistributionOnto(shared.strategies);

      if (stratData.advance() == STRATSTATUS_END)
        break;
    }

    riter++;
  }
}


/************************************************************
 *                                                          *
 * Ranges                                                   *
 *                                                          *
 ************************************************************/

void Slist::makeRanges(Ranges& ranges) const
{
  if (strategies.empty())
    return;

  strategies.front().initRanges(ranges);

  if (strategies.size() == 1)
    return;

  for (auto iter = next(strategies.begin());
      iter != strategies.end(); iter++)
    iter->extendRanges(ranges);
}


bool Slist::purgeRanges(
  Strategy& constants,
  const Ranges& rangesOwn,
  const Ranges& rangesParent,
  const bool debugFlag)
{
  // Make a list of iterators -- one per Strategy.
  // The iterators later step through one "row" (distribution) of
  // all Strategy's in synchrony.
  StratData stratData;
  stratData.fill(strategies);
  stratData.riter = rangesOwn.begin();

  constants.resize(rangesParent.size());
  auto citer = constants.begin();

  bool eraseFlag = false;

  for (auto& parentRange: rangesParent)
  {
    // Get to the same distribution in each Strategy if it exists.
    const StratStatus status = stratData.advance(parentRange.dist());
    if (status == STRATSTATUS_END)
      break;
    else if (status == STRATSTATUS_FURTHER_DIST)
      continue;

    if (parentRange.constant())
    {
      parentRange.constantResult(* citer);
      stratData.eraseDominatedDist();
      eraseFlag = true;
      citer++;

      if (debugFlag)
      {
        cout << "Erased constant for parent range:\n";
        cout << parentRange.strHeader(control.runRankComparisons());
        cout << parentRange.str(control.runRankComparisons());
      }
    }
    else if (parentRange < * stratData.riter)
    {
      stratData.eraseDominatedDist();
      eraseFlag = true;

      if (debugFlag)
      {
        cout << "Erased dominated range for parent range:\n";
        cout << parentRange.strHeader(control.runRankComparisons());
        cout << parentRange.str(control.runRankComparisons());
      }
    }
  }

  // Shrink to the size used.
  constants.eraseRest(citer);

  if (eraseFlag)
    Slist::consolidate(&Strategy::lessEqualPrimaryStudied);

  return eraseFlag;
}


/************************************************************
 *                                                          *
 * Results and winners methods                              *
 *                                                          *
 ************************************************************/

void Slist::studyOppsVoid()
{
  // It is somewhat helpful to know the results across all strategies
  // when either opponent is known to be void.  First, it helps a
  // little bit when adapting strategies from later plays.  Second,
  // it lets us forget almost entire Strategies in more cases.
  // For example, we can reach 11-card combinations if both North
  // and West are void, but not without an opposing void.
  
  resultWestVoid.clear();
  resultEastVoid.clear();

  for (const auto& strat: strategies)
    strat.studyOppsVoid(resultWestVoid, resultEastVoid);
}


void Slist::getResultLowest(Result& result) const
{
  for (const auto& strat: strategies)
    result *= strat.resultLowest();
}


void Slist::getResultList(list<Result>& resultList) const
{
  resultList.clear();
  for (const auto& strat: strategies)
  {
    const auto& res = strat.resultLowest();
    if (res.winAbsNumber() > 0)
      resultList.push_back(res);
  }

  if (resultList.size() <= 1)
    return;

  resultList.sort([](const Result& res1, const Result& res2)
  {
    return (res1.winAbsNumber() < res2.winAbsNumber());
  });

  resultList.unique([](const Result& res1, const Result& res2)
  {
    return (res1.winAbsNumber() == res2.winAbsNumber());
  });
}


/************************************************************
 *                                                          *
 * Utilities                                                *
 *                                                          *
 ************************************************************/


void Slist::coversManual(
  Covers& coversIn,
  const unsigned char maxRank) const
{
  unsigned stratNo = 0;
  CoverTableau tableau;

  for (auto& strat: strategies)
  {
    const Result result = strat.resultLowest();
    const unsigned rankLow = result.getRank();
    if (rankLow+2 < maxRank)
    {
      // We don't know yet how to cover such Strategy's.
      stratNo++;
      continue;
    }

    strat.coversManual(coversIn, tableau);
    cout << "Manual strategy #" << stratNo << " ";
    cout << tableau.strBracket() << ": ";

    if (tableau.complete())
      cout << tableau.str(coversIn.getSumProfile());
    else
      cout << strat.str("Unexplained manual", true) << "\n";
    
    stratNo++;
  }
}


void Slist::covers(
  Covers& coversIn,
  const unsigned char maxRank,
  ProductStats& productStats,
  [[maybe_unused]] DepthStats& depthStats) const
{
  unsigned stratNo = 0;
  CoverTableau tableau;

  Timer timer;

  for (auto& strat: strategies)
  {
    const Result result = strat.resultLowest();
    const unsigned rankLow = result.getRank();

    // If opponents have e.g. two tops, then we consider their
    // depth to be 1, as we don't have to specify the lowest one
    // explicitly.
    const size_t nominalDepth = coversIn.getSumProfile().size() - 1;

    const size_t actualDepth =
      (rankLow == numeric_limits<unsigned char>::max() ?
      0 : (maxRank + 1 - rankLow) / 2);

/*
cout << "maxRank " << +maxRank << endl;
cout << "rankLow " << +rankLow << endl;
cout << "nominal " << nominalDepth << endl;
cout << "actual  " << actualDepth << endl;

if (strat.symmetricPrimary())
  cout << "STRATSYMM " << actualDepth << "\n";
else
  cout << "STRATGEN " << actualDepth << "\n";
*/

    depthStats.increment(nominalDepth, actualDepth);

    // if (rankLow+2 < maxRank)
    if (actualDepth > control.verbalDepth())
    {
      // We don't know yet how to cover such Strategy's.
      stratNo++;
      continue;
    }

    bool newTableauFlag;
    timer.start();
    strat.covers(coversIn, 
      static_cast<unsigned char>(actualDepth), 
      tableau, newTableauFlag);
    timer.stop();

    cout << "Strategy #" << stratNo << " ";
    cout << tableau.strBracket() << ": ";

    if (tableau.complete())
    {
      cout << tableau.str(coversIn.getSumProfile());
      tableau.updateStats(
        coversIn.getSumProfile(), 
        productStats, 
        newTableauFlag);
    }
    else
      cout << strat.str("Unexplained", true) << "\n";

    cout << "Took " << timer.str(2);
    
    stratNo++;
  }
}


bool Slist::ordered() const
{
  // Check that the strategies are ordered by descending weight.
  for (auto iter = strategies.begin(); iter != prev(strategies.end()); 
      iter++)
  {
    if (iter->weight() < next(iter)->weight())
      return false;
  }
  return true;
}


bool Slist::minimal() const
{
  // This is only for diagnostics.
  if (strategies.size() <= 1)
    return true;

  unsigned i = 0;
  for (auto iter1 = strategies.begin(); iter1 != prev(strategies.end());
      iter1++, i++)
  {
    unsigned j = i+1;
    for (auto iter2 = next(iter1); iter2 != strategies.end(); iter2++, j++)
      if (iter1->compareCompleteBasic(* iter2) != WIN_DIFFERENT)
      {
        cout << "Minimal violation " << i << ", " << j << endl;
        return false;
      }
  }
  return true;
}


bool Slist::constantTricks() const
{
  for (auto& strategy: strategies)
    if (! strategy.constantTricks())
      return false;

  return true;
}


bool Slist::sameTricks() const
{
  StratData stratData;
  stratData.fill(strategies);

  while (! stratData.done())
  {
    if (! stratData.sameTricks())
      return false;
    stratData.advance();
  }

  return true;
}


/************************************************************
 *                                                          *
 * string methods                                           *
 *                                                          *
 ************************************************************/

string Slist::strHeader(
  const string& title,
  const bool rankFlag) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";

  const unsigned incr = (rankFlag ? 10 : 4);

  ss << setw(4) << left << "Dist" << right;

  for (unsigned i = 0; i < strategies.size(); i++)
    ss << setw(static_cast<int>(incr)) << i;
  ss << "\n";

  ss << string(4 + incr * strategies.size(), '-') << "\n";

  return ss.str();
}


string Slist::strSumLine(const bool rankFlag) const
{
  stringstream ss;

  const size_t dashes = 4 + (rankFlag ? 10 : 4) * strategies.size();
  ss << string(dashes, '-') << "\n";
  ss << setw(4) << left << "Sum" << right;

  if (rankFlag)
  {
    Result resLowest;
    for (const auto& strat: strategies)
    {
      const Result res = strat.resultLowest();
      ss << setw(6) << strat.weight() << setw(4) << res.strWinners();
      resLowest *= res;
    }

    if (strategies.size() > 1)
      ss << "\n" <<
        setw(4) << "Prod" <<
        setw(10) << resLowest.strWinners() << "\n";
  }
  else
  {
    for (const auto& strat: strategies)
      ss << setw(4) << strat.weight();
  }
  return ss.str() + "\n";
}


string Slist::str(
  const string& title,
  const bool rankFlag) const
{
  if (strategies.size() == 0)
    return "";

  stringstream ss;
  ss << Slist::strHeader(title, rankFlag);

  StratData stratData;
  stratData.fill(strategies);

  while (! stratData.done())
  {
    ss << setw(4) << left << +stratData.dist() << right;

    for (auto& sd: stratData.data)
      ss << sd.iter->strEntry(rankFlag);

    ss << "\n";

    stratData.advance();
  }

  ss << Slist::strSumLine(rankFlag);

  return ss.str();
}


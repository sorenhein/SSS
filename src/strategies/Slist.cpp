/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Slist.h"
#include "StratData.h"

#include "result/Ranges.h"
#include "../plays/Play.h"
#include "../Survivor.h"


Slist::Slist()
{
  Slist::clear();
}


Slist::~Slist()
{
}


/************************************************************
 *                                                          *
 * Simple methods                                           *
 *                                                          *
 ************************************************************/

void Slist::clear()
{
  strategies.clear();
}


void Slist::resize(const unsigned len)
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


unsigned Slist::size() const
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
  strategies.back().logTrivial(trivial, len);
}


void Slist::collapseOnVoid()
{
  assert(strategies.size() > 0);
  assert(strategies.front().size() == 1);

  if (strategies.size() == 1)
    return;

  // Find the best one for declarer, as everything is revealed.
  auto& first = strategies.front();
  for (auto iter = next(strategies.begin()); 
      iter != strategies.end(); iter++)
  {
    first.addComponentwise(* iter);
  }

  // Only keep the first, best one.
  strategies.erase(next(strategies.begin()), strategies.end());
}


void Slist::adapt(
  const Play& play,
  const Survivors& survivors)
{
  // Adapt the Slist of a following play to this trick by
  // rotating, mapping etc.  This is a somewhat expensive method.

  for (auto& strat: strategies)
    strat.adapt(play, survivors);

  if (play.lhoPtr->isVoid() || play.rhoPtr->isVoid())
    Slist::collapseOnVoid();

  if (strategies.size() > 1 && ! Slist::ordered())
  {
    strategies.sort([](const Strategy& strat1, const Strategy& strat2)
    {
      return (strat1.weight() > strat2.weight());
    });
  }
}


/************************************************************
 *                                                          *
 * Consolidating an Slist                                   *
 *                                                          *
 ************************************************************/

void Slist::consolidateTwo(ComparatorType lessEqualMethod)
{
  // Check whether to swap the two and whether one is dominated.
  auto iter1 = strategies.begin();
  auto iter2 = next(iter1);

  if (iter1->weight() < iter2->weight())
  {
    if (((* iter1).*lessEqualMethod)(* iter2))
      strategies.pop_front();
    else
      iter_swap(iter1, iter2);
  }
  else if (iter2->weight() < iter1->weight())
  {
    if (((* iter2).*lessEqualMethod)(* iter1))
      strategies.pop_back();
  }
  else
  {
    // TODO Same weight: Can take advantage?
    const Compare c = iter1->compareCompleteBasic(* iter2);
    if (c == WIN_FIRST || c == WIN_EQUAL)
      strategies.pop_back();
    else if (c == WIN_SECOND)
      strategies.pop_front();
  }
}


void Slist::consolidate(ComparatorType lessEqualMethod)
{
  // Used when a strategy may have gone out of order or
  // dominations may have arisen.
  auto oldStrats = move(strategies);
  strategies.clear();

  for (auto& strat: oldStrats)
    Slist::addStrategy(strat, lessEqualMethod);
}


/************************************************************
 *                                                          *
 * operator == and two helper methods                       *
 *                                                          *
 ************************************************************/

bool Slist::sameOrdered(const Slist& slist2) const
{
  // This assumes the same ordering.

  auto iter2 = slist2.strategies.begin();

  for (auto iter1 = strategies.begin(); iter1 != strategies.end(); 
      iter1++, iter2++)
  {
    if (!(* iter1 == * iter2))
      return false;
  }

  return true;
}


bool Slist::sameUnordered(const Slist& slist2) const
{
  for (auto iter1 = strategies.begin(); iter1 != strategies.end(); iter1++)
  {
    bool sameFlag = false;
    for (auto iter2 = slist2.strategies.begin(); 
        iter2 != slist2.strategies.end(); iter2++)
    {
      if (* iter1 == * iter2)
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
  // This is dreadfully slow, but it is mainly used for debugging.
  // If we did want to speed this up, we would loop over i and then
  // over j >= i (upper triangle of matrix).  By starting at j = i
  // we would get the frequent case where the lists are ordered.
  // Then we'd add some methods in Comparer where we would stop
  // if a row had no equal, or a column had more than one equal.

  if (Slist::size() != slist2.size())
    return false;
  else if (Slist::sameOrdered(slist2))
    return true;
  else
    return Slist::sameUnordered(slist2);
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
  list<list<Strategy>::const_iterator>& deletions) const
{
  // The simple Strategies += Strategies adds an individual strategy
  // to the LHS if it is not dominated.  Then the following
  // strategies from the RHS are also compared to this, but we already
  // know that there is no point.  This is a more complicated way
  // to do the comparisons up front between all LHS and RHS pairs,
  // but only updating LHS at the end.

  vector<unsigned> ownDeletions(strategies.size(), 0);

  for (auto& strat: slist2.strategies)
  {
    auto iter = strategies.begin();
    unsigned stratNo = 0;
    bool doneFlag = false;
    while (iter != strategies.end() && iter->weight() > strat.weight())
    {
      if (ownDeletions[stratNo] ||
          ! strat.lessEqualPrimaryScrutinized(* iter))
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
          ! strat.lessEqualPrimaryScrutinized(* iter))
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
      // TODO Is there a more target secondary comparison?
      // Does it matter much?
      if (iter->lessEqualPrimaryScrutinized(strat) &&
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
  for (auto& strategy: slist2.strategies)
    Slist::addStrategy(strategy, lessEqualMethod);
}


void Slist::addStrategiesScrutinized(const Slist& slist2)
{
  list<Addition> additions;
  list<list<Strategy>::const_iterator> deletions;

  Slist::markChanges(slist2, additions, deletions);

  Slist::processChanges(additions, deletions);
}


void Slist::plusOneByOne(const Slist& slist2)
{
  // Simple version when both Slist's are known to have size 1,
  // as happens very frequently.

  const auto& str1 = strategies.front();
  const auto& str2 = slist2.strategies.front();

  const CompareType c = str1.compareCompleteStudied(str2);
  
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

void Slist::operator *= (const Strategy& strat)
{
  // This does not re-sort and consolidate strategies.  If that
  // needs to be done, the caller must do it.  It is currently only
  // called from Node::reactivate().

  for (auto& strat1: strategies)
    strat1 *= strat;
}


/************************************************************
 *                                                          *
 * operator *= Strategies                                   *
 *                                                          *
 ************************************************************/

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


void Slist::multiplyOneByOne(const Slist& slist2)
{
  strategies.front() *= slist2.front();
}


/************************************************************
 *                                                          *
 * Splits                                                   *
 *                                                          *
 ************************************************************/

void Slist::pushDistribution(const StratData& stratData)
{
  // TODO Can this go in StratData?
  auto siter = strategies.begin();
  for (auto& sd: stratData.data)
  {
    siter->push_back(* sd.iter);
    siter++;
  }
}


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
      own.pushDistribution(stratData);

      if (stratData.advance() == STRATSTATUS_END)
        break;
      else
        continue;
    }

    if (riter->dist() < stratData.dist())
    {
      // Distribution that is only in strat2.
      riter++;
      continue;
    }

    bool endFlag = false;
    while (stratData.dist() < riter->dist())
    {
      // A unique distribution.
      own.pushDistribution(stratData);

      if (stratData.advance() == STRATSTATUS_END)
      {
        endFlag = true;
        break;
      }
    }

    if (endFlag)
      break;

    if (riter->dist() == stratData.dist())
    {
      // A shared distribution.
      shared.pushDistribution(stratData);

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
      * citer = parentRange.constantResult();
      stratData.eraseDominatedDist();
      eraseFlag = true;
      citer++;

      if (debugFlag)
      {
        cout << "Erased constant for parent range:\n";
        cout << parentRange.strHeader(true);
        cout << parentRange.str(true);
      }
    }
    else if (parentRange < * stratData.riter)
    {
      stratData.eraseDominatedDist();
      eraseFlag = true;

      if (debugFlag)
      {
        cout << "Erased dominated range for parent range:\n";
        cout << parentRange.strHeader(true);
        cout << parentRange.str(true);
      }
    }
  }

  // Shrink to the size used.
  constants.eraseRest(citer);

  return eraseFlag;
}


/************************************************************
 *                                                          *
 * Winners methods                                          *
 *                                                          *
 ************************************************************/

const Result Slist::resultLowest() const
{
  Result resultLowest;

  for (const auto& strat: strategies)
    resultLowest *= strat.resultLowest();

  return resultLowest;
}


/************************************************************
 *                                                          *
 * Utilities                                                *
 *                                                          *
 ************************************************************/


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

  const unsigned incr = (rankFlag ? 12 : 4);

  ss << setw(4) << left << "Dist" << right;

  for (unsigned i = 0; i < strategies.size(); i++)
    ss << setw(static_cast<int>(incr)) << i;
  ss << "\n";

  ss << string(4 + incr * strategies.size(), '-') << "\n";

  return ss.str();
}


string Slist::strWeights(const bool rankFlag) const
{
  stringstream ss;

  const unsigned incr = (rankFlag ? 12 : 4);
  ss << string(4 + incr * strategies.size(), '-') << "\n";
  ss << setw(4) << "Wgt";
  for (const auto& res: strategies)
    ss << setw(static_cast<int>(incr)) << res.weight();
  return ss.str() + "\n";
}


string Slist::strWinners() const
{
  stringstream ss;
  ss << setw(4) << "Win";

  Result resLowest;
  for (const auto& strat: strategies)
  {
    const Result res = strat.resultLowest();
    ss << setw(12) << res.strWinners();
    resLowest *= res;
  }
  ss << "\n";

  if (strategies.size() > 1)
    ss <<
      setw(4) << "Prod" <<
      setw(12) << resLowest.strWinners() << "\n\n";

  return ss.str();
}


string Slist::str(
  const string& title,
  const bool rankFlag) const
{
  if (strategies.size() == 0)
    return "";

  stringstream ss;
  ss << Slist::strHeader(title, rankFlag);

// TODO Can we do this with StratData?

  // Make a list of iterators -- one per Strategy.
  list<list<Result>::const_iterator> iters, itersEnd;
  for (auto& res: strategies)
  {
    iters.push_back(res.begin());
    itersEnd.push_back(res.end());
  }

  // Use the iterator for the first Strategy to get the distributions.
  while (iters.front() != itersEnd.front())
  {
    ss << setw(4) << left << +iters.front()->dist() << right;
    for (auto& iter: iters)
    {
      ss << iter->strEntry(rankFlag);

      // This looks funny, but it's the content of iters that is modified.
      iter++;
    }
    ss << "\n";
  }

  ss << Slist::strWeights(rankFlag);

  if (rankFlag)
    ss << Slist::strWinners();

  return ss.str();
}


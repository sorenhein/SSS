#include <iostream>
#include <iomanip>
#include <sstream>

#include "Strategies.h"
#include "StratData.h"

#include "../plays/Play.h"

#include "../Survivor.h"

// TMP
#include "../stats/Timer.h"
extern vector<Timer> timersStrat;


Strategies::Strategies()
{
  Strategies::reset();
}


Strategies::~Strategies()
{
}


void Strategies::reset()
{
  strategies.clear();
  ranges.clear();
}


Strategy& Strategies::front()
{
  return strategies.front();
}


void Strategies::setTrivial(
  const Result& trivial,
  const unsigned char len)
{
  // Repeat the trivial result len times.

  Strategies::reset();
  strategies.emplace_back(Strategy());
  Strategy& strat = strategies.back();
  strat.logTrivial(trivial, len);
}


bool Strategies::sameOrdered(const Strategies& strats2)
{
  // This assumes the same ordering.

  unsigned i = 0; // Could be used for debugging output, but isn't now.
  auto iter2 = strats2.strategies.begin();

  for (auto iter1 = strategies.begin(); iter1 != strategies.end(); 
      iter1++, iter2++, i++)
  {
    if (!(* iter1 == * iter2))
      return false;
  }

  return true;
}


bool Strategies::sameUnordered(const Strategies& strats2)
{
  // This is dreadfully slow.  We could make use of the 
  // weight-ordered nature, and perhaps also the used-up matches,
  // if this ever became a performance issue.  But I think it's
  // mainly used for debugging.

  for (auto iter1 = strategies.begin(); iter1 != strategies.end(); iter1++)
  {
    bool sameFlag = false;
    for (auto iter2 = strats2.strategies.begin(); 
        iter2 != strats2.strategies.end(); iter2++)
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


void Strategies::restudy()
{
  // Very fast.
  if (strategies.empty())
    return;

  for (auto& strategy: strategies)
    strategy.study();
}


bool Strategies::operator == (const Strategies& strats2)
{
  if (Strategies::size() != strats2.size())
    return false;
  else if (Strategies::sameOrdered(strats2))
    return true;
  else
    return Strategies::sameUnordered(strats2);
}


void Strategies::operator += (const Strategy& strat)
{
  // TODO When does this happen?
  if (strat.size() == 0)
    return;

  // The strategies list is in descending order of weights.
  // The new Strategy might dominate everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  
  if (strategies.empty())
  {
    strategies.push_back(strat);
    return;
  }

timersStrat[0].start();
  auto iter = strategies.begin();
  while (iter != strategies.end() && iter->weight() >= strat.weight())
  {

    if (* iter >= strat)
    {
      // The new strat is dominated.
timersStrat[0].stop();
      return;
    }
    else
      iter++;
  }

  // The new vector must be inserted.  This consumes about a third
  // of the time of the overall method.
  iter = next(strategies.insert(iter, strat));

  // The new vector may dominate lighter vectors. This only consumes
  // 5-10% of the overall time.
  while (iter != strategies.end())
  {

    if (strat >= * iter)
      iter = strategies.erase(iter);
    else
      iter++;
  }

timersStrat[0].stop();
}


void Strategies::markChanges(
  const Strategies& strats2,
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

  for (auto& strat: strats2.strategies)
  {
    auto iter = strategies.begin();
    unsigned stratNo = 0;
    bool doneFlag = false;
    while (iter != strategies.end() && iter->weight() >= strat.weight())
    {
      if (ownDeletions[stratNo])
      {
        iter++;
        stratNo++;
        continue;
      }

      if (iter->greaterEqualByProfile(strat))
      {
        doneFlag = true;
        break;
      }
      else
      {
        iter++;
        stratNo++;
      }
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
// const bool b1 = strat.greaterEqualByProfile(* iter);
      if (strat >= * iter)
      // if (strat.greaterEqualByProfile(* iter))
      {
      // assert(b1);
        if (ownDeletions[stratNo] == 0)
        {
          deletions.push_back(iter);
          ownDeletions[stratNo] = 1;
        }
      }
      // else
      // {
       // assert(!b1);
      // }
      iter++;
      stratNo++;
    }
  }
}


void Strategies::plusOneByOne(const Strategies& strats2)
{
  // Simple version when both Strategies are known to have size 1,
  // as happens very frequently.

  auto& str1 = strategies.front();
  auto& str2 = strats2.strategies.front();

  if (str1 >= str2)
    return;
  else if (str2 >= str1)
    * this = strats2;
  else
  {
    if (str1.weight() >= str2.weight())
      strategies.push_back(str2);
    else
      strategies.push_front(str2);
  }
}


void Strategies::operator += (Strategies& strats2)
{
  if (strategies.empty())
  {
    * this = strats2;
    return;
  }

  const unsigned sno1 = strategies.size();
  const unsigned sno2 = strats2.size();

  if (sno1 == 1 && sno2 == 1)
  {
    // Simplified case.
    Strategies::plusOneByOne(strats2);
  }
  else if (sno1 >= 20 && sno2 >= 20)
  {
    // Complex case.
    // Rare, but very slow per invocation when it happens.
    // Consumes perhaps 75% of the method time.

    list<Addition> additions;
    list<list<Strategy>::const_iterator> deletions;

timersStrat[20].start();
    /* */
    Strategies::makeRanges();
    strats2.makeRanges();

    // We only need the minima here, but we use the existing method
    // for simplicity.
    Strategies::propagateRanges(strats2);

    for (auto& strat: strategies)
      strat.scrutinize(ranges);
    for (auto& strat: strats2.strategies)
      strat.scrutinize(ranges);
      /* */

    Strategies::markChanges(strats2, additions, deletions);
timersStrat[20].stop();

    for (auto& addition: additions)
      strategies.insert(addition.iter, *(addition.ptr));

    for (auto& deletion: deletions)
      strategies.erase(deletion);
  }
  else
  {
    // General case.
    // Frequent and fast, perhaps 25% of the method time.
    for (auto& strat2: strats2.strategies)
      * this += strat2;
  }
}


void Strategies::operator *= (const Strategy& strat)
{
  if (strategies.empty())
  {
timersStrat[7].start();
    * this += strat;
timersStrat[7].stop();
  }
  else
  {
    // TODO Really?  No re-sorting and consolidating?
timersStrat[8].start();
    for (auto& strat1: strategies)
      strat1 *= strat;
timersStrat[8].stop();
  }
}


void Strategies::multiplyAdd(
  const Strategy& strat1,
  const Strategy& strat2)
{
  // This costs about two thirds of the overall method time.
  auto& product = strategies.back();
  product.multiply(strat1, strat2);
  auto piter = prev(strategies.end());

  if (strategies.size() == 1)
  {
    // Keep the product and make a new scratch-pad element.
    strategies.emplace_back(Strategy());
    return;
  }

  // The strategies list is in descending order of weights.
  // The new Strategy might dominate everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  // This checking costs about one third of the overall method time.
  
  auto iter = strategies.begin();
  while (iter != piter && iter->weight() >= piter->weight())
  {

    if (* iter >= * piter)
    {
      // The new strat is dominated.
      return;
    }
    else
      iter++;
  }

  // Already in the right place at the end?
  if (iter == piter)
  {
    strategies.emplace_back(Strategy());
    return;
  }

  // The new vector must be inserted, i.e. spliced in.
  // This is super-fast.
  strategies.splice(iter, strategies, piter);

  // The new vector may dominate lighter vectors.  This is also
  // quite efficient and doesn't happen so often.
  while (iter != strategies.end())
  {
    if (* piter >= * iter)
      iter = strategies.erase(iter);
    else
      iter++;
  }

  // Make a new scratch-pad element.
  strategies.emplace_back(Strategy());
}


void Strategies::multiplyAddNew(
  const Strategy& strat1,
  const Strategy& strat2,
  const Ranges& minima)
{
  // This costs about two thirds of the overall method time.
  auto& product = strategies.back();
  product.multiply(strat1, strat2);
  product.scrutinize(minima);
  auto piter = prev(strategies.end());

  if (strategies.size() == 1)
  {
    // Keep the product and make a new scratch-pad element.
    strategies.emplace_back(Strategy());
    return;
  }

  // The strategies list is in descending order of weights.
  // The new Strategy might dominate everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  // This checking costs about one third of the overall method time.
  
  auto iter = strategies.begin();
  while (iter != piter && iter->weight() >= piter->weight())
  {
    if (iter->greaterEqualByProfile(* piter))
    {
      // The new strat is dominated.
      return;
    }
    else
      iter++;
  }

  // Already in the right place at the end?
  if (iter == piter)
  {
    strategies.emplace_back(Strategy());
    return;
  }

  // The new vector must be inserted, i.e. spliced in.
  // This is super-fast.
  strategies.splice(iter, strategies, piter);
  piter = prev(iter);

  // The new vector may dominate lighter vectors.  This is also
  // quite efficient and doesn't happen so often.
  while (iter != strategies.end())
  {
    if (piter->greaterEqualByProfile(* iter))
      iter = strategies.erase(iter);
    else
      iter++;
  }

  // Make a new scratch-pad element.
  strategies.emplace_back(Strategy());
}


bool Strategies::greaterEqual(
  const ExtendedStrategy& es1,
  const ExtendedStrategy& es2,
  const SplitStrategies& split1,
  const SplitStrategies& split2) const
{
  // cout << "es1 " << es1.indexOwn << ", " << es1.indexOther << endl;
  // cout << "es2 " << es2.indexOwn << ", " << es2.indexOther << endl;
  // cout << "split1 " << split1.matrix.size() << " x " <<
    // split1.matrix[0].size() << endl;
  // cout << "split2 " << split2.matrix.size() << " x " <<
    // split2.matrix[0].size() << endl;
// assert(es1.indexOwn < split1.matrix.size());
// assert(es2.indexOwn < split1.matrix.size());

  const Compare c1 = split1.matrix[es1.indexOwn][es2.indexOwn];
  if (c1 == COMPARE_LESS_THAN || c1 == COMPARE_INCOMMENSURATE)
    return false;

// assert(es1.indexOther < split2.matrix.size());
// assert(es2.indexOther < split2.matrix.size());

  const Compare c2 = split2.matrix[es1.indexOther][es2.indexOther];
  if (c2 == COMPARE_LESS_THAN || c2 == COMPARE_INCOMMENSURATE)
    return false;

  return es1.overlap.greaterEqualByProfile(es2.overlap);
}


void Strategies::multiplyAddNewer(
  const Strategy& strat1,
  const Strategy& strat2,
  const Ranges& minima,
  const SplitStrategies& splitOwn,
  const SplitStrategies& splitOther,
  const unsigned indexOwn,
  const unsigned indexOther,
  list<ExtendedStrategy>& extendedStrategies)
{
  auto& lastEntry = extendedStrategies.back();
  lastEntry.overlap.multiply(strat1, strat2);
  lastEntry.overlap.scrutinize(minima);
  lastEntry.indexOwn = indexOwn;
  lastEntry.indexOther = indexOther;

  auto piter = prev(extendedStrategies.end());

  if (extendedStrategies.size() == 1)
  {
    // Keep the product and make a new scratch-pad element.
    extendedStrategies.emplace_back(ExtendedStrategy());
    return;
  }

  // The strategies list is in descending order of weights.
  // The new Strategy might dominate everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  // This checking costs about one third of the overall method time.
  
  auto iter = extendedStrategies.begin();
  while (iter != piter && iter->overlap.weight() >= piter->overlap.weight())
  {
    // TMP if (somehow iter->overlap is >= piter->overlap)
    if (Strategies::greaterEqual(* iter, * piter, splitOwn, splitOther))
    {
      // The new strat is dominated.
      return;
    }
    else
      iter++;
  }

  // Already in the right place at the end?
  if (iter == piter)
  {
    extendedStrategies.emplace_back(ExtendedStrategy());
    return;
  }

  // The new vector must be inserted, i.e. spliced in.
  // This is super-fast.
  extendedStrategies.splice(iter, extendedStrategies, piter);
  piter = prev(iter);

  // The new vector may dominate lighter vectors.  This is also
  // quite efficient and doesn't happen so often.
  while (iter != extendedStrategies.end())
  {
    if (Strategies::greaterEqual(* piter, * iter, splitOwn, splitOther))
      iter = extendedStrategies.erase(iter);
    else
      iter++;
  }

  // Make a new scratch-pad element.
  extendedStrategies.emplace_back(ExtendedStrategy());
}


void Strategies::combinedLower(
  const Ranges& ranges1, 
  const Ranges& ranges2,
  const bool keepConstantsFlag,
  Ranges& minima) const
{
  // Finds the overall lower envelope of two ranges, which is useful
  // before multiplying together two Strategies.

  auto iter1 = ranges1.begin();
  auto iter2 = ranges2.begin();

  while (true)
  {
    if (iter1 == ranges1.end())
    {
      if (iter2 != ranges2.end())
        for (auto it = iter2; it != ranges2.end(); it++)
        {
          if (keepConstantsFlag || ! it->constant())
            minima.push_back(* it);
        }
      break;
    }
    else if (iter2 == ranges2.end())
    {
      if (iter1 != ranges1.end())
        for (auto it = iter1; it != ranges1.end(); it++)
        {
          if (keepConstantsFlag || ! it->constant())
            minima.push_back(* it);
        }
      break;
    }

    if (iter1->dist < iter2->dist)
    {
      if (keepConstantsFlag || ! iter1->constant())
        minima.push_back(* iter1);
      iter1++;
    }
    else if (iter1->dist > iter2->dist)
    {
      if (keepConstantsFlag || ! iter2->constant())
        minima.push_back(* iter2);
      iter2++;
    }
    else
    {
      if (iter1->minimum <= iter2->minimum)
      {
        if (keepConstantsFlag || ! iter1->constant())
          minima.push_back(* iter1);
      }
      else
      {
        if (keepConstantsFlag || ! iter2->constant())
          minima.push_back(* iter2);
      }
      iter1++;
      iter2++;
    }
  }
}


void Strategies::setSplit(
  Strategies& stratsToSplit,
  const Strategy& strat2,
  SplitStrategies& split) const
{
  // Split our strategies by distribution into one group (own) with
  // those distributions that are unique to us, and another (shared)
  // with distributions that overlap.  This is relative to strat2.

  // All our Strategy's have the same distributions, so we pick one.
  const unsigned ssize = stratsToSplit.size();

  // List of iterators to a Result of each Strategy in stratsToSplit.
  // All are in sync to point to a given distribution.
  // They move in sync down across the Strategy's.
  StratData stratData;
  stratData.data.resize(ssize);
  stratsToSplit.getLoopData(stratData);

  split.own.strategies.resize(ssize);
  split.shared.strategies.resize(ssize);

  // Pointers to each of the own, unique partial Strategy's,
  // for later use.
  split.ownPtrs.resize(ssize);
  auto ownIter = split.own.strategies.begin();
  for (unsigned i = 0; i < ssize; i++, ownIter++)
    split.ownPtrs[i] = &* ownIter;

  // strat2 is an example of a Strategy that we use to split 
  // stratsToSplit according to the distributions.

  auto riter = strat2.begin();
  while (true)
  {
    if (riter == strat2.end())
    {
      // A unique distribution.
// cout << "unique dist finished " << +stratData.dist() << endl;
      ownIter = split.own.strategies.begin();
      for (auto& sd: stratData.data)
      {
        ownIter->push_back(* sd.iter);
        ownIter++;
      }
// cout << split.own.str("split.own now");

      if (stratData.advance() == STRATSTATUS_END)
        break;
      else
        continue;
    }

    if (riter->dist < stratData.dist())
    {
// cout << "dist " << +riter->dist << " only in other" << endl;
      // Distribution that is only in strat2.
      riter++;
      continue;
    }

    bool endFlag = false;
    while (stratData.dist() < riter->dist)
    {
      // A unique distribution.
// cout << "unique dist non-finished " << +stratData.dist() << endl;
      ownIter = split.own.strategies.begin();
      for (auto& sd: stratData.data)
      {
        ownIter->push_back(* sd.iter);
        ownIter++;
      }
// cout << split.own.str("split.own now");

      if (stratData.advance() == STRATSTATUS_END)
      {
        endFlag = true;
        break;
      }
    }

    if (endFlag)
      break;

    if (riter->dist == stratData.dist())
    {
      // A shared distribution.
// cout << "shared dist " << +riter->dist << endl;
      auto sharedIter = split.shared.strategies.begin();
      for (auto& sd: stratData.data)
      {
        sharedIter->push_back(* sd.iter);
        sharedIter++;
      }
// cout << split.shared.str("split.shared now");

      if (stratData.advance() == STRATSTATUS_END)
        break;
    }

    riter++;
  }


  split.own.makeRanges();
  for (auto& own: split.own.strategies)
    own.scrutinize(split.own.ranges);

  split.matrix.resize(ssize);
  for (unsigned i = 0; i < ssize; i++)
    split.matrix[i].resize(ssize);

  if (split.own.numDists() == 0)
  {
    for (unsigned i = 0; i < ssize; i++)
      for (unsigned j = 0; j < ssize; j++)
        split.matrix[i][j] = COMPARE_EQUAL;
  }
  else
  {
    for (unsigned i = 0; i < ssize; i++)
    {
      split.matrix[i][i] = COMPARE_EQUAL;
      for (unsigned j = 0; j < i; j++)
      {
        Compare c = split.ownPtrs[i]->compareByProfile(* split.ownPtrs[j]);
        split.matrix[i][j] = c;

        // Flip for the anti-symmetric position.
        if (c == COMPARE_LESS_THAN)
          split.matrix[j][i] = COMPARE_GREATER_THAN;
        else if (c == COMPARE_GREATER_THAN)
          split.matrix[j][i] = COMPARE_LESS_THAN;
        else
          split.matrix[j][i] = c;
      }
    }
  }
}


void Strategies::operator *= (Strategies& strats2)
{
  const unsigned len2 = strats2.strategies.size();
  if (len2 == 0)
  {
    // Keep the current results.
    return;
  }

  const unsigned len1 = strategies.size();
  if (len1 == 0)
  {
    // Keep the new results.  Very fast.
    strategies = strats2.strategies;
    return;
  }

  if (len1 == 1 && len2 == 1)
  {
timersStrat[12].start();
    strategies.front() *= strats2.strategies.front();
timersStrat[12].stop();
    return;
  }

  // This implementation of the general product attempts to reduce
  // memory overhead.  The temporary product is formed in the last
  // element of Strategies as a scratch pad.  If it turns out to be
  // viable, it is already in Strategies and subject to move semantics.

// cout << Strategies::str("strategies");
// cout << strats2.str("strats2");

Strategies strCopy = * this;

  auto strategiesOwn = move(strategies);
  strategies.clear();

  strategies.emplace_back(Strategy());

  if (! ranges.empty() && ! strats2.ranges.empty())
  {
/*
timersStrat[21].start();

// cout << strCopy.str("strategiesOwn");
// cout << strats2.str("strats2.strategies");

    // Split out unique and overlapping distributions.
    SplitStrategies splitOwn, splitOther;
    Strategies::setSplit(strCopy, strats2.strategies.front(), splitOwn);

// cout << splitOwn.own.str("own1");
// cout << splitOwn.shared.str("shared1");

    Strategies::setSplit(strats2, strategiesOwn.front(), splitOther);

// cout << splitOwn.own.str("own2");
// cout << splitOwn.shared.str("shared2");

timersStrat[21].stop();

timersStrat[22].start();
    // Same as below, I guess.
    Ranges minima2;
    Strategies::combinedLower(ranges, strats2.ranges, false, minima2);

// cout << "scrutinizing" << endl;
    for (auto& strat: strategiesOwn)
      strat.scrutinize(ranges);
    for (auto& strat: strats2.strategies)
      strat.scrutinize(ranges);
// cout << "scrutinized" << endl;
timersStrat[22].stop();

timersStrat[23].start();
    // Multiply out the matrices.
    Strategies stmp;
    list<ExtendedStrategy> extendedStrats;
    extendedStrats.emplace_back(ExtendedStrategy());

    unsigned i = 0;
    for (auto& strat1: strategiesOwn)
    {
      unsigned j = 0;
      for (auto& strat2: strats2.strategies)
      {
        stmp.multiplyAddNewer(strat1, strat2, minima2,
          splitOwn, splitOther, i, j, extendedStrats);
        j++;
      }
      i++;
    }
timersStrat[23].stop();

    // TODO
    // Add back the non-overlapping results.
*/

timersStrat[14].start();

    // We only use the minima here.
    Ranges minima;
    Strategies::combinedLower(ranges, strats2.ranges, false, minima);

/*
cout << "ranges\n";
cout << ranges.front().strHeader();
for (auto& range: ranges)
  cout << range.str();
cout << "strats2.ranges\n";
cout << strats2.ranges.front().strHeader();
for (auto& range: strats2.ranges)
  cout << range.str();
cout << "minima\n";
cout << minima.front().strHeader();
for (auto& range: minima)
  cout << range.str();
  */

    for (auto& strat1: strategiesOwn)
      for (auto& strat2: strats2.strategies)
        Strategies::multiplyAddNew(strat1, strat2, minima);

timersStrat[14].stop();



  }
  else
  {
    // Probably comes from reactivate().
timersStrat[2].start();

    for (auto& strat1: strategiesOwn)
      for (auto& strat2: strats2.strategies)
        Strategies::multiplyAdd(strat1, strat2);

timersStrat[2].stop();
  }

  strategies.pop_back();
}


unsigned Strategies::size() const
{
  return strategies.size();
}


unsigned Strategies::numDists() const
{
  if (strategies.empty())
    return 0;
  else
    return strategies.front().size();
}


void Strategies::collapseOnVoid()
{
  // Very fast.
  assert(strategies.size() > 0);
  if (strategies.size() == 1)
  {
    assert(strategies.front().size() == 1);
    return;
  }

  auto iterBest = strategies.begin();
  assert(iterBest->size() == 1);

  // Find the best one for declarer.
  for (auto iter = next(strategies.begin()); 
      iter != strategies.end(); iter++)
  {
    assert(iter->size() == 1);
    if (* iter >= * iterBest)
      iterBest = iter;
  }

  // Copy it to the front and remove the others.
  strategies.front() = * iterBest;
  strategies.erase(next(strategies.begin()), strategies.end());
}


void Strategies::getLoopData(StratData& stratData)
{
  // This is used to loop over all strategies in synchrony, one
  // distribution at a time.
  auto siter = stratData.data.begin();
  for (auto& strat: strategies)
  {
    siter->ptr = &strat;
    siter->iter = strat.begin();
    siter->end = strat.end();
    siter++;
  }
}


void Strategies::makeRanges()
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


void Strategies::propagateRanges(const Strategies& child)
{
  // This propagates the child's ranges to the current parent ranges.
  // The distribution number has to match.
  if (ranges.empty())
  {
    ranges = child.ranges;
    return;
  }

  auto iter1 = ranges.begin();
  auto iter2 = child.ranges.begin();

  while (iter2 != child.ranges.end())
  {
    if (iter1 == ranges.end() || iter1->dist > iter2->dist)
    {
      ranges.insert(iter1, * iter2);
      iter2++;
    }
    else if (iter1->dist < iter2->dist)
      iter1++;
    else
    {
      * iter1 *= * iter2;
      iter1++;
      iter2++;
    }
  }
}


const Ranges& Strategies::getRanges() const
{
  return ranges;
}


void Strategies::consolidate()
{
  // Acceptably fast.

  Strategies::restudy();

  auto oldStrats = move(strategies);
  strategies.clear();  // But leave ranges intact

  for (auto& strat: oldStrats)
    * this += strat;
}


void Strategies::adapt(
  const Play& play,
  const Survivors& survivors)
{
timersStrat[6].start();
  for (auto& strat: strategies)
    strat.adapt(play, survivors);

  if (play.lhoPtr->isVoid() || play.rhoPtr->isVoid())
    Strategies::collapseOnVoid();
timersStrat[6].stop();
}


string Strategies::strRanges(const string& title) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";

  if (! ranges.empty())
    ss << ranges.front().strHeader();
  for (auto& range: ranges)
    ss << range.str();

  return ss.str();
}


string Strategies::strHeader(
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


string Strategies::strWeights(const bool rankFlag) const
{
  stringstream ss;

  const unsigned incr = (rankFlag ? 12 : 4);
  ss << string(4 + incr * strategies.size(), '-') << "\n";
  ss << setw(4) << "Wgt";
  for (const auto& res: strategies)
    ss << setw(static_cast<int>(incr)) << res.weight();
  return ss.str() + "\n";
}


string Strategies::str(
  const string& title,
  const bool rankFlag) const
{
  if (strategies.size() == 0)
    return "";

  stringstream ss;
  ss << Strategies::strHeader(title, rankFlag);

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
    ss << setw(4) << left << +iters.front()->dist << right;
    for (auto& iter: iters)
    {
      ss << iter->strEntry(rankFlag);

      // This looks funny, but it's the content of iters that is modified.
      iter++;
    }
    ss << "\n";
  }

  ss << Strategies::strWeights(rankFlag);

  return ss.str();
}


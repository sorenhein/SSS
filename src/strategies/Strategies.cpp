#include <iostream>
#include <iomanip>
#include <sstream>

#include "Strategies.h"
#include "StratData.h"

#include "../plays/Play.h"

#include "../Survivor.h"


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


void Strategies::setTrivial(
  const Result& trivial,
  const unsigned len)
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
  // The strategies list is in descending order of weights.
  // The new Strategy might dominate everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  
  auto iter = strategies.begin();

  // strat cannot beat anything with a higher weight.
  while (iter != strategies.end() && iter->weight() > strat.weight())
  {
    if (iter->compare(strat) == COMPARE_GREATER_THAN)
      // The new strat is dominated.
      return;
    else
      iter++;
  }

  while (iter != strategies.end() && iter->weight() == strat.weight())
  {
    // They might be the same if they have the same weight.
    if (iter->compare(strat) == COMPARE_EQUAL)
      return;
    else
      iter++;
  }

  // The new vector must be inserted.
  iter = next(strategies.insert(iter, strat));

  // The new vector may dominate lighter vectors.
  while (iter != strategies.end())
  {
    if (strat > * iter)
      iter = strategies.erase(iter);
    else
      iter++;
  }
}


void Strategies::operator += (const Strategies& strats2)
{
  for (auto& strat2: strats2.strategies)
    * this += strat2;
}


void Strategies::operator *= (const Strategy& strat)
{
  if (strategies.empty())
    * this += strat;
  else
  {
    for (auto& strat1: strategies)
      strat1 *= strat;
  }
}


void Strategies::operator *= (const Strategies& strats2)
{
  const unsigned len2 = strats2.strategies.size();
  if (len2 == 0)
    // Keep the current results.
    return;

  const unsigned len1 = strategies.size();
  if (len1 == 0)
  {
    // Keep the new results.
    strategies = strats2.strategies;
    return;
  }

  if (len1 == 1 && len2 == 1)
  {
    strategies.front() *= strats2.strategies.front();
    return;
  }

  // General case.  The implementation is straightforward but probably
  // inefficient.  Maybe there's a faster way to do it in place.
  auto strategiesOwn = strategies;
  strategies.clear();

  Strategy stmp;
  for (auto& strat1: strategiesOwn)
  {
    for (auto& strat2: strats2.strategies)
    {
      stmp = strat1;
      stmp *= strat2;
      *this += stmp;
    }
  }
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
    if (* iter > * iterBest)
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


void Strategies::purgeRanges(const Strategies& parent)
{
  for (auto& strat: strategies)
    strat.purgeRanges(ranges, parent.ranges);
}


void Strategies::getConstants(Strategy& constants) const
{
  // This is called for the parent and does not set the winners.
  // TODO It is not very efficient.
  vector<unsigned> distributions(ranges.size());
  vector<unsigned> tricks(ranges.size());

  unsigned i = 0;
  for (auto& range: ranges)
  {
    if (range.constant())
    {
      distributions[i] = range.dist;
      tricks[i] = range.lower;
      i++;
    }
  }

  distributions.resize(i);
  tricks.resize(i);

  constants.log(distributions, tricks);
}


const Ranges& Strategies::getRanges() const
{
  return ranges;
}


void Strategies::consolidate()
{
  // TODO Can perhaps be done inline.
  // Would have to sort first (or last).
  auto oldStrats = strategies;
  Strategies::reset();

  for (auto& strat: oldStrats)
    * this += strat;
}


void Strategies::adapt(
  const Play& play,
  const Survivors& survivors)
{
  for (auto& strat: strategies)
    strat.adapt(play, survivors);

  if (play.lhoPtr->isVoid() || play.rhoPtr->isVoid())
    Strategies::collapseOnVoid();
}


void Strategies::checkWeights() const
{
  for (auto& strat: strategies)
    strat.checkWeights();    
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
    ss << setw(incr) << i;
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
    ss << setw(incr) << res.weight();
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
    ss << setw(4) << left << iters.front()->dist << right;
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


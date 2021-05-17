#include <iostream>
#include <iomanip>
#include <sstream>

#include "Strategies.h"

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
  results.clear();
}


void Strategies::setTrivial(
  const TrickEntry& trivialEntry,
  const unsigned len)
{
  Strategy tv;
  tv.logTrivial(trivialEntry, len);

  Strategies::reset();
  results.push_back(tv);
}


bool Strategies::operator ==(const Strategies& tvs)
{
  // TODO This assumes the same ordering, but it's a start.
  if (Strategies::size() != tvs.size())
  {
cout << "Size " <<Strategies::size() << " vs " << tvs.size() << endl;
    return false;
  }

unsigned i = 0;
  bool sameFlag = true;
  auto iter2 = tvs.results.begin();
  for (auto iter1 = results.begin(); iter1 != results.end(); 
      iter1++, iter2++)
  {
    if (!(* iter1 == * iter2))
    {
cout << "ABOUT TO DIFFER " << i << "\n";
      sameFlag = false;
      break;
    }
    i++;
  }

  if (sameFlag)
    return true;

  // Not very clever, detailed comparison.
  for (auto iter1 = results.begin(); iter1 != results.end(); iter1++)
  {
    sameFlag = false;
    for (iter2 = tvs.results.begin(); iter2 != tvs.results.end(); iter2++)
    {
      if (* iter1 == * iter2)
      {
        sameFlag = true;
        break;
      }
    }

    if (! sameFlag)
    {
cout << "ABOUT TO REALLY DIFFER\n";
      return false;
    }
  }

  return true;
}


void Strategies::operator +=(const Strategy& tv)
{
  // The results list is in descending order of weights.
  // The new Strategy dominates everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  
  auto riter = results.begin();

  // tv cannot beat anything with a higher weight.
  while (riter != results.end() && riter->weight() > tv.weight())
  {
    if (riter->compare(tv) == COMPARE_GREATER_THAN)
      return;
    else
      riter++;
  }

  while (riter != results.end() && riter->weight() == tv.weight())
  {
    // They might be the same if they have the same weight.
    if (riter->compare(tv) == COMPARE_EQUAL)
      return;
    else
      riter++;
  }

  // The new vector must be inserted.
  riter = next(results.insert(riter, tv));

  // The new vector may dominate lighter vectors.
  while (riter != results.end())
  {
    if (tv > * riter)
      riter = results.erase(riter);
    else
      riter++;
  }
}


void Strategies::operator +=(const Strategies& tvs)
{
  for (auto& tv: tvs.results)
    * this += tv;
}


void Strategies::operator *=(const Strategies& tvs2)
{
  const unsigned len2 = tvs2.results.size();
  if (len2 == 0)
    // Keep the current results.
    return;

  const unsigned len1 = results.size();
  if (len1 == 0)
  {
    // Keep the new results.
    results = tvs2.results;
    return;
  }

  if (len1 == 1 && len2 == 1)
  {
    results.front() *= tvs2.results.front();
    return;
  }

  // General case.  The implementation is straightforward but probably
  // inefficient.  Maybe there's a faster way to do it in place.
  list<Strategy> resultsOwn = results;
  results.clear();

  Strategy tmp;
  for (auto& tv1: resultsOwn)
  {
    for (auto& tv2: tvs2.results)
    {
      tmp = tv1;
      tmp *= tv2;
      *this += tmp;
    }
  }
}


void Strategies::operator *=(const Strategy& tv2)
{
  if (results.size() == 0)
    * this += tv2;
  else
  {
    for (auto& tv1: results)
      tv1 *= tv2;
  }
}


void Strategies::operator |=(const Strategies& tvs2)
{
  // Vector-wise combination.
  assert(results.size() == tvs2.results.size());

  auto riter = results.begin();
  auto riter2 = tvs2.results.begin();

  while (riter != results.end())
  {
    * riter *= * riter2;
    riter++;
    riter2++;
  }
}


unsigned Strategies::size() const
{
  return results.size();
}


unsigned Strategies::numDists() const
{
  if (results.empty())
    return 0;
  else
    return results.front().size();
}


void Strategies::collapseOnVoid()
{
  assert(results.size() > 0);
  if (results.size() == 1)
  {
    assert(results.front().size() == 1);
    return;
  }

  // Find the best one for declarer.
  auto iterTV = results.begin();
  assert(iterTV->size() == 1);

  // Find the best one for declarer.
  for (auto iter = next(results.begin()); iter != results.end(); iter++)
  {
    assert(iter->size() == 1);
    if (* iter > * iterTV)
      iterTV = iter;
  }

  // Copy it to the front and remove the others.
  results.front() = * iterTV;
  results.erase(next(results.begin()), results.end());
}


void Strategies::bound(
  Strategy& constants,
  Strategy& lower) const
  // Strategy& upper) const
{
  // Calculate Strategy values to summarize this Strategies.
  // constants is the set of constant strategies.  Other Strategies
  // may have other constant values, or non-constant ones, that
  // may be lower or higher than this results.  There are only
  // entries for those distributions that have constant tricks.
  // lower and upper and the bounds.  They exist for each
  // distribution.
  
  assert(results.size() > 0);

  constants = results.front();
  lower = results.front();
  // upper = results.front();

  if (results.size() == 1)
    return;

  for (auto iter = next(results.begin()); iter != results.end(); iter++)
    iter->bound(constants, lower);
    // iter->bound(constants, lower, upper);
}


void Strategies::bound(Bounds& bounds) const
{
  // Calculate Strategy values to summarize this Strategies.
  // constants is the set of constant strategies.  Other Strategies
  // may have other constant values, or non-constant ones, that
  // may be lower or higher than this results.  There are only
  // entries for those distributions that have constant tricks.
  // lower and upper and the bounds.  They exist for each
  // distribution.
  
  assert(results.size() > 0);

  bounds.constants = results.front();
  bounds.minima = results.front();
  // bounds.maxima = results.front();

  if (results.size() == 1)
    return;

  // TODO Pass in a Bounds instead
  // Eliminate the bounds with 3 arguments
  for (auto iter = next(results.begin()); iter != results.end(); iter++)
    // iter->bound(bounds.constants, bounds.minima, bounds.maxima);
    iter->bound(bounds.constants, bounds.minima);
}


unsigned Strategies::purge(const Strategy& constants)
{
  // TODO Can perhaps be done inline.
  // Returns number of distributions purged.
  auto oldResults = results;
  Strategies::reset();
  unsigned num = 0;

  for (auto& result: oldResults)
  {
    num = result.purge(constants);
    * this += result;
  }
  return num;
}


void Strategies::makeRanges()
{
  if (results.empty())
    return;

  results.front().initRanges(ranges);

  if (results.size() == 1)
    return;

  for (auto iter = next(results.begin()); iter != results.end(); iter++)
    iter->extendRanges(ranges);
}


void Strategies::propagateRanges(const Strategies& child)
{
  if (ranges.empty())
  {
    ranges = child.ranges;
    return;
  }

  auto iter1 = ranges.begin();
  auto iter2 = child.ranges.begin();

  while (iter1 != ranges.end() && iter2 != child.ranges.end())
  {
    if (iter1->dist < iter2->dist)
      iter1++;
    else if (iter1->dist > iter2->dist)
    {
      ranges.insert(iter1, * iter2);
      iter2++;
    }
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
  for (auto iter = results.begin(); iter != results.end(); iter++)
    iter->purgeRanges(ranges, parent.ranges);
}


void Strategies::getConstants(Strategy& constants) const
{
  // This is called for the parent and does not set the winners.
  // TODO We could do this more efficiently if we had a resize
  // of Strategy.
  vector<unsigned> distributions(ranges.size());
  vector<unsigned> tricks(ranges.size());

  unsigned i = 0;
  auto citer = constants.begin();
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


void Strategies::addConstantWinners(Strategy& constants) const
{
  // We are now in a child node.
  // TODO Potentially make sub-classes of Node for Parent and Child.
  for (auto& tv: results)
    tv.addConstantWinners(constants);
}


void Strategies::adapt(
  const Play& play,
  const Survivors& survivors)
{
  for (auto& tv: results)
    tv.adapt(play, survivors);

  if (play.lhoPtr->isVoid() || play.rhoPtr->isVoid())
    Strategies::collapseOnVoid();
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

  for (unsigned i = 0; i < results.size(); i++)
    ss << setw(incr) << i;
  ss << "\n";

  ss << string(4 + incr * results.size(), '-') << "\n";

  return ss.str();
}


string Strategies::strWeights(const bool rankFlag) const
{
  stringstream ss;

  const unsigned incr = (rankFlag ? 12 : 4);
  ss << string(4 + incr * results.size(), '-') << "\n";
  ss << setw(4) << "Wgt";
  for (const auto& res: results)
    ss << setw(incr) << res.weight();
  return ss.str() + "\n";
}


string Strategies::str(
  const string& title,
  const bool rankFlag) const
{
  if (results.size() == 0)
    return "";

  stringstream ss;
  ss << Strategies::strHeader(title, rankFlag);

  // Make a list of iterators -- one per Strategy.
  list<list<TrickEntry>::const_iterator> iters, itersEnd;
  for (auto& res: results)
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


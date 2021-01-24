#include <iostream>
#include <iomanip>
#include <sstream>

#include "Tvectors.h"


Tvectors::Tvectors()
{
  Tvectors::reset();
}


Tvectors::~Tvectors()
{
}


void Tvectors::reset()
{
  results.clear();
}


void Tvectors::setTrivial(
  const TrickEntry& trivialEntry,
  const unsigned len)
{
  Tvector tv;
  tv.logTrivial(trivialEntry, len);

  Tvectors::reset();
  results.push_back(tv);
}


void Tvectors::operator +=(const Tvector& tv)
{
  // The results list is in descending order of weights.
  // The new Tvector dominates everything with a lower weight and
  // can only be dominated by a Tvector with at least its own weight.
  
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


void Tvectors::operator +=(const Tvectors& tvs)
{
  for (auto& tv: tvs.results)
    * this += tv;
}


void Tvectors::operator *=(const Tvectors& tvs2)
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
  list<Tvector> resultsOwn = results;
  results.clear();

  Tvector tmp;
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


void Tvectors::operator *=(const Tvector& tv2)
{
  if (results.size() == 0)
    * this += tv2;
  else
  {
    for (auto& tv1: results)
      tv1 *= tv2;
  }
}


void Tvectors::operator |=(const Tvectors& tvs2)
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


unsigned Tvectors::size() const
{
  return results.size();
}


unsigned Tvectors::numDists() const
{
  if (results.empty())
    return 0;
  else
    return results.front().size();
}


void Tvectors::collapseOnVoid()
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


void Tvectors::bound(
  Tvector& constants,
  Tvector& lower,
  Tvector& upper) const
{
  // Calculate Tvector values to summarize this Tvectors.
  // constants is the set of constant strategies.  Other Tvectors
  // may have other constant values, or non-constant ones, that
  // may be lower or higher than this results.  There are only
  // entries for those distributions that have constant tricks.
  // lower and upper and the bounds.  They exist for each
  // distribution.
  
  assert(results.size() > 0);

  constants = results.front();
  lower = results.front();
  upper = results.front();

  if (results.size() == 1)
    return;

  for (auto iter = next(results.begin()); iter != results.end(); iter++)
    iter->bound(constants, lower, upper);
}


unsigned Tvectors::purge(const Tvector& constants)
{
  // TODO Can perhaps be done inline.
  // Returns number of distributions purged.
  auto oldResults = results;
  Tvectors::reset();
  unsigned num = 0;

  for (auto& result: oldResults)
  {
    num = result.purge(constants);
    * this += result;
  }
  return num;
}


void Tvectors::adapt(
  const Survivors& survivors,
  const unsigned trickNS,
  const vector<Winner>& northOrder,
  const vector<Winner>& southOrder,
  const bool lhoVoidFlag,
  const bool rhoVoidFlag,
  const bool rotateFlag)
{
  for (auto& tv: results)
    tv.adapt(survivors, trickNS, northOrder, southOrder,
      lhoVoidFlag, rhoVoidFlag, rotateFlag);

  if (lhoVoidFlag || rhoVoidFlag)
    Tvectors::collapseOnVoid();
}


string Tvectors::strHeader(
  const string& title,
  const bool rankFlag) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";

  const unsigned incr = (rankFlag ? 8 : 4);

  ss << setw(4) << left << "Dist" << right;

  for (unsigned i = 0; i < results.size(); i++)
    ss << setw(incr) << i;
  ss << "\n";

  ss << string(4 + incr * results.size(), '-') << "\n";

  return ss.str();
}


string Tvectors::strWeights(const bool rankFlag) const
{
  stringstream ss;

  const unsigned incr = (rankFlag ? 8 : 4);
  ss << string(4 + incr * results.size(), '-') << "\n";
  ss << setw(4) << "Wgt";
  for (const auto& res: results)
    ss << setw(incr) << res.weight();
  return ss.str() + "\n";
}


string Tvectors::str(
  const string& title,
  const bool rankFlag) const
{
  if (results.size() == 0)
    return "";

  stringstream ss;
  ss << Tvectors::strHeader(title, rankFlag);

  // Make a list of iterators -- one per Tvector.
  list<list<TrickEntry>::const_iterator> iters, itersEnd;
  for (auto& res: results)
  {
    iters.push_back(res.begin());
    itersEnd.push_back(res.end());
  }

  // Use the iterator for the first Tvector to get the distributions.
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

  ss << Tvectors::strWeights(rankFlag);

  return ss.str();
}


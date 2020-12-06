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


string Tvectors::strHeader(const string& title) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";

  ss << setw(4) << left << "Dist" << right;

  for (unsigned i = 0; i < results.size(); i++)
    ss << setw(4) << i;
  ss << "\n";

  ss << string(4 + 4 * results.size(), '-') << "\n";

  return ss.str();
}


string Tvectors::strWeights() const
{
  stringstream ss;
  ss << string(4 + 4 * results.size(), '-') << "\n";
  ss << setw(4) << "Wgt";
  for (const auto& res: results)
    ss << setw(4) << res.weight();
  return ss.str() + "\n";
}


string Tvectors::str(const string& title) const
{
  if (results.size() == 0)
    return "";

  stringstream ss;
  ss << Tvectors::strHeader(title);

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
      ss << setw(4) << iter->tricks;

      // This looks funny, but it's the content of iters that is modified.
      iter++;
    }
    ss << "\n";
  }

  ss << Tvectors::strWeights();

  return ss.str();
}


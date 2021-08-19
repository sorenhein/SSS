#include <iostream>
#include <iomanip>
#include <sstream>

#include "Extensions.h"

// TMP
#include "../../utils/Timer.h"
extern vector<Timer> timersStrat;


Extensions::Extensions()
{
  Extensions::reset();
}


Extensions::~Extensions()
{
}


void Extensions::reset()
{
  extensions.clear();
  splits1.reset();
  splits2.reset();
}


void Extensions::split(
  Strategies& strategies,
  const Strategy& counterpart,
  const ExtensionSplit split)
{
  // Split strategies into unique and overlapping parts with the
  // distributions in counterpart.

  if (split == EXTENSION_SPLIT1)
    splits1.split(strategies, counterpart);
  else
    splits2.split(strategies, counterpart);
}


void Extensions::makeEntry(
  const Strategy& strat1,
  const Strategy& strat2,
  const Ranges& ranges,
  const unsigned index1,
  const unsigned index2)
{
  auto& entry = extensions.back();
  entry.multiply(strat1, strat2, ranges);
  entry.finish(index1, index2,
    splits1.weight(index1) + splits2.weight(index2));
}


bool Extensions::greaterEqualByTricks(
  const Extension& ext1,
  const Extension& ext2,
  CompareDetail& compOverlap) const
{
  const CompareDetail c1 = splits1.compareDetail(ext1.index1(), ext2.index1());
  if (c1 == WIN_SECOND_PRIMARY || c1 == WIN_DIFFERENT_PRIMARY)
    return false;

  const CompareDetail c2 = splits2.compareDetail(ext1.index2(), ext2.index2());
  if (c2 == WIN_SECOND_PRIMARY || c2 == WIN_DIFFERENT_PRIMARY)
    return false;

  compOverlap = ext1.compareDetail(ext2);
  if (compOverlap == WIN_SECOND_PRIMARY || 
      compOverlap == WIN_DIFFERENT_PRIMARY)
    return false;

  return true;
}


Compare Extensions::compareDetail(
  const Extension& ext1,
  const Extension& ext2,
  const CompareDetail& compOverlap) const
{
  const CompareDetail c1 = splits1.compareDetail(ext1.index1(), ext2.index1());
  const CompareDetail c2 = splits2.compareDetail(ext1.index2(), ext2.index2());
  CompareDetail cum = static_cast<CompareDetail>(c1 | c2 | compOverlap);

  // Like in the Strategy compare methods, this could go somewhere
  // more central.

  if (cum & WIN_DIFFERENT_PRIMARY)
    return WIN_DIFFERENT;

  if (cum & WIN_FIRST_PRIMARY)
  {
    if (cum & WIN_SECOND_PRIMARY)
      return WIN_DIFFERENT;
    else
      return WIN_FIRST;
  }
  else if (cum & WIN_SECOND_PRIMARY)
    return WIN_SECOND;

  if (cum & WIN_DIFFERENT_SECONDARY)
    return WIN_DIFFERENT;

  if (cum & WIN_FIRST_SECONDARY)
  {
    if (cum & WIN_SECOND_SECONDARY)
      return WIN_DIFFERENT;
    else
      return WIN_FIRST;
  }
  else if (cum & WIN_SECOND_SECONDARY)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


void Extensions::add()
{
  // Process the scratchpad element.
  auto piter = prev(extensions.end());

  if (extensions.size() == 1)
  {
    // Keep the product and make a new scratch-pad element.
    extensions.emplace_back(Extension());
    return;
  }

  // The strategies list is in descending order of weights.
  // The new Strategy might dominate anything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  // This checking costs about one third of the overall method time.
  
  CompareDetail c;
  auto iter = extensions.begin();
  while (iter != piter && iter->weight() > piter->weight())
  {
    if (Extensions::greaterEqualByTricks(* iter, * piter, c))
    {
      // The new strat is dominated.
      return;
    }
    else
      iter++;
  }

  while (iter != piter && iter->weight() == piter->weight())
  {
    if (Extensions::greaterEqualByTricks(* iter, * piter, c))
    {
      // Same tricks.
      Compare d = Extensions::compareDetail(* iter, * piter, c);
      if (d == WIN_FIRST || d == WIN_EQUAL)
        return;
      else if (d == WIN_SECOND)
      {
        iter = extensions.erase(iter);
      }
      else
        iter++;
    }
    else
      iter++;
  }

  // Already in the right place at the end?
  if (iter == piter)
  {
    extensions.emplace_back(Extension());
    return;
  }

  // The new vector must be inserted, i.e. spliced in.
  // This is super-fast.
  extensions.splice(iter, extensions, piter);
  piter = prev(iter);

  // The new vector may dominate lighter vectors.  This is also
  // quite efficient and doesn't happen so often.
  while (iter != extensions.end())
  {
    if (Extensions::greaterEqualByTricks(* piter, * iter, c))
      iter = extensions.erase(iter);
    else
      iter++;
  }

  // Make a new scratch-pad element.
  extensions.emplace_back(Extension());
}


void Extensions::multiply(const Ranges& ranges)
{
  // Multiply the two splits efficiently.

  // Scratchpad element.
  extensions.emplace_back(Extension());

  unsigned i = 0;
  for (auto& strat1: splits1.sharedStrategies())
  {
    unsigned j = 0;
    for (auto& strat2: splits2.sharedStrategies())
    {
      Extensions::makeEntry(strat1, strat2, ranges, i, j);
      Extensions::add();
      j++;
    }
    i++;
  }

  // Remove scratchpad element.
  extensions.pop_back();
}


void Extensions::flatten(list<Strategy>& strategies)
{
  // Add back the non-overlapping results explicitly.
  for (auto& ext: extensions)
    ext.flatten(strategies, 
      splits1.ownStrategy(ext.index1()), 
      splits2.ownStrategy(ext.index2()));
}


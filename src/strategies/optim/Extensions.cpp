/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Extensions.h"


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
  Slist& slist,
  const Strategy& counterpart,
  const Ranges& ranges,
  const ExtensionSplit split)
{
  // Split strategies into unique and overlapping parts with the
  // distributions in counterpart.

  if (split == EXTENSION_SPLIT1)
    splits1.split(slist, counterpart, ranges);
  else
    splits2.split(slist, counterpart, ranges);
}


void Extensions::split(
  Slist& slist1,
  Slist& slist2,
  const Ranges& ranges)
{
  Extensions::split(slist1, slist2.front(), ranges, EXTENSION_SPLIT1);
  Extensions::split(slist2, slist1.front(), ranges, EXTENSION_SPLIT2);
}


bool Extensions::productDominatedHeavier(
  list<Extension>::iterator& iter,
  const Extension& product) const
{
  // product comes after iter and is the new product which may or
  // may not be dominated by earlier Extension's.

  while (iter->weight() > product.weight())
  {
    if (product.lessEqualPrimary(* iter))
    {
      // The new strat is dominated.
      return true;
    }
    else
      iter++;
  }
  return false;
}


bool Extensions::processSameWeights(
  list<Extension>::iterator& iter,
  list<Extension>::iterator& piter)
{
  // piter comes after iter and is the new product.  If there is
  // a domination, it may go in either direction.  Returns true
  // if the new product is dominated.

  while (iter != piter && iter->weight() == piter->weight())
  {
    if (piter->lessEqualPrimary(* iter))
    {
      // Same tricks.
      Compare c = iter->compareSecondary(* piter);
      if (c == WIN_FIRST || c == WIN_EQUAL)
        return true;
      else if (c == WIN_SECOND)
        iter = extensions.erase(iter);
      else
        iter++;
    }
    else
      iter++;
  }
  return false;
}


void Extensions::eraseDominatedLighter(
  list<Extension>::iterator& iter,
  list<Extension>::iterator& piter)
{
  // piter comes before iter and is the new product which may
  // dominate later Extension's.

  while (iter != extensions.end())
  {
    if (iter->lessEqualPrimary(* piter))
      iter = extensions.erase(iter);
    else
      iter++;
  }
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
  
  auto iter = extensions.begin();

  if (Extensions::productDominatedHeavier(iter, * piter))
    // The new strat is dominated by an Extension with more weight.
    return;

  if (Extensions::processSameWeights(iter, piter))
    // The new strat is dominated by an Extension with equal weight.
    return;

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
  Extensions::eraseDominatedLighter(iter, piter);

  // Make a new scratch-pad element.
  extensions.emplace_back(Extension());
}


void Extensions::multiply(const Ranges& ranges)
{
  // Multiply the two splits efficiently.

  // Scratchpad element.
  extensions.emplace_back(Extension());

  for (auto& split1: splits1.splitStrategies())
  {
    for (auto& split2: splits2.splitStrategies())
    {
      auto& entry = extensions.back();
      entry.multiply(split1, split2, ranges);
      Extensions::add();
    }
  }

  // Remove scratchpad element.
  extensions.pop_back();
}


void Extensions::flatten(Slist& slist)
{
  // Add back the non-overlapping results explicitly.

  slist.clear();
  for (auto& ext: extensions)
    ext.flatten(slist);
}


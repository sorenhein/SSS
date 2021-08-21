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
  Strategies& strategies,
  const Strategy& counterpart,
  const Ranges& ranges,
  const ExtensionSplit split)
{
  // Split strategies into unique and overlapping parts with the
  // distributions in counterpart.

  if (split == EXTENSION_SPLIT1)
    splits1.split(strategies, counterpart, ranges);
  else
    splits2.split(strategies, counterpart, ranges);
}


/*
void Extensions::makeEntry(
  const Strategy& strat1,
  const Strategy& strat2,
  const Ranges& ranges,
  const unsigned index1,
  const unsigned index2)
{
  auto& entry = extensions.back();
// cout << "makeEntry (old way)\n";
// cout << strat1.str("strat1", true);
// cout << strat2.str("strat2", true);
  entry.multiply(strat1, strat2, ranges);
  entry.finish(index1, index2,
    splits1.weight(index1) + splits2.weight(index2));
}


#include <iostream>
bool Extensions::lessEqualPrimary(
  const Extension& ext1,
  const Extension& ext2) const
{
  if (! splits1.lessEqualPrimary(ext1.index1(), ext2.index1()))
  {
// cout << "B1'\n";
    return false;
  }
  else if (! splits2.lessEqualPrimary(ext1.index2(), ext2.index2()))
  {
// cout << "B2'\n";
    return false;
  }
  else
  {
// cout << "B3'\n";
    return ext1.lessEqualPrimary(ext2);
  }
}


Compare Extensions::compareSecondary(
  const Extension& ext1,
  const Extension& ext2) const
{
  Compare c = splits1.compareSecondary(ext1.index1(), ext2.index1());
  c *= splits2.compareSecondary(ext1.index2(), ext2.index2());
  c *= ext1.compareSecondary(ext2);

  return c;
}
*/


/*
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
  
// cout << "STARTOLD\n";
// piter->strOld(
      // splits1.ownStrategy(piter->index1()), 
      // splits2.ownStrategy(piter->index2()));

  auto iter = extensions.begin();
  while (iter != piter && iter->weight() > piter->weight())
  {
// cout << "iter\n";
// iter->strOld(
      // splits1.ownStrategy(iter->index1()), 
      // splits2.ownStrategy(iter->index2()));

    if (Extensions::lessEqualPrimary(* piter, * iter))
    {
// cout << "A1\n";
      // The new strat is dominated.
      return;
    }
    else
    {
// cout << "A2\n";
      iter++;
    }
  }

  while (iter != piter && iter->weight() == piter->weight())
  {
    if (Extensions::lessEqualPrimary(* piter, * iter))
    {
      // Same tricks.
      Compare c = Extensions::compareSecondary(* iter, * piter);
      if (c == WIN_FIRST || c == WIN_EQUAL)
      {
// cout << "A3\n";
        return;
      }
      else if (c == WIN_SECOND)
      {
// cout << "A4\n";
        iter = extensions.erase(iter);
      }
      else
      {
// cout << "A5\n";
        iter++;
      }
    }
    else
    {
// cout << "A6\n";
      iter++;
    }
  }

  // Already in the right place at the end?
  if (iter == piter)
  {
// cout << "A7\n";
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
    if (Extensions::lessEqualPrimary(* iter, * piter))
    {
// cout << "A8\n";
      iter = extensions.erase(iter);
    }
    else
    {
// cout << "A9\n";
      iter++;
    }
  }

  // Make a new scratch-pad element.
  extensions.emplace_back(Extension());
}
*/


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
  
// cout << "STARTNEW" << endl;
// piter->strNew();
  auto iter = extensions.begin();
  while (iter != piter && iter->weight() > piter->weight())
  {

// cout << "iter" << endl;
// iter->strNew();

    if (piter->lessEqualPrimaryNew(* iter))
    {
// cout << "A1\n";
      // The new strat is dominated.
      return;
    }
    else
    {
// cout << "A2\n";
      iter++;
    }
  }

  while (iter != piter && iter->weight() == piter->weight())
  {
    if (piter->lessEqualPrimaryNew(* iter))
    {
      // Same tricks.
      Compare c = iter->compareSecondaryNew(* piter);
      if (c == WIN_FIRST || c == WIN_EQUAL)
      {
// cout << "A3\n";
        return;
      }
      else if (c == WIN_SECOND)
      {
// cout << "A4\n";
        iter = extensions.erase(iter);
      }
      else
      {
// cout << "A5\n";
        iter++;
      }
    }
    else
    {
// cout << "A6\n";
      iter++;
    }
  }

  // Already in the right place at the end?
  if (iter == piter)
  {
    extensions.emplace_back(Extension());
// cout << "A7\n";
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
    if (iter->lessEqualPrimaryNew(* piter))
    {
// cout << "A8\n";
      iter = extensions.erase(iter);
    }
    else
    {
// cout << "A9\n";
      iter++;
    }
  }

  // Make a new scratch-pad element.
  extensions.emplace_back(Extension());
}


#include <iostream>
void Extensions::multiply(const Ranges& ranges)
{
  // Multiply the two splits efficiently.

  // Scratchpad element.
  extensions.emplace_back(Extension());

  /*
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
  */

  for (auto& split1: splits1.splitStrategies())
  {
    for (auto& split2: splits2.splitStrategies())
    {
      auto& entry = extensions.back();
      entry.multiplyNew(split1, split2, ranges);
      Extensions::add();
    }
  }

  // Remove scratchpad element.
  extensions.pop_back();
}


void Extensions::flatten(list<Strategy>& strategies)
{
  // Add back the non-overlapping results explicitly.
  /*
  for (auto& ext: extensions)
    ext.flatten(strategies, 
      splits1.ownStrategy(ext.index1()), 
      splits2.ownStrategy(ext.index2()));
      */

  for (auto& ext: extensions)
    ext.flattenNew(strategies);

  /*
  if (strategies.size() != strats2.size())
  {
    cout << "Different sizes: " << strategies.size() << " vs " <<
      strats2.size() << endl;
    assert(false);
  }
  auto siter1 = strategies.begin();
  auto siter2 = strats2.begin();
  unsigned i = 0;
  while (siter1 != strategies.end())
  {
    if (! (* siter1 == * siter2))
    {
      cout << "Difference for i = " << i << endl;
      for (auto& s: strategies)
        cout << s.str("orig", true);
      cout << endl;

      for (auto& s: strats2)
        cout << s.str("new", true);
      cout << endl;

      assert(false);
    }
    siter1++;
    siter2++;
    i++;
  }
  */
}


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


bool Extensions::lessEqualPrimary(
  const Extension& ext1,
  const Extension& ext2) const
{
  if (! splits1.lessEqualPrimary(ext1.index1(), ext2.index1()))
    return false;
  else if (! splits2.lessEqualPrimary(ext1.index2(), ext2.index2()))
    return false;
  else
    return ext1.lessEqualPrimary(ext2);
}


Compare Extensions::compareDetail(
  const Extension& ext1,
  const Extension& ext2) const
{
  const CompareDetail c1 = splits1.compareDetail(ext1.index1(), ext2.index1());
  const CompareDetail c2 = splits2.compareDetail(ext1.index2(), ext2.index2());
  const CompareDetail compOverlap = ext1.compareDetail(ext2);

  CompareDetail cum = static_cast<CompareDetail>(c1 | c2 | compOverlap);

  const Compare cnew1 = splits1.compareSecondary(ext1.index1(), ext2.index1());
  const Compare cnew2 = splits2.compareSecondary(ext1.index2(), ext2.index2());

  assert(compOverlap == WIN_EQUAL_OVERALL ||
         compOverlap == WIN_FIRST_SECONDARY ||
         compOverlap == WIN_SECOND_SECONDARY ||
         compOverlap == WIN_DIFFERENT_SECONDARY);


  // TODO Need a Compare class with a proper combine() or += or *=
  Compare cnew;
  if (cnew1 == WIN_DIFFERENT || 
      cnew2 == WIN_DIFFERENT ||
      compOverlap == WIN_DIFFERENT_SECONDARY)
    cnew = WIN_DIFFERENT;
  else if (cnew1 == WIN_EQUAL)
  {
    if (compOverlap == WIN_EQUAL_OVERALL)
      cnew = cnew2;
    else if (compOverlap == WIN_FIRST_SECONDARY)
      cnew = (cnew2 == WIN_SECOND ? WIN_DIFFERENT : WIN_FIRST);
    else if (compOverlap == WIN_SECOND_SECONDARY)
      cnew = (cnew2 == WIN_FIRST ? WIN_DIFFERENT : WIN_SECOND);
    else
    {
      assert(false);
      cnew = WIN_UNSET;
    }
      
  }
  else if (cnew2 == WIN_EQUAL)
  {
    if (compOverlap == WIN_EQUAL_OVERALL)
      cnew = cnew1;
    else if (compOverlap == WIN_SECOND_SECONDARY)
      cnew = (cnew1 == WIN_SECOND ? WIN_SECOND : WIN_DIFFERENT);
    else if (compOverlap == WIN_FIRST_SECONDARY)
      cnew = (cnew1 == WIN_FIRST ? WIN_FIRST : WIN_DIFFERENT);
    else
    {
      assert(false);
      cnew = WIN_UNSET;
    }
  }
  else if (compOverlap == WIN_EQUAL_OVERALL)
  {
    if (cnew1 == cnew2)
      cnew = cnew1;
    else
      cnew = WIN_DIFFERENT;
    // cnew = (cnew1 == cnew2 ? cnew1 : WIN_DIFFERENT);
  }
  else if (cnew1 == cnew2)
  {
    if (cnew1 == WIN_FIRST && compOverlap == WIN_FIRST_SECONDARY)
      cnew = WIN_FIRST;
    else if (cnew1 == WIN_SECOND && compOverlap == WIN_SECOND_SECONDARY)
      cnew = WIN_SECOND;
    else
      cnew = WIN_DIFFERENT;
  }
  else
    cnew = WIN_DIFFERENT;


  // Like in the Strategy compare methods, this could go somewhere
  // more central.

  if (cum & WIN_DIFFERENT_PRIMARY)
  {
    assert(cnew == WIN_DIFFERENT);
    return WIN_DIFFERENT;
  }

  if (cum & WIN_FIRST_PRIMARY)
  {
    if (cum & WIN_SECOND_PRIMARY)
    {
      assert(cnew == WIN_DIFFERENT);
      return WIN_DIFFERENT;
    }
    else
    {
      assert(cnew == WIN_FIRST);
      return WIN_FIRST;
    }
  }
  else if (cum & WIN_SECOND_PRIMARY)
  {
    assert(cnew == WIN_SECOND);
    return WIN_SECOND;
  }

  if (cum & WIN_DIFFERENT_SECONDARY)
  {
    assert(cnew == WIN_DIFFERENT);
    return WIN_DIFFERENT;
  }

  if (cum & WIN_FIRST_SECONDARY)
  {
    if (cum & WIN_SECOND_SECONDARY)
    {
      // if (cnew != WIN_DIFFERENT)
      // {
        // cout << "cnew1 " << cnew1 << ", cnew2 " << cnew2 << 
          // ", compOverlap " << compOverlap << endl;
        // cout << "cnew " << cnew << endl;
        // cout << "c1 " << c1 << ", c2 " << c2 << endl;
        // cout << "cum " << cum << endl;
      // }
      assert(cnew == WIN_DIFFERENT);
      return WIN_DIFFERENT;
    }
    else
    {
      // if (cnew != WIN_FIRST)
      // {
        // cout << "cnew1 " << cnew1 << ", cnew2 " << cnew2 << 
          // ", compOverlap " << compOverlap << endl;
        // cout << "cnew " << cnew << endl;
        // cout << "c1 " << c1 << ", c2 " << c2 << endl;
        // cout << "cum " << cum << endl;
      // }
      assert(cnew == WIN_FIRST);
      return WIN_FIRST;
    }
  }
  else if (cum & WIN_SECOND_SECONDARY)
  {
    assert(cnew == WIN_SECOND);
    return WIN_SECOND;
  }
  else
  {
    assert(cnew == WIN_EQUAL);
    return WIN_EQUAL;
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
  while (iter != piter && iter->weight() > piter->weight())
  {
    if (Extensions::lessEqualPrimary(* piter, * iter))
    {
      // The new strat is dominated.
      return;
    }
    else
      iter++;
  }

  while (iter != piter && iter->weight() == piter->weight())
  {
    if (Extensions::lessEqualPrimary(* piter, * iter))
    {
      // Same tricks.
      Compare c = Extensions::compareDetail(* iter, * piter);
      if (c == WIN_FIRST || d == WIN_EQUAL)
        return;
      else if (c == WIN_SECOND)
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
    if (Extensions::lessEqualPrimary(* iter, * piter))
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


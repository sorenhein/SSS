/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include "Extension.h"


Extension::Extension()
{
  Extension::reset();
}


Extension::~Extension()
{
}


void Extension::reset()
{
  overlap.reset();
  own1ptr = nullptr;
  own2ptr = nullptr;
  weightInt = 0;
}


/* */
#include <iostream>
void Extension::multiply(
  const Strategy& strat1,
  const Strategy& strat2,
  const Ranges& ranges)
{
  overlap.multiply(strat1, strat2);
  overlap.scrutinize(ranges);

// cout << overlap.str("overlap in multiply (old)", true);
}
/* */


void Extension::multiplyNew(
  const Split& split1,
  const Split& split2,
  const Ranges& ranges)
{
  overlap.multiply(* split1.sharedPtr, * split2.sharedPtr);
  overlap.scrutinize(ranges);

  own1ptr = split1.ownPtr;
  own2ptr = split2.ownPtr;
// cout << own1ptr->str("own1 in multiplyNew", true);
// cout << own2ptr->str("own2 in multiplyNew", true);
// cout << split1.sharedPtr->str("shared1", true);
// cout << split2.sharedPtr->str("shared2", true);
// cout << overlap.str("overlap in multiplyNew", true);

  weightInt = overlap.weight() + own1ptr->weight() + own2ptr->weight();
}


/* */
void Extension::finish(
  const unsigned index1,
  const unsigned index2,
  const unsigned weightUniques)
{
  index1Int = index1;
  index2Int = index2;

  weightInt = overlap.weight() + weightUniques;
}


void Extension::flatten(
  list<Strategy>& strategies,
  const Strategy& strat1,
  const Strategy& strat2)
{
  overlap *= strat1;
  overlap *= strat2;
  strategies.push_back(move(overlap));
}
/* */


void Extension::flattenNew(list<Strategy>& strategies)
{
  overlap *= * own1ptr;
  overlap *= * own2ptr;
  strategies.push_back(move(overlap));
}


/* */
bool Extension::lessEqualPrimary(const Extension& ext2) const
{
  return overlap.lessEqualPrimaryScrutinized(ext2.overlap);
}
/* */


#include <iostream>
bool Extension::lessEqualPrimaryNew(const Extension& ext2) const
{
  /*
  if (own1ptr->empty())
    return (ext2.own1ptr->empty() ? true : false);
  else if (ext2.own1ptr->empty())
    return false;
    */

  if (! own1ptr->lessEqualPrimaryScrutinized(* ext2.own1ptr))
  {
// cout << "  B1\n";
    return false;
  }
  else if (! own2ptr->lessEqualPrimaryScrutinized(* ext2.own2ptr))
  {
// cout << "  B2\n";
    return false;
  }
  else
  {
// cout << "  B3\n";
    return overlap.lessEqualPrimaryScrutinized(ext2.overlap);
  }
}


/* */
Compare Extension::compareSecondary(const Extension& ext2) const
{
  return overlap.compareSecondary(ext2.overlap);
}
/* */


Compare Extension::compareSecondaryNew(const Extension& ext2) const
{
  /*
  if (own1ptr->empty())
    return (ext2.own1ptr->empty() ? WIN_EQUAL : WIN_FIRST);
  else if (ext2.own1ptr->empty())
    return WIN_SECOND;
    */

  Compare c = own1ptr->compareSecondary(* ext2.own1ptr);
  c *= own2ptr->compareSecondary(* ext2.own2ptr);
  c *= overlap.compareSecondary(ext2.overlap);
  
  return c;
}


/* */
unsigned Extension::index1() const
{
  return index1Int;
}


unsigned Extension::index2() const
{
  return index2Int;
}
/* */


unsigned Extension::weight() const
{
  return weightInt;
}


#include <iostream>
void Extension::strOld(
  const Strategy& own1,
  const Strategy& own2) const
{
  cout << overlap.str("old overlap", true);
  cout << "indices " << index1Int << ", " << index2Int << endl;
  cout << "weight " << weightInt << endl;
  cout << own1.str("own1", true);
  cout << own2.str("own2", true);
}


void Extension::strNew() const
{
  cout << overlap.str("new overlap", true);
  cout << "weight " << weightInt << endl;
  if (own1ptr)
    cout << own1ptr->str("own1", true);
  else
    cout << "(empty1)\n";
  if (own2ptr)
    cout << own2ptr->str("own2", true);
  else
    cout << "(empty2)\n";
}


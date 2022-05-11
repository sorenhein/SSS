/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_STACKENTRY_H
#define SSS_STACKENTRY_H

#include <set>

#include "tableau/CoverTableau.h"

class Tricks;


using namespace std;


template<class T>
struct StackEntry
{
  CoverTableau tableau;

  typename set<T>::const_iterator iter;

  StackEntry(
    const Tricks& tricks,
    const unsigned char tmin,
    typename const set<T>::const_iterator& iterIn)
  {
    // This method is for the row version.
    tableau.init(tricks, tmin);
    iter = iterIn;
  };

  StackEntry(
    typename const set<T>::const_iterator& iterIn,
    const CoverTableau& tableauIn,
    const Tricks& additions,
    const unsigned rawWeightAdded,
    const unsigned rowNumber)
  {
    // This method only makes sense for the Cover version.
    iter = iterIn;
    tableau = tableauIn;
    tableau.extendRow(* iterIn, additions, rawWeightAdded, rowNumber);
  };

  StackEntry(
    typename const set<T>::const_iterator& iterIn,
    const CoverTableau& tableauIn)
  {
    // This method is used for both versions.
    iter = iterIn;
    tableau = tableauIn;
    tableau.addRow(* iterIn);
  };

  bool operator < (const StackEntry& se2) const
  {
    return (tableau < se2.tableau);
  };
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSTACK_H
#define SSS_COVERSTACK_H

#include <set>
#include <list>

#include "tableau/CoverTableau.h"

class Cover;
class CoverRow;
class Tricks;


using namespace std;


struct CoverStackEntry
{
  CoverTableau tableau;

  set<Cover>::const_iterator iter;

  bool operator < (const CoverStackEntry& se2) const
  {
    return (tableau < se2.tableau);
  };
};

struct RowStackEntry
{
  CoverTableau tableau;

  list<CoverRow>::const_iterator iter;

  bool operator < (const RowStackEntry& rse2) const
  {
    return (tableau < rse2.tableau);
  };
};

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

template<class T>
class CoverStack
{
  private:

    list<StackEntry<T>> stack;

  public:

    CoverStack();

    void reset();

    void emplace(
      const Tricks& tricks,
      const unsigned char tmin,
      typename const set<T>::const_iterator& iterIn);

    void emplace(
      typename const set<T>::const_iterator& iterIn,
      const CoverTableau& tableau,
      const Tricks& additions,
      const unsigned rawWeightAdded,
      const unsigned rowNumber);

    void emplace(
      typename const set<T>::const_iterator& iterIn,
      const CoverTableau& tableau);

    typedef typename list<StackEntry<T>>::iterator Liter;

    // typename list<StackEntry<T>>::iterator erase(
      // typename list<StackEntry<T>>::iterator iterIn) 
    Liter erase(Liter iterIn) 
    {
      return stack.erase(iterIn);
    };

    // typename list<StackEntry<T>>::iterator begin() 
    Liter begin()
    { 
      return stack.begin(); 
    };

    // typename list<StackEntry<T>>::iterator end() 
    Liter end()
    { 
      return stack.end(); 
    };
};

#endif

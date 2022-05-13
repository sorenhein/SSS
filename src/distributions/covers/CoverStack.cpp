/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverStack.h"
#include "Cover.h"

mutex mtxCoverStack;


template<class T>
CoverStack<T>::CoverStack()
{
  CoverStack::clear();
}


template<class T>
void CoverStack<T>::emplace(
  const Tricks& tricks,
  const unsigned char tmin,
  typename const set<T>::const_iterator& iterIn)
{
  // This method is for the row version.
  lock_guard<mutex> lg(mtxCoverStack);
  multiset<StackEntry<T>>::emplace(StackEntry<T>(tricks, tmin, iterIn));
}


template<class T>
void CoverStack<T>::emplace(
  typename const set<T>::const_iterator& iterIn,
  const CoverTableau& tableauIn,
  const Tricks& additions,
  const unsigned rawWeightAdded,
  const unsigned rowNumber)
{
  // This method is for the cover version.
  lock_guard<mutex> lg(mtxCoverStack);
  multiset<StackEntry<T>>::emplace(StackEntry<T>(iterIn, tableauIn, 
    additions, rawWeightAdded, rowNumber));
}


template<class T>
void CoverStack<T>::emplace(
  typename const set<T>::const_iterator& iterIn,
  const CoverTableau& tableauIn)
{
  lock_guard<mutex> lg(mtxCoverStack);
  multiset<StackEntry<T>>::emplace(StackEntry<T>(iterIn, tableauIn));
}


template<class T>
unsigned CoverStack<T>::prune(const CoverTableau& solution)
{
  // TODO Not efficient
  StackEntry<T> se;
  se.tableau = solution;
  auto iter =  multiset<StackEntry<T>>::lower_bound(se);

  const unsigned num = distance(iter, this->end());

  // TODO Go back, also taking into account min complexity of 2?
  multiset<StackEntry<T>>::erase(iter, this->end());

  return num;
}


template class CoverStack<Cover>;
template class CoverStack<CoverRow>;

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSTACK_H
#define SSS_COVERSTACK_H

#include <set>

#include "StackEntry.h"

class CoverTableau;
class Tricks;


using namespace std;


template<class T>
class CoverStack: public multiset<StackEntry<T>>
{
  public:

    CoverStack();

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

    size_t prune(const CoverTableau& solution);

};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SURVIVORLIST_H
#define SSS_SURVIVORLIST_H

#include <list>


using namespace std;


// Survivor is used in the context of rank-reduced distributions.
// It can happen that a NS card leads two EW ranks to collapse after
// the trick.  The parent trick has a number of possible distributions,
// say 4, and the child trick only has 3 tricks.  These have to be
// mapped to one another.  There is a Survivor for each parent
// distribution.  The fullNo is the parent number and the reducedNo
// is the child number.

struct SurvivorList
{
  struct Survivor
  {
    unsigned char fullNo;
    unsigned char reducedNo;
  };


  list<Survivor> distNumbers;
  unsigned char reducedSize;


  SurvivorList();

  void clear();

  void resize(const unsigned len);

  void push_back(const Survivor& survivor);

  unsigned sizeFull() const;

  unsigned char sizeReduced() const;

  string str() const;
};

#endif

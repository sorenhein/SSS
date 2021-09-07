/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SURVIVORLIST_H
#define SSS_SURVIVORLIST_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>


using namespace std;


// Survivor is used in the context of rank-reduced distributions.
// It can happen that a NS card leads two EW ranks to collapse after
// the trick.  The parent trick has a number of possible distributions,
// say 4, and the child trick only has 3 tricks.  These have to be
// mapped to one another.  There is a Survivor for each parent
// distribution.  The fullNo is the parent number and the reducedNo
// is the child number.

struct Survivor
{
  unsigned char fullNo;
  unsigned char reducedNo;
};


struct SurvivorList
{
  list<Survivor> distNumbers;
  unsigned char reducedSize;

  void clear()
  {
    distNumbers.clear();
    reducedSize = 0;
  };

  void resize(const unsigned len)
  {
    distNumbers.resize(len);
  };

  void push_back(const Survivor& survivor)
  {
    distNumbers.push_back(survivor);
  };

  unsigned sizeFull() const
  {
    return distNumbers.size();
  };

  unsigned char sizeReduced() const
  {
    return reducedSize;
  };

  string str() const
  {
    stringstream ss;
    ss << "Survivor list\n";
    for (auto& s: distNumbers)
      ss << +s.fullNo << ", " << +s.reducedNo << endl;
    return ss.str() + "\n";

  };
};

#endif


/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLSTAT_H
#define SSS_EXPLSTAT_H

#include <vector>
#include <list>
#include <string>

using namespace std;


struct ExplStat
{

  vector<unsigned> lengths;
  vector<unsigned> singles;
  vector<vector<unsigned>> pairs;

  ExplStat();

  void reset();

  void resize(const unsigned pairSize);

  bool empty() const;

  void incrLengths(const unsigned count);

  void incrSingles(const unsigned index);

  void incrPairs(
    const unsigned index1,
    const unsigned index2);

  string strLengths() const;

  string strPairs() const;

  string str() const;
};

#endif


/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLSTATS_H
#define SSS_EXPLSTATS_H

#include <vector>
#include <list>
#include <string>

class CoverSpec;

using namespace std;


struct ExplStat
{
  vector<unsigned> lengths;
  vector<unsigned> singles;
  vector<vector<unsigned>> pairs;

  void incrLengths(const unsigned count)
  {
    assert(count < lengths.size());
    lengths[count]++;
  };

  void incrSingles(const unsigned index)
  {
    assert(index < singles.size());
    singles[index]++;
  };

  void incrPairs(
    const unsigned index1,
    const unsigned index2)
  {
    assert(index1 < pairs.size());
    assert(index2 < pairs.size());

    pairs[index1][index2]++;
    pairs[index2][index1]++;
  };
};


class ExplStats
{
  private:

    vector<vector<ExplStat>> explStats;

  public:

  ExplStats();

  void reset();

  void resize(const vector<vector<list<CoverSpec>>>& specs);

  ExplStat& getEntry(
    const unsigned char lengthIndex,
    const unsigned char tops1Index);

  string str() const;
};

#endif


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

#include "ExplStat.h"

class CoverSpec;

using namespace std;


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

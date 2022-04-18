
/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLSTAT_H
#define SSS_EXPLSTAT_H

#include <vector>
#include <string>

using namespace std;


class ExplStat
{
  private:

    vector<unsigned> lengths;


    string strLengths() const;

    string strPairs() const;


  public:

    ExplStat();

    void reset();

    void resize();

    void incrLengths(const unsigned count);

    string str() const;
};

#endif

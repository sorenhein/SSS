/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

// Compares vectors, taking into account the possibility that
// there may not be an ordering available.

#ifndef SSS_COMPARER_H
#define SSS_COMPARER_H

#include <vector>
#include <string>

#include "Compare.h"

using namespace std;


class Comparer
{
  private:

    unsigned dim1;
    unsigned dim2;

    vector<vector<Compare>> matrix;


    void makeMarginals(
      vector<vector<unsigned>>& marginal1,
      vector<vector<unsigned>>& marginal2) const;

    void summarize(
      const vector<vector<unsigned>>& marginal,
      vector<unsigned>& sum) const;

    void makeSums(
      vector<unsigned>& sum1,
      vector<unsigned>& sum2) const;

    string strHeader() const;

  public:

    Comparer();

    void reset();

    void resize(
      const unsigned size1,
      const unsigned size2);

    void log(
      const unsigned n1,
      const unsigned n2,
      const Compare cmp);

    bool logForEquality(
      const unsigned n1,
      const unsigned n2,
      const Compare cmp);

    Compare compare() const;

    bool equal() const;

    string str() const;
};

#endif

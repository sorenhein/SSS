/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RANGE_H
#define SSS_RANGE_H

#include "MultiResult.h"

#include <string>

class Result;

using namespace std;


class Range
{
  private:

    MultiResult minimum;
    MultiResult resultHigh;
    MultiResult resultLow;

  public:

    void init(const Result& result);

    void extend(const Result& result);

    void operator *= (const Range& range2);

    bool operator < (const Range& range2) const;

    bool constant() const;

    void constantResult(Result& result) const;

    unsigned char dist() const;

    unsigned char min() const;

    string strHeader(const bool rankFlag) const;

    string str(const bool rankFlag) const;
};

#endif

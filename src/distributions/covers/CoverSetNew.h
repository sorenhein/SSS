/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSETNEW_H
#define SSS_COVERSETNEW_H

#include <vector>
#include <string>

#include "CoverElement.h"

using namespace std;


class CoverSetNew
{
  private:

    bool symmFlag;

    unsigned char complexity;

    CoverElement length;

    vector<CoverElement> tops;


  public:

  CoverSetNew();

  void reset();

  void resize(const unsigned compSize);

  void set(
    const unsigned char lenActual,
    const unsigned char lenLow,
    const unsigned char lenHigh,
    vector<unsigned char>& topsLow,
    vector<unsigned char>& topsHigh);

  bool includes(
    const unsigned char lengthIn,
    const vector<unsigned>& topsIn);

  unsigned char getComplexity() const;

  string strHeader() const;

  string strLine(const unsigned char lenActual) const;

};

#endif

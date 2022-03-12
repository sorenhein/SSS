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

    unsigned char range;
    
    unsigned char topSize; // Last used top number + 1; may be 0
    unsigned char topCount; // Number of tops that are not unused

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
    const vector<unsigned char>& topsActual,
    const vector<unsigned char>& topsLow,
    const vector<unsigned char>& topsHigh);

  bool includes(
    const unsigned char lengthIn,
    const vector<unsigned>& topsIn) const;

  unsigned char getComplexity() const;

  unsigned char getTopSize() const;

  unsigned char getRangeSum() const;

  bool explainable() const;

  string strHeader() const;

  string strLine(
    const unsigned char lenActual,
    const vector<unsigned char>& topsActual) const;

  string strLine() const;

  string strVerbal(
    const unsigned char maxLength,
    const vector<unsigned char>& topTotals) const;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TERMCOMPARE_H
#define SSS_TERMCOMPARE_H

#include <vector>

#include "TermHelp.h"

using namespace std;


class TermCompare
{
  private:

    vector<bool> lookup;

    void setConstants();

  public:

    TermCompare();

    unsigned short getIndex(
      const unsigned char lower,
      const unsigned char upper,
      const CoverOperator oper) const;

    bool includes(
      const unsigned short index,
      const unsigned char value) const;

    unsigned char getData(
      const bool usedFlag,
      const unsigned char range,
      const unsigned char complexity) const;

    bool used(const unsigned char data) const;

    unsigned char range(const unsigned char data) const;

    unsigned char complexity(const unsigned char data) const;
};

#endif

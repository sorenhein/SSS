/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_FACTOREDPRODUCT_H
#define SSS_FACTOREDPRODUCT_H

#include <cassert>

class Product;

using namespace std;


struct FactoredProduct
{
  Product * noncanonicalPtr;

  Product * canonicalPtr;

  // Number of unused tops that are elided.
  unsigned canonicalShift;


  unsigned char getComplexity() const;
};

#endif

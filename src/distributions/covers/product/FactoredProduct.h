/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_FACTOREDPRODUCT_H
#define SSS_FACTOREDPRODUCT_H

class Product;
class Profile;

using namespace std;


struct FactoredProduct
{
  Product * noncanonicalPtr;

  Product * canonicalPtr;

  // Number of unused tops that are elided.
  unsigned char canonicalShift;


  bool symmetrizable(const Profile& sumProfile) const;

  unsigned char getComplexity() const;

  unsigned char effectiveDepth() const;
};

#endif

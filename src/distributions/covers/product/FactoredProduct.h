/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_FACTOREDPRODUCT_H
#define SSS_FACTOREDPRODUCT_H

#include <string>

class Product;
class Profile;
enum Opponent: unsigned;

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

  bool explainable() const;

  Opponent simplestOpponent(const Profile& sumProfile) const;

  string strHeader() const;

  string strLine() const;

  // So far this only does the simplest case: One meaningful top.
  string strVerbal(
    const Profile& sumProfile,
    const Opponent simplestOpponent,
    const bool symmFlag) const;
};

#endif

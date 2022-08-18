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
class RanksNames;
enum Opponent: unsigned;
enum CompareType: unsigned;
enum CoverComposition: unsigned;
enum CoverVerbal: unsigned;

using namespace std;


class FactoredProduct
{
  private:

    Product const * canonicalPtr;

    // Number of unused tops that are elided.
    unsigned char canonicalShift;


  public:

    void set(
      Product const * ptr,
      const unsigned char shift);

    void set(
      const FactoredProduct& fp2,
      const unsigned char shift);

    unsigned long long code() const;

    bool includes(const Profile& distProfile) const;

    unsigned char getComplexity() const;

    unsigned char effectiveDepth() const;

    bool sameTops(const FactoredProduct& fp2) const;

    unsigned char minimumByTops(
      const Opponent voidSide,
      const Profile& sumProfile) const;

    bool symmetrizable(const Profile& sumProfile) const;

    CoverComposition composition() const;

    CoverVerbal verbal() const;

    bool lengthConsistent(const unsigned char specificLength) const;

    Opponent simplestOpponent(const Profile& sumProfile) const;
    Opponent simplestSingular(const Profile& sumProfile) const;

    CompareType presentOrder(const FactoredProduct& fp2) const;

    string strHeader(const unsigned length = 0) const;

    string strLine() const;

    string strVerbal(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const CoverVerbal verbal,
      const bool symmFlag) const;
};

#endif

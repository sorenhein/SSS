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

    bool lengthConsistent(const unsigned char specificLength) const;

    bool explainable() const;
    bool explainableNew() const;

    Opponent simplestOpponent(const Profile& sumProfile) const;

    CompareType presentOrder(const FactoredProduct& fp2) const;

    string strHeader(const unsigned length = 0) const;

    string strLine() const;

    // So far this only does the simplest case: One meaningful top.
    string strVerbal(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const Opponent simplestOpponent,
      const bool symmFlag) const;

    string strVerbalSingular(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const Opponent simplestOpponent,
      const bool symmFlag) const;
};

#endif

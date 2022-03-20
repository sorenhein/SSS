/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

/*
   A Product contains a set of Term's that all have to apply (they are
   multiplied together) in order for a holding to be contained.
   Like a Term, a Product does not contain information about the number
   of cards or tops that are available in a particular holding.
 */

#ifndef SSS_PRODUCT_H
#define SSS_PRODUCT_H

// TODO list instead?
#include <vector>
#include <string>

#include "Length.h"
#include "Top.h"

#include "../../utils/table.h"

using namespace std;

struct ProductProfile;


class Product
{
  private:

    Length length;

    vector<Top> tops;

    unsigned char complexity;

    unsigned char range;
    
    unsigned char topSize; // Last used top number + 1; may be 0
    unsigned char topCount; // Number of tops that are not unused


  public:

  Product();

  void reset();

  void resize(const unsigned compSize);

  void set(
    const ProductProfile& sumProfile,
    const ProductProfile& lowerProfile,
    const ProductProfile& upperProfile);

  bool includes(const ProductProfile& distProfile) const;

  unsigned char getComplexity() const;

  unsigned char getTopSize() const;

  unsigned char getRangeSum() const;

  bool explainable() const;

  string strHeader() const;

  string strLine(const ProductProfile& sumProfile) const;

  string strLine() const;

  string strVerbal(
    const ProductProfile& sumProfile,
    const Opponent simplestOpponent) const;
};

#endif

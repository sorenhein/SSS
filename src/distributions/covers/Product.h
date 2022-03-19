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

using namespace std;


struct ProductProfile
{
  unsigned char length;
  vector<unsigned char> tops;
};


class Product
{
  private:

    Length length;

    vector<Top> tops;

    bool symmFlag;

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

    // const unsigned char lenActual,
    // const unsigned char lenLow,
    // const unsigned char lenHigh,
    // const vector<unsigned char>& topsActual,
    // const vector<unsigned char>& topsLow,
    // const vector<unsigned char>& topsHigh);

  bool includes(
    const ProductProfile& distProfile) const;
    // const unsigned char lengthIn,
    // const vector<unsigned>& topsIn) const;

  unsigned char getComplexity() const;

  unsigned char getTopSize() const;

  unsigned char getRangeSum() const;

  bool explainable() const;

  string strHeader() const;

  string strLine(
    const ProductProfile& sumProfile) const;
    // const unsigned char lenActual,
    // const vector<unsigned char>& topsActual) const;

  string strLine() const;

  string strVerbal(
    const ProductProfile& sumProfile) const;
    // const unsigned char maxLength,
    // const vector<unsigned char>& topTotals) const;
};

#endif

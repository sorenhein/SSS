/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSET_H
#define SSS_COVERSET_H

#include <iostream>
#include <sstream>
#include <string>

#include "Length.h"
#include "Top.h"

#include "Product.h"

#include "../../utils/table.h"

using namespace std;

struct ProductProfile;


class CoverSet
{
  private:


    bool symmFlag;

    Length length;

    Top top1;

    Product product;


  public:

    void reset();

    void set(
      const ProductProfile& sumProfile,
      const ProductProfile& lowerProfile,
      const ProductProfile& upperProfile,
      const bool symmFlagIn = false);

    bool includes(
      const ProductProfile& distProfile,
      const ProductProfile& sumProfile) const;

    string str(const ProductProfile& sumProfile) const;
};

#endif

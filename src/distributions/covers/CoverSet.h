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


enum CoverMode
{
  COVER_MODE_NONE = 0,
  COVER_LENGTHS_ONLY = 1,
  COVER_TOPS_ONLY = 2,
  COVER_LENGTHS_AND_TOPS = 3
};


class CoverSet
{
  private:

    CoverMode mode;

    bool symmFlag;

    Length length;

    Top top1;

    Product product;


    bool includesLength(
      const ProductProfile& distProfile,
      const ProductProfile& sumProfile) const;

    bool includesTop1(
      const ProductProfile& distProfile,
      const ProductProfile& sumProfile) const;

    bool includesLengthAndTop1(
      const ProductProfile& distProfile,
      const ProductProfile& sumProfile) const;


  public:

    void reset();

    void setSymm(const bool symmFlagIn);

    void setMode(const CoverMode modeIn);

    CoverMode getMode() const;

    void set(
      const ProductProfile& sumProfile,
      const ProductProfile& lowerProfile,
      const ProductProfile& upperProfile);

    bool includes(
      const ProductProfile& distProfile,
      const ProductProfile& sumProfile) const;

    string str(const ProductProfile& sumProfile) const;
};

#endif

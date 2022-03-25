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

#include "Product.h"

using namespace std;

struct Profile;


class CoverSet
{
  private:


    bool symmFlag;

    Product product;


  public:

    void reset();

    void set(
      const Profile& sumProfile,
      const Profile& lowerProfile,
      const Profile& upperProfile,
      const bool symmFlagIn = false);

    bool includes(
      const Profile& distProfile,
      const Profile& sumProfile) const;

    string str(const Profile& sumProfile) const;
};

#endif

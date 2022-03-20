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


    bool includesLength(
      const unsigned char wlen,
      const unsigned char oppsLength) const;

    bool includesTop1(
      const unsigned char wtop,
      const unsigned char oppsTops1) const;

    bool includesLengthAndTop1(
      const unsigned char wtop,
      const unsigned char wlen,
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;


  public:

    void reset();

    void setSymm(const bool symmFlagIn);

    void setMode(const CoverMode modeIn);

    CoverMode getMode() const;

    void setLength(
      const unsigned char len,
      const unsigned char oppsLength);

    void setLength(
      const unsigned char len1,
      const unsigned char len2,
      const unsigned char oppsLength);

    void setTop1(
      const unsigned char tops,
      const unsigned char oppsSize);

    void setTop1(
      const unsigned char tops1,
      const unsigned char tops2,
      const unsigned char oppsSize);

    bool includes(
      const ProductProfile& distProfile,
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;

    string str(
      const unsigned char oppsLength,
      const unsigned char oppsTops1) const;
};

#endif

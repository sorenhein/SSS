/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSET_H
#define SSS_COVERSET_H

#include <string>

#include "CoverElement.h"

using namespace std;


enum CoverMode
{
  COVER_MODE_NONE = 0,
  COVER_LENGTHS_ONLY = 1,
  COVER_TOPS_ONLY = 2,
  COVER_LENGTHS_AND_TOPS = 3
};

struct CoverSet
{
  CoverMode mode;
  bool symmFlag;
  CoverElement length;
  CoverElement top1;

  void reset();

  // private
  bool includesLength(
    const unsigned char wlen,
    const unsigned char oppsLength) const;

  // private
  bool includesTop1(
    const unsigned char wtop,
    const unsigned char oppsTops1) const;

  // private
  bool includesLengthAndTop1(
    const unsigned char wtop,
    const unsigned char wlen,
    const unsigned char oppsLength,
    const unsigned char oppsTops1) const;

  // public
  bool includes(
    const unsigned char wtop,
    const unsigned char wlen,
    const unsigned char oppsLength,
    const unsigned char oppsTops1) const;

  string strLengthEqual(const unsigned char oppsLength) const;
};

#endif

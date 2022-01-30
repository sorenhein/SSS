/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSPEC_H
#define SSS_COVERSPEC_H

#include <array>
#include <string>

#include "CoverElement.h"


enum CoverMode
{
  COVER_MODE_NONE = 0,
  COVER_LENGTHS_ONLY = 1,
  COVER_TOPS_ONLY = 2,
  COVER_LENGTHS_AND_TOPS = 3
};

enum CoverState
{
  COVER_DONE = 0,
  COVER_OPEN = 1,
  COVER_IMPOSSIBLE = 2,
  COVER_STATE_SIZE = 3
};

struct CoverSpec
{
  // For easier identification.  Could perhaps be unsigned char
  unsigned index;

  unsigned char oppsLength;
  unsigned char oppsTops1;

  // There are two sets of elements (length, top1, and potentially more
  // tops in the future).  Each set has a mode.  The two sets are OR'ed
  // together if both are present.  Within a set, the elements are
  // AND'ed together if both are present.

  array<CoverMode, 2> mode;
  array<bool, 2> symmFlags;
  array<CoverElement, 2> westLength;
  array<CoverElement, 2> westTop1;


  CoverSpec();

  void getIndices(
    unsigned char& length,
    unsigned char& tops1) const;

  string strLengthEqual(
    const unsigned char wlen,
    const bool symmFlag) const;

  string strLengthInside(
    const unsigned char wlen1,
    const unsigned char wlen2,
    const bool symmFlag) const;

  string strLength(const unsigned specNumber) const;

  string strTop1Equal(
    const unsigned char wtop,
    const bool symmFlag) const;

  string strTop1Inside(
    const unsigned char wtop1,
    const unsigned char wtop2,
    const bool symmFlag) const;

  string strTop1(const unsigned specNumber) const;

  string strBothEqual(
    const unsigned char wlen,
    const unsigned char wtop,
    const bool symmFlag) const;

  string strTop1Fixed(const unsigned specNumber) const;

  string strSet(const unsigned specNumber) const;

  string str() const;
};

#endif

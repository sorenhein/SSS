/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSPEC_H
#define SSS_COVERSPEC_H

#include <array>
#include <string>

#include "CoverSet.h"


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

  array<CoverSet, 2> setsWest;


  CoverSpec();

  void reset();

  // private
  bool includes(
    const unsigned specNumber,
    const unsigned char wlen,
    const unsigned char wtop) const;
    
  // public
  bool includes(
    const unsigned char wlen,
    const unsigned char wtop) const;
    
  void getIndices(
    unsigned char& length,
    unsigned char& tops1) const;

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
    const unsigned specnumber,
    const unsigned char wlen,
    const unsigned char wtop,
    const bool symmFlag) const;

  string strTop1Fixed(const unsigned specNumber) const;

  string strSet(const unsigned specNumber) const;

  string str() const;
};

#endif

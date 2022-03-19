/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSPEC_H
#define SSS_COVERSPEC_H

#include <list>
#include <string>

#include "CoverSet.h"

struct ProductProfile;


enum CoverState
{
  COVER_DONE = 0,
  COVER_OPEN = 1,
  COVER_IMPOSSIBLE = 2,
  COVER_STATE_SIZE = 3
};

enum CoverControl
{
  COVER_ADD = 0,
  COVER_EXTEND = 1
};


class CoverSpec
{
  private:

    // For easier identification.  Could perhaps be unsigned char
    unsigned index;

    unsigned char oppsLength;
    unsigned char oppsTops1;

    // There are several sets of elements (two for now).  
    // Each set has a mode.  
    // The sets are OR'ed together if both are present.  
    // Within a set, the elements are AND'ed together if both are present.

    // Every time ctrl == COVER_EXTEND, we add one.
    list<CoverSet> setsWest;


    CoverSet& addOrExtend(const CoverControl ctrl);


  public:

    CoverSpec();

    void reset();

    void setID(
      const unsigned char length,
      const unsigned char tops1);

    void setIndex(const unsigned indexIn);

    void getID(
      unsigned char& length,
      unsigned char& tops1) const;

    unsigned getIndex() const;

    void westLength(
      const unsigned char len,
      const CoverControl ctrl = COVER_ADD);

    void eastLength(
      const unsigned char len,
      const CoverControl ctrl = COVER_ADD);

    void westLengthRange(
      const unsigned char len1,
      const unsigned char len2,
      const CoverControl ctrl = COVER_ADD);

    void eastLengthRange(
      const unsigned char len1,
      const unsigned char len2,
      const CoverControl ctrl = COVER_ADD);

    void westTop1(
      const unsigned char tops,
      const CoverControl ctrl = COVER_ADD);

    void eastTop1(
      const unsigned char tops,
      const CoverControl ctrl = COVER_ADD);

    void westTop1Range(
      const unsigned char tops1,
      const unsigned char tops2,
      const CoverControl ctrl = COVER_ADD);

    void eastTop1Range(
      const unsigned char tops1,
      const unsigned char tops2,
      const CoverControl ctrl = COVER_ADD);

    void westGeneral(
      const unsigned char len1,
      const unsigned char len2,
      const unsigned char tops1,
      const unsigned char tops2,
      const bool symmFlag = false,
      const CoverControl ctrl = COVER_ADD);

    void eastGeneral(
      const unsigned char len1,
      const unsigned char len2,
      const unsigned char tops1,
      const unsigned char tops2,
      const bool symmFlag = false,
      const CoverControl ctrl = COVER_ADD);

    bool includes(const ProductProfile& distProfile) const;
    
    string strRaw() const;

    string str() const;
};

#endif

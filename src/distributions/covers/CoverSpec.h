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


class CoverSpec
{
  private:

    // For easier identification.  Could perhaps be unsigned char
    unsigned index;

    unsigned char oppsLength;
    unsigned char oppsTops1;

    // There are two sets of elements.  Each set has a mode.  
    // The two sets are OR'ed together if both are present.  
    // Within a set, the elements are AND'ed together if both are present.

    array<CoverSet, 2> setsWest;


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
      const unsigned specNumber = 0);

    void eastLength(
      const unsigned char len,
      const unsigned specNumber = 0);

    void westLengthRange(
      const unsigned char len1,
      const unsigned char len2,
      const unsigned specNumber = 0);

    void eastLengthRange(
      const unsigned char len1,
      const unsigned char len2,
      const unsigned specNumber = 0);

    void westTop1(
      const unsigned char tops,
      const unsigned specNumber = 0);

    void eastTop1(
      const unsigned char tops,
      const unsigned specNumber = 0);

    void westTop1Range(
      const unsigned char tops1,
      const unsigned char tops2,
      const unsigned specNumber = 0);

    void eastTop1Range(
      const unsigned char tops1,
      const unsigned char tops2,
      const unsigned specNumber = 0);

    void westGeneral(
      const unsigned char len1,
      const unsigned char len2,
      const unsigned char tops1,
      const unsigned char tops2,
      const bool symmFlag = false,
      const unsigned specNumber = 0);

    void eastGeneral(
      const unsigned char len1,
      const unsigned char len2,
      const unsigned char tops1,
      const unsigned char tops2,
      const bool symmFlag = false,
      const unsigned specNumber = 0);

    bool includes(
      const unsigned char wlen,
      const unsigned char wtop) const;
    
    string strRaw() const;

    string str() const;
};

#endif

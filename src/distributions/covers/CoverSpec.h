/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSPEC_H
#define SSS_COVERSPEC_H

#include <list>
#include <string>

#include "CoverHelp.h"
#include "Product.h"
#include "Cover.h"

#include "Profile.h"
#include "Tricks.h"

class ProfilePair;
class ProductMemory;


enum CoverControl
{
  COVER_ADD = 0,
  COVER_EXTEND = 1
};


class CoverSpec
{
  friend class CoverRowOld;

  private:

    // For easier identification.  Could perhaps be unsigned char
    unsigned index;

    Profile sumProfile;

    // There are several sets of elements (two for now).  
    // Each set has a mode.  
    // The sets are OR'ed together if both are present.  
    // Within a set, the elements are AND'ed together if both are present.

    // Every time ctrl == COVER_EXTEND, we add one.
    list<Cover> setsWest;


    Cover& addOrExtend(const CoverControl ctrl);


  public:

    CoverSpec();

    void reset();

    void setID(
      const unsigned char numTops,
      const unsigned char length,
      const unsigned char tops1);

    void setIndex(const unsigned indexIn);

    void getID(
      unsigned char& length,
      unsigned char& tops1) const;

    unsigned getIndex() const;

    void westLengthRange(
      ProductMemory& productMemory,
      const unsigned char len1,
      const unsigned char len2,
      const CoverControl ctrl = COVER_ADD);

    void westTop1Range(
      ProductMemory& productMemory,
      const unsigned char tops1,
      const unsigned char tops2,
      const CoverControl ctrl = COVER_ADD);

    void westGeneral(
      ProductMemory& productMemory,
      const unsigned char len1,
      const unsigned char len2,
      const unsigned char tops1,
      const unsigned char tops2,
      const bool symmFlag = false,
      const CoverControl ctrl = COVER_ADD);
};

#endif

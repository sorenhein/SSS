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
#include "CoverNew.h"

#include "Profile.h"
#include "Tricks.h"

class ProfilePair;
class ProductMemory;


enum CoverControl
{
  COVER_ADD = 0,
  COVER_EXTEND = 1
};


    struct ProductPlus
    {
      Product product;

      Tricks tricks;

      unsigned weight;

      unsigned char numDist;

      bool symmFlag;


      void set(
        const Profile& sumProfile,
        const ProfilePair& profilePair,
        const bool symmFlagIn)
      {
        symmFlag = symmFlagIn;

        assert(sumProfile.size() == 2);

        product.resize(2);
        product.set(sumProfile, profilePair);
      };


      void prepare(
        const vector<Profile>& distProfiles,
        const vector<unsigned char>& cases)
      {
        tricks.prepare(product, distProfiles, cases, weight, numDist);
      };


      string strVerbal(const Profile& sumProfile) const
      {
        Opponent simplestOpponent = product.simplestOpponent(sumProfile);
        return product.strVerbal(sumProfile, simplestOpponent, symmFlag);
      };
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
    // list<ProductPlus> setsWest;
    list<CoverNew> setsWest;


    CoverNew& addOrExtend(const CoverControl ctrl);


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
      ProductMemory& productMemory,
      const unsigned char len,
      const CoverControl ctrl = COVER_ADD);

    void eastLength(
      ProductMemory& productMemory,
      const unsigned char len,
      const CoverControl ctrl = COVER_ADD);

    void westLengthRange(
      ProductMemory& productMemory,
      const unsigned char len1,
      const unsigned char len2,
      const CoverControl ctrl = COVER_ADD);

    void eastLengthRange(
      ProductMemory& productMemory,
      const unsigned char len1,
      const unsigned char len2,
      const CoverControl ctrl = COVER_ADD);

    void westTop1(
      ProductMemory& productMemory,
      const unsigned char tops,
      const CoverControl ctrl = COVER_ADD);

    void eastTop1(
      ProductMemory& productMemory,
      const unsigned char tops,
      const CoverControl ctrl = COVER_ADD);

    void westTop1Range(
      ProductMemory& productMemory,
      const unsigned char tops1,
      const unsigned char tops2,
      const CoverControl ctrl = COVER_ADD);

    void eastTop1Range(
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

    void eastGeneral(
      ProductMemory& productMemory,
      const unsigned char len1,
      const unsigned char len2,
      const unsigned char tops1,
      const unsigned char tops2,
      const bool symmFlag = false,
      const CoverControl ctrl = COVER_ADD);
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVER_H
#define SSS_COVER_H

#include <vector>
#include <string>

#include "Product.h"
#include "ProductMemory.h"
#include "Tricks.h"

#include "../../utils/table.h"

using namespace std;

class Profile;


class Cover
{
  private:

    ProductUnit * productUnitPtr;

    Tricks tricks;

    unsigned weight;

    unsigned char numDist;

    bool symmFlag;

    unsigned long long code;


  public:

    Cover();

    void reset();

    void set(
      ProductMemory& productMemory,
      const Profile& sumProfile,
      const ProfilePair& profilePair,
      const bool symmFlagIn);

    void prepare(
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    bool includes(const Profile& distProfile) const;

    bool possible(
      const Tricks& explained,
      const Tricks& residuals,
      Tricks& additions,
      unsigned char& tricksAdded) const;

    CoverState explain(Tricks& tricksSeen) const;

    bool operator < (const Cover& cover2) const;

    bool sameWeight(const Cover& covers2) const;

    bool sameTricks(const Cover& covers2) const;

    bool empty() const;

    bool full() const;

    bool symmetric() const;

    unsigned getWeight() const;

    unsigned size() const;

    unsigned char getNumDist() const;

    unsigned char getTopSize() const;

    unsigned char getComplexity() const;

    string strHeader() const;

    string strLine(const Profile& sumProfile) const;

    string strLine() const;

    string strProfile() const;

    string strHeaderTricksShort() const;

    string strTricksShort() const;

    string str(const Profile& sumProfile) const;
};

#endif

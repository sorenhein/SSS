/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERNEW_H
#define SSS_COVERNEW_H

#include <vector>
#include <string>

#include "Product.h"
#include "ProductMemory.h"
#include "CoverSpec.h"
#include "Tricks.h"

#include "../../utils/table.h"

using namespace std;

class Profile;


class CoverNew
{
  private:

    ProductUnit * productUnitPtr;

    Tricks tricks;

    // Could we get weight and numDist into Product?
    unsigned weight;

    unsigned char numDist;

    Opponent simplestOpponent;
    

  public:

    CoverNew();

    void reset();

    void set(
      ProductMemory& productMemory,
      const Profile& sumProfile,
      const ProfilePair& profilePair);

    void prepare(
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    bool possible(
      const Tricks& explained,
      const Tricks& residuals,
      Tricks& additions,
      unsigned char& tricksAdded) const;

    CoverState explain(Tricks& tricksSeen) const;

    bool earlier(const CoverNew& cover2) const;

    bool sameWeight(const CoverNew& covers2) const;

    bool sameTricks(const CoverNew& covers2) const;

    bool empty() const;

    bool full() const;

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

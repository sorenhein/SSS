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
#include "CoverSpec.h"

#include "../../utils/table.h"

using namespace std;

class Profile;


class CoverNew
{
  private:

    Product product;

    vector<unsigned char> profile;

    // Could we get weight and numDist into Product?
    unsigned weight;

    unsigned char numDist;

    Opponent simplestOpponent;
    

  public:

    CoverNew();

    void reset();

    void resize(const unsigned topNumber);

    /*
    void set(
      const Profile& sumProfile,
      const Profile& lowerProfile,
      const Profile& upperProfile);
      */

    void set(
      const Profile& sumProfile,
      const ProfilePair& profilePair);

    void prepare(
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    bool possible(
      const vector<unsigned char>& explained,
      const vector<unsigned char>& residuals,
      vector<unsigned char>& additions,
      unsigned char& tricksAdded) const;

    CoverState explain(vector<unsigned char>& tricks) const;

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

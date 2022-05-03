/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVER_H
#define SSS_COVER_H

#include <vector>
#include <string>

#include "Tricks.h"

#include "product/Product.h"
#include "product/ProductMemory.h"

#include "../../utils/table.h"

using namespace std;

class Profile;
class ProductStats;


class Cover
{
  private:

    FactoredProduct const * factoredProductPtr;

    Tricks tricks;

    // Micro-complexity per weight unit.
    unsigned mcpw;

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

    void setExisting(
      const ProductMemory& productMemory,
      const Profile& sumProfile,
      const ProfilePair& profilePair,
      const bool symmFlagIn);

    bool prepare(
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    bool symmetrizable(const Profile& sumProfile) const;

    // Will invalidate Cover if not symmetrizable!
    bool symmetrize();

    void tricksOr(
      Tricks& running,
      const vector<unsigned char>& cases) const;

    bool possible(
      const Tricks& explained,
      const Tricks& residuals,
      const vector<unsigned char>& cases,
      Tricks& additions) const;

    bool possible(const Tricks& residuals) const;

    void updateStats(
      ProductStats& productStats,
      const Profile& sumProfile,
      const bool newTableauFlag) const;

    bool sameWeight(const Cover& covers2) const;

    bool sameTricks(const Cover& covers2) const;

    bool symmetric() const;

    unsigned size() const;

    bool operator < (const Cover& cover2) const;

    const Tricks& getTricks() const;

    unsigned char effectiveDepth() const;

    unsigned getWeight() const;

    unsigned char getComplexity() const;

    unsigned char minComplexityAdder(const unsigned resWeight) const;

    string strHeader() const;

    string strLine() const;

    string strProfile() const;

    string strHeaderTricksShort() const;

    string strTricksShort() const;

    string str(const Profile& sumProfile) const;
};

#endif

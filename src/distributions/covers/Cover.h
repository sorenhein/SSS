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

using namespace std;

class ProfilePair;
class Profile;
class RanksNames;
class ProductStats;
class ProductMemory;
struct CoverLength;

enum Opponent: unsigned;
enum CoverSymmetry: unsigned;
enum CoverComposition: unsigned;
enum CoverVerbal: unsigned;

class Cover
{
  private:

    FactoredProduct const * factoredProductPtr;

    Tricks tricks;

    // Micro-complexity per weight unit.
    unsigned mcpw;

    // Set if the cover is symmetric without being symmetrized.
    CoverSymmetry coverSymmetry;

    // Set if the factored product is effectively to be mirrored.
    bool symmetrizeFlag;

    unsigned long long code;


    bool singular() const;


  public:

    Cover();

    void reset();

    void set(
      ProductMemory& productMemory,
      const Profile& sumProfile,
      const ProfilePair& profilePair,
      const bool symmetricFlag,
      const bool symmetrizeFlagIn);

    void setExisting(
      const ProductMemory& productMemory,
      const Profile& sumProfile,
      const ProfilePair& profilePair,
      const bool symmetrizeFlagIn);

    bool setByProduct(
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    void tricksOr(
      Tricks& running,
      const vector<unsigned char>& cases) const;

    bool possible(
      const Tricks& explained,
      const Tricks& residuals,
      const vector<unsigned char>& cases,
      Tricks& additions,
      unsigned& rawWeightAdder) const;

    void updateStats(
      ProductStats& productStats,
      const Profile& sumProfile,
      const bool newTableauFlag) const;

    bool sameWeight(const Cover& covers2) const;

    bool sameTricks(const Cover& covers2) const;

    bool sameTops(const Cover& covers2) const;

    unsigned char minimumByTops(
      const Opponent voidSide,
      const Profile& sumProfile) const;

    bool symmetrizable(const Profile& sumProfile) const;

    // Will invalidate Cover if not symmetrizable!
    bool symmetrize();

    bool symmetrized() const;

    bool symmetric() const;

    bool antiSymmetric() const;

    CoverSymmetry symmetry() const;

    CoverComposition composition() const;

    bool lengthConsistent(const CoverLength& specificLength) const;

    bool explainable(const Profile& sumProfile) const;

    size_t size() const;

    bool operator < (const Cover& cover2) const;

    const Tricks& getTricks() const;

    unsigned char effectiveDepth() const;

    unsigned getWeight() const;

    unsigned char getComplexity() const;

    unsigned char minComplexityAdder(const unsigned resWeight) const;

    CoverVerbal verbal(const Profile& sumProfile) const;

    string strHeader() const;

    string strLine() const;

    string strProfile() const;

    string strHeaderTricksShort() const;

    string strTricksShort() const;

    string str(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const CoverVerbal verbal) const;

    string strNumerical() const;

// TODO
string ID() const {return "CC";};
};

#endif

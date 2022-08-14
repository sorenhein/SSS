/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERROW_H
#define SSS_COVERROW_H

#include <list>
#include <string>

#include "Tricks.h"
#include "CoverCategory.h"

using namespace std;

class Cover;
class Profile;
class RanksNames;
class ProductStats;
struct CoverLength;

enum Opponent: unsigned;
enum CoverSymmetry: unsigned;


class CoverRow
{
  private:

    // A row is a list of covers that are OR'ed together.
    list<Cover const *> coverPtrs;

    // The OR'ed tricks of the covers.
    Tricks tricks;

    // The net weight of the strategy covered.
    unsigned weight;

    // The raw weight of all individual cover's (may overlap).
    unsigned rawWeight;

    unsigned char complexity;

    CoverVerbal verbal;


    string strEnum() const;


  public:

    CoverRow();

    void reset();

    void resize(const size_t len);

    void fillDirectly(
      list<Cover const *>& coverPtrsIn,
      const vector<unsigned char>& cases);

    bool possibleAdd(
      const Cover& cover,
      const Tricks& residuals,
      const vector<unsigned char>& cases,
      Tricks& additions,
      unsigned& rawWeightAdder) const;

    bool possible(const Tricks& residuals) const;

    void add(
      const Cover& cover,
      const Tricks& additions,
      const unsigned rawWeightAdder,
      Tricks& residuals,
      const CoverVerbal verbalIn);

    void add(
      const Cover& cover,
      Tricks& residuals,
      const CoverVerbal verbalIn);

    void setVerbal(const Profile& profile);

    void updateStats(
      ProductStats& productStats,
      const Profile& sumProfile,
      const bool newTableauFlag) const;

    size_t size() const;

    bool symmetric() const;

    CoverSymmetry symmetry() const;

    bool symmetrizable(const Profile& sumProfile) const;

    bool operator < (const CoverRow& row2) const;

    bool lowerVerbal(const CoverRow& row2) const;

    const Tricks& getTricks() const;

    bool sameTops(const CoverRow& row2) const;

    unsigned char minimumByTops(
      const Opponent voidSide,
      const Profile& sumProfile) const;

    unsigned char effectiveDepth() const;

    bool lengthConsistent(const CoverLength& specificLength) const;

    unsigned getWeight() const;

    unsigned getRawWeight() const;

    unsigned char getComplexity() const;

    unsigned char minComplexityAdder(const unsigned resWeight) const;

    string strHeader() const;

    string str(
      const Profile& sumProfile,
      const RanksNames& ranksNames) const;

    string strNumerical() const;

// TODO
string ID() const {return "RR";};
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TRICKS_H
#define SSS_TRICKS_H

#include <vector>
#include <list>
#include <string>

using namespace std;

class FactoredProduct;
class Profile;
class Result;

enum CoverSymmetry: unsigned;


class Tricks
{
  private:

    size_t length;

    unsigned weight;

    // The tricks are grouped as shown in TrickConvert.  This makes
    // comparisons etc. faster, but costs some time to set up.
    vector<unsigned> signature;

    // Useful pre-calculation.
    size_t lastForward;


    const unsigned char lookup(const size_t extIndex) const;

    void weigh(const vector<unsigned char>& cases);

    unsigned char productValue(
      const FactoredProduct& factoredProduct,
      const bool symmetrizeFlag,
      const vector<Profile>& distProfiles,
      const size_t extIndex) const;

    unsigned casesToLengthEW(const vector<unsigned char>& cases) const;

    void lengthBoundary(
      size_t& cindex,
      const unsigned target,
      const vector<unsigned char>& cases) const;

    void transfer(
      const Tricks& tricks,
      const vector<unsigned char>& cases,
      const size_t cstart,
      const size_t cend,
      Tricks& tricksMin);

    bool antiSymmetric() const;

  
  public:

    void clear();

    void resize(const size_t len);

    void setByResults(
      const list<Result>& results,
      const vector<unsigned char>& cases,
      unsigned char& tricksMin);

    bool setByProduct(
      const FactoredProduct& factoredProduct,
      const bool symmetrizeFlag,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    // Will invalidate Tricks if not symmetrizable!
    bool symmetrize();

    unsigned factor();

    void uniqueOver(
      const Tricks& compare,
      const vector<unsigned char>& cases);

    bool possible(
      const Tricks& explained,
      const Tricks& residuals,
      const vector<unsigned char>& cases,
      Tricks& additions) const;

    Tricks& orNormal(
      const Tricks& tricks2,
      const vector<unsigned char>& cases);

    void orSymm(
      const Tricks& tricks2,
      const vector<unsigned char>& cases);

    bool symmetric() const;

    bool symmetricWith(const Tricks& tricks2) const;

    CoverSymmetry symmetry() const;

    void partition(
      Tricks& tricksSymmetric,
      Tricks& tricksAntisymmetric,
      const vector<unsigned char>& cases) const;

    void partitionGeneral(
      vector<Tricks>& tricksByLength,
      vector<Tricks>& tricksMinByLength,
      const vector<unsigned char>& cases) const;

    Tricks& operator += (const Tricks& tricks2);

    Tricks& operator -= (const Tricks& tricks2);

    bool operator == (const Tricks& tricks2) const;

    bool operator <= (const Tricks& tricks2) const;

    size_t size() const;

    unsigned getWeight() const;

    string strList() const;

    string strShort() const;

    string strSpaced() const;
};

#endif

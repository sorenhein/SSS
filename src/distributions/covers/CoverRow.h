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

using namespace std;

class Cover;
class Profile;
class ProductStats;



class CoverRow
{
  private:

    // A row is a list of covers that are OR'ed together.
    list<Cover const *> coverPtrs;

    // The OR'ed tricks of the covers.
    Tricks tricks;

    unsigned weight;

    unsigned char complexity;


  public:

    CoverRow();

    void reset();

    void resize(const unsigned len);

    void fillDirectly(
      list<Cover const *>& coverPtrsIn,
      const vector<unsigned char>& cases);

    bool possibleAdd(
      const Cover& cover,
      const Tricks& residuals,
      const vector<unsigned char>& cases,
      Tricks& additions) const;

    bool possible(
      const Tricks& residuals,
      Tricks& additions) const;

    void add(
      const Cover& cover,
      const Tricks& additions,
      Tricks& residuals);

    void updateStats(
      ProductStats& productStats,
      const Profile& sumProfile,
      const bool newTableauFlag) const;

    unsigned size() const;

    bool operator < (const CoverRow& rows2) const;

    bool operator <= (const Tricks& residuals) const;

    unsigned char effectiveDepth() const;

    unsigned getWeight() const;

    unsigned char getComplexity() const;

    unsigned char minComplexityAdder(const unsigned resWeight) const;

    string strHeader() const;

    string str(const Profile& sumProfile) const;
};

#endif

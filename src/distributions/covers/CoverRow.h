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

/* A row is a list of covers that are OR'ed together.
 */


class CoverRow
{
  private:

    list<Cover const *> coverPtrs;

    // The OR'ed tricks of the covers.
    Tricks tricks;

    unsigned weight;

    unsigned char numDist;

    unsigned char complexity;


  public:

    CoverRow();

    void reset();

    void resize(const unsigned len);

    void fillDirectly(
      list<Cover const *>& coverPtrsIn,
      const vector<unsigned char>& cases);

    bool attempt(
      const Cover& cover,
      const Tricks& residuals,
      Tricks& additions,
      unsigned char& tricksAdded) const;

    void add(
      const Cover& cover,
      const Tricks& additions,
      Tricks& residuals,
      unsigned char& residualsSum);

    void subtract(
      const Tricks& additions,
      Tricks& residuals,
      unsigned char& residualsSum) const;

    bool possible(
      const Tricks& explained,
      const Tricks& residuals,
      Tricks& additions,
      unsigned char& tricksAdded) const;

    CoverState explain(Tricks& tricksSeen) const;

    void updateStats(
      ProductStats& productStats,
      const Profile& sumProfile,
      const bool newTableauFlag) const;

    bool operator <= (const CoverRow& coverRow2) const;

    unsigned size() const;

    const Tricks& getTricks() const;

    unsigned getWeight() const;

    unsigned char getNumDist() const;

    unsigned char getComplexity() const;

    unsigned char getOverlap() const;

    string strInternal() const;

    string strHeader() const;

    string str(const Profile& sumProfile) const;
};

#endif

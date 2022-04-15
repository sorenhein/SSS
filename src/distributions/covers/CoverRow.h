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


    // TODO I actually think we don't need these.  Once we have
    // the manual data, then for each such we should make a temporar
    // cover, set(productMemory, sumProfile, profilePair, symmFlag).
    // Then we prepare(distProfiles, cases).
    // Then we look it up in the comprehensive list, which yields
    // a pointer.
    // Then we CoverRow::add it, which must consider symmetryFlag.
    bool includes(
      const Profile& distProfile,
      const Profile& sumProfile) const;

    void score(
      const vector<Profile>& distProfiles,
      const Profile& sumProfile,
      const vector<unsigned char>& cases);


  public:

    CoverRow();

    void reset();

    void resize(const unsigned len);

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

    CoverState explain(Tricks& tricksSeen) const;

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

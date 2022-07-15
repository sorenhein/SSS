/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_ROWMATCHES_H
#define SSS_ROWMATCHES_H

#include <string>

#include "RowMatch.h"
#include "PartialVoid.h"

class Covers;
class Cover;
class CoverTableau;
class CoverStore;
class Tricks;
class Explain;


using namespace std;

class Profile;
enum Opponent: unsigned;


struct VoidInfo
{
  Cover const * coverPtr;

  Tricks * tricksPtr;

  unsigned westLength;

  unsigned repeats;
};


class RowMatches
{
  private:

    list<RowMatch> matches;

    // PartialVoid voidWest;
    // PartialVoid voidEast;
    VoidInfo voidWest;
    VoidInfo voidEast;


  public:
    
    void emplace(
      CoverRow& rowIn,
      const size_t westLength);

    void transfer(
      CoverRow& rowIn,
      const size_t westLength,
      const Opponent towardVoid,
      const unsigned repeats = 1);

    void setVoid(
      const Opponent side,
      const VoidInfo& voidInfo,
      const Profile& sumProfile);

    void incorporateLengths(
      const CoverStore& coverStore,
      const vector<unsigned char>& cases,
      vector<Tricks>& tricksOfLength,
      Explain& explain);

    bool incorporateTops(
      Covers& covers,
      const vector<Tricks>& tricksWithinLength,
      Explain& explain);

    void incorporateVoids(const Profile& sumProfile);

    void symmetrize(const Profile& sumProfile);

    void makeSolution(
      const CoverStore& coverStore,
      const vector<unsigned char>& cases,
      Explain& explain,
      CoverTableau& solution);

    list<RowMatch>::const_iterator begin() const
      { return matches.begin(); };
    list<RowMatch>::const_iterator end() const
      { return matches.end(); };

    string str() const;
};

#endif

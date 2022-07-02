/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_ROWMATCHES_H
#define SSS_ROWMATCHES_H

#include <string>

#include "RowMatch.h"

class Cover;
class Tricks;


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


  public:
    
    void transfer(
      CoverRow& rowIn,
      const size_t westLength,
      const Opponent towardVoid,
      const unsigned repeats = 1);

    void setVoid(
      const Opponent side,
      const VoidInfo& voidInfo,
      const Profile& sumProfile);

    list<RowMatch>::const_iterator begin() const
      { return matches.begin(); };
    list<RowMatch>::const_iterator end() const
      { return matches.end(); };

    string str() const;
};

#endif

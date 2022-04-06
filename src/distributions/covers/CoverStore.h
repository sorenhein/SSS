/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSTORE_H
#define SSS_COVERSTORE_H

#include <string>
#include <map>

#include "Cover.h"

using namespace std;

class ProductMemory;
class Profile;
class ProfilePair;


// Unlike a Memory, this store is more local and specific to one
// distribution.  Hence the name is also different.

class CoverStore
{
  private:

    map<Cover, unsigned> store; // Value is irrelevant


  public:

    CoverStore();

    void reset();

    void add(
      ProductMemory& productMemory,
      const Profile& sumProfile,
      const ProfilePair& productPair,
      const bool symmFlag,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    const Cover& lookup(
      ProductMemory& productMemory,
      const Profile& sumProfile,
      const ProfilePair& productPair,
      const bool symmFlag) const;

    map<Cover, unsigned>::const_iterator begin() const;

    map<Cover, unsigned>::const_iterator end() const;

};

#endif

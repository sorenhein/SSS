/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSTORE_H
#define SSS_COVERSTORE_H

#include <set>
#include <list>
#include <vector>
#include <string>

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

    set<Cover> store;

    // To avoid creating a temporary one every time.
    Cover coverScratch;

    // Cache the symmetric ones until we have added all the
    // regular ones.
    list<Cover> symmetricCache;


    void eliminate(set<Cover>::iterator& itMatch);


  public:

    CoverStore();

    void reset();

    void add(
      ProductMemory& productMemory,
      const Profile& sumProfile,
      const ProfilePair& productPair,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    void admixSymmetric();

    const Cover& lookup(const Cover& cover) const;

    const Cover& lookup(
      const ProductMemory& productMemory,
      const Profile& sumProfile,
      const ProfilePair& productPair,
      const bool symmFlag); // Effectively const

    set<Cover>::const_iterator begin() const;

    set<Cover>::const_iterator end() const;

    unsigned size() const;

    string str() const;
};

#endif

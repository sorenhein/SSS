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

class Tricks;
class Explain;
class Partial;


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
      const ProfilePair& profilePair,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    void admixSymmetric();

    const Cover& lookup(const Cover& cover) const;

    void heaviestPartial(
      const Tricks& tricks,
      const vector<unsigned char>& cases,
      const Explain& explain,
      Partial& partial) const;

    set<Cover>::const_iterator begin() const;

    set<Cover>::const_iterator end() const;

    size_t size() const;

    string str() const;
};

#endif

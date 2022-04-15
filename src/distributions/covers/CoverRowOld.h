/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERROWOLD_H
#define SSS_COVERROWOLD_H

#include <vector>
#include <string>

#include "Tricks.h"
#include "Cover.h"
#include "Profile.h"
#include "CoverHelp.h"


using namespace std;

class ProductMemory;
struct ManualData;


class CoverRowOld
{
  private:

    list<Cover> covers;

    Tricks tricks;

    unsigned indexInternal;

    Profile sumProfile;


    unsigned weight;
    unsigned char numDist;


  public:

    CoverRowOld();

    void reset();

    void resize(const unsigned len);

    void prepareNew(
      const ProductMemory& productMemory,
      const list<ManualData>& manualList,
      const Profile& sumProfileIn,
      const unsigned indexIn,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    void add(
      const Cover& cover,
      const bool symmFlagIn,
      const Profile& sumProfileIn,
      const unsigned indexIn);

    void weigh(const vector<unsigned char>& cases);

    bool includes(const Profile& distProfile) const;

    CoverState explain(Tricks& tricksSeen) const;

    bool operator <= (const CoverRowOld& cover2) const;

    unsigned index() const;

    unsigned getWeight() const;

    unsigned char getNumDist() const;

    string str() const;

    string strProfile() const;
};

#endif

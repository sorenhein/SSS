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
#include "CoverSpec.h"
#include "CoverNew.h"
#include "CoverHelp.h"


using namespace std;

class Profile;


class CoverRowOld
{
  private:

    Tricks tricks;

    unsigned indexInternal;
    Profile sumProfile;
    // list<ProductPlus> setsWest;
    list<CoverNew> setsWest;

    unsigned weight;
    unsigned char numDist;


  public:

    CoverRowOld();

    void reset();

    void prepare(
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases,
      const CoverSpec& specIn);

    bool includes(const Profile& distProfile) const;

    CoverState explain(Tricks& tricksSeen) const;

    bool operator <= (const CoverRowOld& cover2) const;

    void getID(
      unsigned char& length,
      unsigned char& tops1) const;

    unsigned index() const;

    unsigned getWeight() const;

    unsigned char getNumDist() const;

    string str() const;

    string strProfile() const;
};

#endif

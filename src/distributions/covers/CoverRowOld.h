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
#include "Cover.h"
#include "CoverHelp.h"


using namespace std;

class Profile;
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

    void prepare(
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases,
      const CoverSpec& specIn);

    void prepareMedium(
      const list<Cover>& coverList,
      const Profile& sumProfileIn,
      const unsigned indexIn,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

    void prepareNew(
      ProductMemory& productMemory,
      const list<ManualData>& manualList,
      const Profile& sumProfileIn,
      const unsigned indexIn,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);

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

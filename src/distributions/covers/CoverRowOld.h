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

    // Profile sumProfile;

    unsigned weight;

    unsigned char numDist;

    unsigned char complexity;


  public:

    CoverRowOld();

    void reset();

    void resize(const unsigned len);

    void add(
      const Cover& cover,
      // const Profile& sumProfileIn,
      const unsigned indexIn);

    void weigh(const vector<unsigned char>& cases);

    CoverState explain(Tricks& tricksSeen) const;

    bool operator <= (const CoverRowOld& cover2) const;

    unsigned size() const;

    unsigned index() const;

    const Tricks& getTricks() const;

    unsigned getWeight() const;

    unsigned getComplexity() const;

    unsigned char getNumDist() const;

    unsigned char getOverlap() const;

    string strInternal() const;

    string strHeader() const;

    string str(const Profile& sumProfile) const;

    // string strProfile() const;
};

#endif

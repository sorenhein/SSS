/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERNEW_H
#define SSS_COVERNEW_H

#include <vector>
#include <string>

#include "CoverSetNew.h"
#include "CoverSpec.h"


using namespace std;


class CoverNew
{
  private:

    CoverSetNew coverSet;

    vector<unsigned char> profile;

    unsigned weight;

    unsigned char numDist;
    

  public:

    CoverNew();

    void reset();

    void prepare(
      const vector<unsigned char>& lengths,
      const vector<vector<unsigned char> const *>& topPtrs,
      const vector<unsigned char>& cases);

    CoverState explain(vector<unsigned char>& tricks) const;

    bool operator <= (const CoverNew& cover2) const;

    unsigned getWeight() const;

    unsigned char getNumDist() const;

    string str(const unsigned char lengthActual) const;

    string strProfile() const;
};

#endif

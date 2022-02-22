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

    void resize(const unsigned topNumber);

    void set(
      const unsigned char lenActual,
      const unsigned char lenLow,
      const unsigned char lenHigh,
      const vector<unsigned char>& topsActual,
      const vector<unsigned char>& topsLow,
      const vector<unsigned char>& topsHigh);

    void prepare(
      const vector<unsigned char>& lengths,
      vector<vector<unsigned > const *>& topPtrs,
      const vector<unsigned char>& cases);

    CoverState explain(vector<unsigned char>& tricks) const;

    // bool operator <= (const CoverNew& cover2) const;
    bool earlier(const CoverNew& cover2) const;

    bool sameParameters(const CoverNew& covers2) const;

    bool sameTricks(const CoverNew& covers2) const;

    bool empty() const;

    bool full() const;

    unsigned getWeight() const;

    unsigned char getNumDist() const;

    string strHeader() const;

    string strLine(
      const unsigned char lengthActual,
      const vector<unsigned char>& topsActual) const;

    string strProfile() const;
};

#endif

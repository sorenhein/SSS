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

    // Could we get weight and numDist into CoverSetNew?
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

    bool possible(
      const vector<unsigned char>& explained,
      const vector<unsigned char>& residuals,
      vector<unsigned char>& additions,
      unsigned char& tricksAdded) const;

    CoverState explain(vector<unsigned char>& tricks) const;

    // bool operator <= (const CoverNew& cover2) const;
    bool earlier(const CoverNew& cover2) const;

    bool sameWeight(const CoverNew& covers2) const;

    bool sameTricks(const CoverNew& covers2) const;

    bool empty() const;

    bool full() const;

    unsigned getWeight() const;

    unsigned char getNumDist() const;

    unsigned char getTopSize() const;

    unsigned char getComplexity() const;

    string strHeader() const;

    string strLine(
      const unsigned char lengthActual,
      const vector<unsigned char>& topsActual) const;

    string strProfile() const;

    string strHeaderTricksShort() const;

    string strTricksShort() const;
};

#endif

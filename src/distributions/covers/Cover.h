/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVER_H
#define SSS_COVER_H

#include <vector>
#include <string>

#include "CoverSpec.h"


using namespace std;


class Cover
{
  private:

    vector<unsigned char> profile;

    CoverSpec spec;

    unsigned weight;
    unsigned char numDist;


    typedef unsigned char (Cover::*CoverComparePtr)(
      const unsigned char value,
      const unsigned char ref1,
      const unsigned char ref2) const;

    bool includes(
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const unsigned dno,
      const unsigned specNumber);


  public:

    Cover();

    void reset();

    void prepare(
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const vector<unsigned char>& cases,
      const CoverSpec& specIn);

    CoverState explain(vector<unsigned char>& tricks) const;

    bool operator <= (const Cover& cover2) const;

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

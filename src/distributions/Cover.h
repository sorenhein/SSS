/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVER_H
#define SSS_COVER_H

#include <vector>
#include <string>

#include "CoverHelp.h"


using namespace std;


class Cover
{
  private:

    vector<unsigned char> profile;

    CoverSpec spec;

    unsigned char weight;


    typedef unsigned char (Cover::*CoverComparePtr)(
      const unsigned char value,
      const unsigned char ref1,
      const unsigned char ref2) const;


    string strLength() const;

    string strTop() const;


  public:

    Cover();

    void reset();

    void prepare(
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const vector<unsigned char>& cases,
      const CoverSpec& specIn);

    CoverState explain(vector<unsigned char>& tricks) const;

    unsigned char getWeight() const;

    string str() const;

    string strProfile() const;
};

#endif

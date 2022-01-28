/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RESEXPL_H
#define SSS_RESEXPL_H

#include <list>
#include <string>

#include "Cover.h"


using namespace std;


struct ExplData
{
  Cover const * coverPtr;
  unsigned char weight;
  unsigned char numDist;
  unsigned char level; // Explanations may be nested
};


class ResExpl
{
  private:

    unsigned char tricksMin;

    list<ExplData> data;


    list<ExplData>::iterator dominator(const Cover& cover);


  public:

    ResExpl();

    void reset();

    void setMinimum(const unsigned char tmin);

    void insert(const Cover& cover);

    string str() const;
};

#endif

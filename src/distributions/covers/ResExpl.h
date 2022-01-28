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


    void append(
      Cover const * cover,
      const unsigned char weight,
      const unsigned char numDist,
      const unsigned char level);


  public:

    ResExpl();

    void reset();

    void add(
      const Cover& cover,
      const unsigned char weight,
      const unsigned char numDist,
      const unsigned char level);

    string str() const;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RESEXPL_H
#define SSS_RESEXPL_H

#include <list>
#include <string>

#include "CoverRow.h"

class ExplStats;
class Profile;


using namespace std;


struct ExplData
{
  CoverRow const * coverRowPtr;
  unsigned weight;
  unsigned char numDist;
  unsigned char level; // Explanations may be nested
};


class ResExpl
{
  private:

    unsigned char tricksMin;
    unsigned char maxLength;
    unsigned char maxTops;

    list<ExplData> data;


    list<ExplData>::iterator dominator(const CoverRow& coverRow);


  public:

    ResExpl();

    void reset();

    void setParameters(
      const unsigned char tmin,
      const unsigned char maxLengthIn,
      const unsigned char maxTopsIn);

    void insert(const CoverRow& coverRow);

    bool empty() const;

    void updateStats(ExplStats& explStats) const;

    string str(const Profile& sumProfile) const;
};

#endif

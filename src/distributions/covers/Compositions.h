
/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMPOSITIONS_H
#define SSS_COMPOSITIONS_H

#include <vector>
#include <list>
#include <string>

#include "Profile.h"

using namespace std;


class Compositions
{
  private:

    vector<list<Profile>> compositions;


    void makeLength(const unsigned char length);


  public:

    Compositions();

    void reset();

    void make(const unsigned char maxLength = 13);

    list<Profile>::const_iterator begin(const unsigned char len) const;
    list<Profile>::const_iterator end(const unsigned char len) const;

    string str(const unsigned char lenSpecific = 0) const;
};

#endif

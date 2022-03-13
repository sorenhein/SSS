/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TABLEAUCACHE_H
#define SSS_TABLEAUCACHE_H

#include <iostream>
#include <iomanip>
#include <sstream>

#include <vector>
#include <list>
#include <string>

#include "CoverTableau.h"


using namespace std;


class TableauCache
{
  private:

    struct CacheEntry
    {
      vector<unsigned char> excessTricks;
      CoverTableau tableau;

      string str() const
      {
        stringstream ss;

        for (auto t: excessTricks)
          ss << +t << " ";
        ss << "\n\n";

        ss << tableau.str();

        return ss.str() + "\n";
      };
    };

    list<CacheEntry> entries;


  public:

    TableauCache();

    void reset();

    void store(
      const vector<unsigned char>& excessTricksIn,
      const CoverTableau& tableau);

    bool lookup(
      const vector<unsigned char>& excessTricksIn,
      CoverTableau const *& tableauPtr) const;

    unsigned size() const;
      
    string str() const;
};

#endif

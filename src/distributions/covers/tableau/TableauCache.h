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

#include "../Tricks.h"

#include "../product/Profile.h"

class RanksNames;


using namespace std;


class TableauCache
{
  private:

    struct CacheEntry
    {
      Tricks excessTricks;
      CoverTableau tableau;
      unsigned count;

      string str(
        const Profile& sumProfile,
        const RanksNames& ranksNames) const
      {
        stringstream ss;

        ss << "Count " << count << ", vector ";
        ss << excessTricks.strSpaced() << "\n";
        ss << tableau.str(sumProfile, ranksNames);

        return ss.str();
      };
    };

    list<CacheEntry> entries;


  public:

    TableauCache();

    void reset();

    void store(
      const Tricks& excessTricks,
      const CoverTableau& tableau);

    bool lookup(
      const Tricks& excessTricks,
      CoverTableau& solution);

    size_t size() const;

    void getCounts(
      size_t& numTableaux,
      size_t& numUses) const;
      
    string str(
      const Profile& sumProfile,
      const RanksNames& ranksNames) const;
};

#endif

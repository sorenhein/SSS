/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERMEMORY_H
#define SSS_COVERMEMORY_H

#include <vector>
#include <list>

using namespace std;

class Profile;
class ExplStats;
class ProductMemory;


class CoverMemory
{
  private:

    vector<vector<unsigned>> counts;


  public:

    void resize(const unsigned char maxCards);

    void resizeStats(ExplStats& explStats) const;

    void prepareRows(
      Covers& covers,
      ProductMemory& productMemory,
      const unsigned char maxLength,
      const unsigned char maxTops,
      const unsigned char numTops,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& cases);
};

#endif

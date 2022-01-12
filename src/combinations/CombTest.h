/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBTEST_H
#define SSS_COMBTEST_H

#include <vector>

using namespace std;

class Distributions;
class Distribution;
class Strategies;
class CombMemory;
class Combination;
class CombEntry;


class CombTest
{
  private:

    void checkReductions(
      // const vector<CombEntry>& centries,
      // const vector<Combination>& uniqs,
      const unsigned cards,
      const CombMemory& combMemory,
      const CombEntry& centry,
      const Strategies& strategies,
      const unsigned char maxRank,
      const Distribution& distribution) const;


  public:

    // void checkAllMinimals(vector<CombEntry>& centries) const;
    void checkAllMinimals(
      const CombMemory& combMemory,
      const unsigned cards) const;

    void checkAllReductions(
      const unsigned cards,
      // const vector<CombEntry>& centries,
      const CombMemory& combMemory,
      // const vector<Combination>& uniqs,
      const Distributions& distributions) const;
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBTEST_H
#define SSS_COMBTEST_H

using namespace std;

class Distributions;
class Distribution;
class DistributionX;
class DistCore;
class Strategies;
class CombMemory;
class CombEntry;


class CombTest
{
  private:

    void checkReductions(
      const unsigned cards,
      const CombMemory& combMemory,
      const CombEntry& centry,
      const Strategies& strategies,
      const unsigned char maxRank,
      const DistributionX& dist) const;
      // const Distribution& distribution) const;


  public:

    void checkAllMinimals(
      const CombMemory& combMemory,
      const unsigned cards) const;

    void checkAllReductions(
      const unsigned cards,
      const CombMemory& combMemory,
      const Distributions& distributions) const;
};

#endif

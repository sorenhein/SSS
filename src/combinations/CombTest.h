/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBTEST_H
#define SSS_COMBTEST_H

using namespace std;

class Distribution;
class Strategies;
class CombMemory;
class CombEntry;


class CombTest
{
  public:

    void checkReductions(
      const unsigned char cards,
      const CombMemory& combMemory,
      const CombEntry& centry,
      const Strategies& strategies,
      const unsigned char maxRank,
      const Distribution& dist) const;
};

#endif

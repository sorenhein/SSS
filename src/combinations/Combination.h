/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBINATION_H
#define SSS_COMBINATION_H

#include <string>

#include "../strategies/Strategies.h"

using namespace std;

class CombEntry;
class Combinations;
class Distributions;
class Distribution;
class Ranks;
class Plays;


class Combination
{
  private:

    // Distribution const * distPtr;

    Strategies strats;

    unsigned char maxRank;


  public:

    Combination();

    void reset();

    void setMaxRank(const unsigned char maxRankIn);

    unsigned char getMaxRank() const;

    void setTrivial(
      const Result& entry,
      const unsigned char count,
      const bool debugFlag = false);

    const Strategies& strategize(
      const CombEntry& centry,
      const Combinations& combinations,
      const Distributions& distributions,
      Ranks& ranks,
      Plays& plays,
      bool debugFlag = false);

    const Strategies& strategizeVoid(
      const CombEntry& centry,
      const Combinations& combinations,
      const Distributions& distributions,
      Ranks& ranks,
      Plays& plays,
      bool debugFlag = false);

    const Strategies& strategies() const;

    // string str() const;

};

#endif

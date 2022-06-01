/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBINATION_H
#define SSS_COMBINATION_H

#include <string>

#include "../strategies/Strategies.h"

#include "../utils/CombinationType.h"

using namespace std;

class CombEntry;
class Combinations;
class Distributions;
class CoverTableau;
class ProductStats;
class DepthStats;
class Ranks;
class Plays;


class Combination
{
  private:

    Strategies strats;

    unsigned char maxRank;


    void setTrivial(
      const Result& entry,
      const unsigned char count,
      const bool debugFlag = false);


  public:

    Combination();

    void reset();

    void forgetDetails();

    void setMaxRank(const unsigned char maxRankIn);

    unsigned char getMaxRank() const;

    CombinationType strategize(
      const CombEntry& centry,
      const Combinations& combinations,
      const Distributions& distributions,
      Ranks& ranks,
      Plays& plays,
      bool symmOnlyFlag,
      bool debugFlag = false);

    const Strategies& strategizeVoid(
      const CombEntry& centry,
      const Combinations& combinations,
      const Distributions& distributions,
      Ranks& ranks,
      Plays& plays,
      bool debugFlag = false);

    void covers(
      Covers& coversIn,
      ProductStats& productStats,
      DepthStats& depthStats);

    const Strategies& strategies() const;

};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLAIN_H
#define SSS_EXPLAIN_H

#include <string>

#include "CoverCategory.h"


using namespace std;


class Explain
{
  private:

    unsigned numStrategyTops;

    unsigned char tricksMinInt;

    ExplainSymmetry explain;

    ExplainSymmetry behave;


  public:

    void setTricks(
      const unsigned char trickMinIn,
      const unsigned weightSymm,
      const unsigned weightAsymm);

    void setTops(const unsigned numStrategyTopsIn);

    void behaveSymmetrically();
    void behaveAntiSymmetrically();
    void behaveGenerally();

    bool skip(
      const unsigned char effectiveDepth,
      const bool symmetricCoverFlag) const;

    bool symmetricComponent() const;
    bool asymmetricComponent() const;

    unsigned char tricksMin() const;
};

#endif

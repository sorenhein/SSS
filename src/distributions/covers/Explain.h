/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLAIN_H
#define SSS_EXPLAIN_H

#include <string>

enum CoverSymmetry: unsigned;
enum CoverComposition: unsigned;


using namespace std;


class Explain
{
  private:

    unsigned numStrategyTops;

    unsigned char tricksMinInt;

    CoverSymmetry explain;

    CoverComposition composition;

    CoverSymmetry behaveInt;


  public:

    void setParameters(
      const unsigned numStrategyTopsIn,
      const unsigned char trickMinIn);

    void setWeights(
      const unsigned weightSymm,
      const unsigned weightAntisymm);

    void setComposition(const CoverComposition compositionIn);

    void behave(const CoverSymmetry behaveIn);

    bool skip(
      const unsigned char effectiveDepth,
      const CoverSymmetry coverSymmetry) const;

    bool skip(
      const unsigned char effectiveDepth,
      const CoverSymmetry coverSymmetry,
      const CoverComposition coverComposition) const;

    unsigned char tricksMin() const;
};

#endif

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

    CoverLength specificLength;

    CoverSymmetry symmetry;

    CoverComposition composition;


  public:

    void setParameters(
      const unsigned numStrategyTopsIn,
      const unsigned char trickMinIn);

    void setSymmetry(const CoverSymmetry symmetryIn);

    void setComposition(const CoverComposition compositionIn);

    void setSpecificLength(const unsigned char lengthIn);

    void unsetSpecificLength();

    const CoverLength& getSpecificLength() const;

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

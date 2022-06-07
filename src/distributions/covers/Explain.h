/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLAIN_H
#define SSS_EXPLAIN_H

#include <string>

enum ExplainSymmetry: unsigned;
enum ExplainComposition: unsigned;


using namespace std;


class Explain
{
  private:

    unsigned numStrategyTops;

    unsigned char tricksMinInt;

    ExplainSymmetry explain;

    ExplainComposition composition;

    ExplainSymmetry behaveInt;


  public:

    void setParameters(
      const unsigned numStrategyTopsIn,
      const unsigned char trickMinIn);

    void setWeights(
      const unsigned weightSymm,
      const unsigned weightAntisymm);

    void setComposition(const ExplainComposition compositionIn);

    void behave(const ExplainSymmetry behaveIn);

    bool skip(
      const unsigned char effectiveDepth,
      const ExplainSymmetry coverSymmetry) const;

    bool skip(
      const unsigned char effectiveDepth,
      const ExplainSymmetry coverSymmetry,
      const ExplainComposition coverComposition) const;

    unsigned char tricksMin() const;
};

#endif

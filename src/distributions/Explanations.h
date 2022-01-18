/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLANATIONS_H
#define SSS_EXPLANATIONS_H

#include <list>
#include <string>

#include "ExplHelp.h"
#include "Explanation.h"

class Result;


using namespace std;


class Explanations
{
  private:

    list<Explanation> explanations;

    list<Explanation const *> fits;


  public:

    Explanations();

    void reset();

    void prepare(
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const unsigned char maxLength,
      const unsigned char maxTops);

    ExplanationState explain(const vector<Result>& results);

    string str() const;
};

#endif

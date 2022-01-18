/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLANATION_H
#define SSS_EXPLANATION_H

#include <vector>
#include <string>

#include "ExplHelp.h"


using namespace std;


class Explanation
{
  private:

    vector<unsigned char> profile;

    ExplanationSpec spec;

    unsigned char weight;


    typedef unsigned char (Explanation::*ExplComparePtr)(
      const unsigned char value,
      const unsigned char ref) const;

    unsigned char lessEqual(
      const unsigned char value,
      const unsigned char ref) const;

    unsigned char equal(
      const unsigned char value,
      const unsigned char ref) const;

    unsigned char greaterEqual(
      const unsigned char value,
      const unsigned char ref) const;


    string strLength() const;

    string strTop() const;


  public:

    Explanation();

    void reset();

    void prepare(
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const ExplanationSpec& specIn);

    ExplanationState explain(vector<unsigned char>& tricks) const;

    unsigned char getWeight() const;

    string str() const;
};

#endif

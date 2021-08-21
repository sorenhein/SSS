/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_STUDY_H
#define SSS_STUDY_H

#include <vector>
#include <list>

#include "../../utils/Compare.h"

class Ranges;
class Result;

using namespace std;


class Study
{
  private:

    // Used for faster comparisons on average.
    vector<unsigned> summary;
    bool studiedFlag;

    // Even more detailed pre-calculations, but requires a Ranges
    // that applies to both Strategy's being compared.

    list<unsigned> profiles;


    void setConstants();


  public:

    Study();

    ~Study();

    void reset();

    void study(const list<Result>& results);

    void unstudy();

    bool studied() const;

    void scrutinize(
      const list<Result>& results,
      const Ranges& ranges);

    bool maybeLessEqualStudied(const Study& study2) const;

    bool lessEqualScrutinized(const Study& study2) const;

    Compare comparePrimaryScrutinized(const Study& study2) const;
};

#endif

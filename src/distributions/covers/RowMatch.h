/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_ROWMATCH_H
#define SSS_ROWMATCH_H

#include <string>

#include "Tricks.h"


using namespace std;

class CoverRow;


// RowMatch is used when piecing together rows from different solutions
// (across different West lengths) into a sparser set of covers.

class RowMatch
{
  private:

    CoverRow const * rowPtr;

    unsigned count;
    size_t lengthFirst;
    size_t lengthLast;

    Tricks tricks;


  public:

    void set(
      CoverRow const * rowPtrIn,
      const size_t westLength,
      const Tricks& tricksIn);

    void add(const Tricks& tricksIn);

    bool contiguous(const size_t westLength) const;

    bool singleCount() const;
    
    const CoverRow& getSingleRow() const;

    const Tricks& getTricks() const;

    string str() const;
};

#endif

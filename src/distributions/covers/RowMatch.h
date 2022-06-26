/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_ROWMATCH_H
#define SSS_ROWMATCH_H

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
      const Tricks& tricksIn)
    {
      rowPtr = rowPtrIn;
      count = 1;
      lengthFirst = westLength;
      lengthLast = westLength;
      tricks = tricksIn;
    };


    void add(const Tricks& tricksIn)
    {
      assert(count > 0);
      count++;
      lengthLast++;
      tricks += tricksIn;
    };

    bool contiguous(const size_t westLength) const
    {
      return (lengthLast + 1 == westLength);
    };


    bool singleCount() const
    {
      return (count == 1);
    };

    
    const CoverRow& getSingleRow() const
    {
      assert(rowPtr != nullptr);
      return * rowPtr;
    };


    const Tricks& getTricks() const
    {
      return tricks;
    };
};

#endif

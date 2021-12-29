/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

// This class is very similar to Result, but it uses Winners rather
// than Winner.  MultiResult is only used in Range where multiple
// winners may indeed occur.  In Strategy they are avoided, and
// as there are a lot of Result's out there, the memory overhead
// should be avoided.  Only the methods of Result that are used are
// replicated here.  Still, it is rather duplicative, but I couldn't
// think of a good way to derive one from the other, or both from
// something common.


#ifndef SSS_MULTIRESULT_H
#define SSS_MULTIRESULT_H

#include <string>

#include "Winners.h"

class Result;

using namespace std;


class MultiResult
{
  private:

    unsigned char distInt;

    unsigned char tricksInt;

    Winners winnersInt;

  public:

    MultiResult();

    MultiResult& operator = (const Result& res);

    void operator *= (const MultiResult& multiResult);

    void operator *= (const Result& result);

    void operator += (const MultiResult& multiResult);

    void operator += (const Result& result);
  
    bool operator == (const MultiResult& multiResult) const;

    bool operator != (const MultiResult& multiResult) const;

    Compare compareComplete(const MultiResult& res2) const;

    Result result() const;

    unsigned char dist() const;

    unsigned char tricks() const;

    string strHeaderEntry(
      const bool rankFlag,
      const string& title = "") const;

    string strEntry(const bool rankFlag) const;
};

#endif

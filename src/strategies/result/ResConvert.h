/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RESCONVERT_H
#define SSS_RESCONVERT_H

#include <vector>
#include <list>

class Result;
class Ranges;

using namespace std;


class ResConvert
{
  private:

    vector<unsigned char> lookupGE;

    void setConstants();

    void increment(
      unsigned& counter,
      unsigned& profile,
      const unsigned incr,
      list<unsigned>& profiles) const;

    void finish(
      unsigned& counter,
      unsigned& accum,
      unsigned& position,
      unsigned& result) const;


  public:

    ResConvert();

    void scrutinizeRange(
      const list<Result>& results,
      const Ranges& ranges,
      list<unsigned>& profiles) const;

    bool greaterEqual(
      const unsigned arg1,
      const unsigned arg2) const;
};

#endif

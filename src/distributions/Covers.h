/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERS_H
#define SSS_COVERS_H

#include <list>
#include <vector>
#include <string>

#include "Cover.h"

class Distribution;
class CoverMemory;
class Result;


using namespace std;


class Covers
{
  private:

    list<Cover> covers;


  public:

    Covers();

    void reset();

    void prepare(
      const CoverMemory& coverMemory,
      const unsigned char maxLength,
      const unsigned char maxTops,
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const vector<unsigned char>& cases);

    CoverState explain(
      const list<Result>& results,
      list<Cover const *>& fits) const;

    string str(list<Cover const *>& fits) const;
};

#endif

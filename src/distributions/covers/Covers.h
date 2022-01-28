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
#include "ResExpl.h"

class Distribution;
class CoverMemory;
class Result;


using namespace std;


class Covers
{
  private:

    list<Cover> covers;


    void setup(
      const list<Result>& results,
      vector<unsigned char>& tricks,
      unsigned char& tricksMin) const;

    string strDebug(
      const string& title,
      const vector<unsigned char>& tricks) const;


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
      ResExpl& resExpl) const;
};

#endif

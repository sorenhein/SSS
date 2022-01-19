/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERS_H
#define SSS_COVERS_H

#include <list>
#include <string>

#include "CoverHelp.h"
#include "Cover.h"

class Result;


using namespace std;


class Covers
{
  private:

    list<Cover> covers;

    list<Cover const *> fits;

    void prepareSpecific(
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const unsigned char maxLength,
      const unsigned char maxTops,
      list<Cover>::iterator& iter);

    void prepareMiddles(
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const unsigned char maxLength,
      const unsigned char maxTops,
      list<Cover>::iterator& iter);


  public:

    Covers();

    void reset();

    void prepare(
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const unsigned char maxLength,
      const unsigned char maxTops);

    CoverState explain(const vector<Result>& results);

    string str() const;
};

#endif

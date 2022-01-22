/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERMEMORY_H
#define SSS_COVERMEMORY_H

#include <vector>
#include <list>
#include <string>

#include "CoverHelp.h"

using namespace std;


class CoverMemory
{
  private:

    vector<vector<list<CoverSpec>>> specs;


    CoverSpec& add(
      const unsigned char cards,
      const unsigned char tops1);

    void prepare_2_2();

    void prepare_7_1();


  public:

    CoverMemory();

    void reset();

    void prepare(const unsigned char maxCards);

    list<CoverSpec>::const_iterator begin(
      const unsigned cards,
      const unsigned tops1) const;

    list<CoverSpec>::const_iterator end(
      const unsigned cards,
      const unsigned tops1) const;

    string str(
      const unsigned cards,
      const unsigned tops1) const;
};

#endif

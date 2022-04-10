/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERMEMORY_H
#define SSS_COVERMEMORY_H

#include <list>

using namespace std;


class CoverMemory
{
  private:


  public:

    void prepareRows(
      const unsigned char maxLength,
      const unsigned char maxTops,
      const unsigned char numTops,
      list<list<ManualData>>& manualData);
};

#endif

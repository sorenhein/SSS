/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TOPDATA_H
#define SSS_TOPDATA_H

// TODO Put these into a .cpp file?
#include "../../../utils/table.h"
#include "../../../ranks/RankNames.h"

using namespace std;

class RankNames;


struct TopData
{
  unsigned char value;
  RankNames const * rankNamesPtr;


  bool used()
  {
    return (value > 0);
  };

  string strTops(const unsigned char count)
  {
    assert(rankNamesPtr);

    if (count == 0)
      return "";
    else if (count == value)
      return rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);
    else
      return rankNamesPtr->strComponent(RANKNAME_ABSOLUTE_SHORT).
        substr(0, count);
  };
};

#endif

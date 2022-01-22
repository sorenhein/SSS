/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERHELP_H
#define SSS_COVERHELP_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "CoverElement.h"

#include "../const.h"


enum CoverMode
{
  COVER_LENGTHS_ONLY = 0,
  COVER_TOPS_ONLY = 1,
  COVER_LENGTHS_OR_TOPS = 2,
  COVER_LENGTHS_AND_TOPS = 3,
  COVER_MODE_SIZE = 4
};

enum CoverState
{
  COVER_DONE = 0,
  COVER_OPEN = 1,
  COVER_IMPOSSIBLE = 2,
  COVER_STATE_SIZE = 3
};

struct CoverSpec
{
  CoverMode mode;
  CoverElement westLength;
  CoverElement westTop1;

  string strLength() const
  {
    return westLength.str("cards");
  };

  string strTop1() const
  {
    return westTop1.str("tops");
  };
};

#endif

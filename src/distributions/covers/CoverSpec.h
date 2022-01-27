/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSPEC_H
#define SSS_COVERSPEC_H

#include <string>

#include "CoverElement.h"


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
  // For easier identification.  Could perhaps be unsigned char
  unsigned index;

  bool invertFlag;

  unsigned char oppsLength;
  unsigned char oppsTops1;

  CoverMode mode;
  CoverElement westLength;
  CoverElement westTop1;

  CoverSpec();

  string strLengthEqual() const;
  string strLengthInside() const;
  string strLength() const;

  string strTop1Equal() const;
  string strTop1Inside() const;
  string strTop1() const;

  string strBothEqual() const;

  string str() const;
};

#endif

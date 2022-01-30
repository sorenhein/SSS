/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/


#include "CoverSet.h"


bool CoverSet::includesLength(
  const unsigned char wlen,
  const unsigned char oppsLength) const
{
  if (symmFlag)
    return length.includes(wlen) || length.includes(oppsLength - wlen);
  else
    return length.includes(wlen);
}


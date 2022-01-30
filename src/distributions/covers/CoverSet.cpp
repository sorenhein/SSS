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


bool CoverSet::includesTop1(
  const unsigned char wtop,
  const unsigned char oppsTops1) const
{
  if (symmFlag)
    return top1.includes(wtop) || length.includes(oppsTops1 - wtop);
  else
    return top1.includes(wtop);
}


bool CoverSet::includesLengthAndTop1(
  const unsigned char wlen,
  const unsigned char wtop,
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  if (symmFlag)
  {
    return 
      (length.includes(wlen) && top1.includes(wtop)) ||
      (length.includes(oppsLength - wlen) && 
        top1.includes(oppsTops1 - wtop));
  }
  else
    return length.includes(wlen) && top1.includes(wtop);
}


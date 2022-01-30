/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverSet.h"


void CoverSet::reset()
{
  mode = COVER_MODE_NONE;
  symmFlag = false;
  length.setOperator(COVER_OPERATOR_SIZE);
  top1.setOperator(COVER_OPERATOR_SIZE);
}


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


bool CoverSet::includes(
  const unsigned char wlen,
  const unsigned char wtop,
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  if (mode == COVER_MODE_NONE)
    return false;
  else if (mode == COVER_LENGTHS_ONLY)
    return CoverSet::includesLength(wlen, oppsLength);
  else if (mode == COVER_TOPS_ONLY)
    return CoverSet::includesTop1(wtop, oppsTops1);
  else if (mode == COVER_LENGTHS_AND_TOPS)
    return CoverSet::includesLengthAndTop1(
      wlen, wtop, oppsLength, oppsTops1);
  else
  {
    assert(false);
    return false;
  }
}


string CoverSet::strLengthEqual(const unsigned char oppsLength) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");
  const unsigned char wlen = length.value1;

  if (wlen == 0)
    ss << side << " is void";
  else if (wlen == oppsLength)
  {
    assert(! symmFlag);
    ss << "East is void";
  }
  else if (wlen == 1)
    ss << side << " has a singleton";
  else if (wlen == oppsLength-1)
  {
    assert(! symmFlag);
    ss << "East has a singleton";
  }
  else if (wlen == 2)
  {
    if (oppsLength > 4)
      ss << side << " has a doubleton";
    else
      ss << "The suit splits 2=2";
  }
  else
    ss << "The suit splits " << +wlen << "=" << +(oppsLength - wlen);

  return ss.str();
}


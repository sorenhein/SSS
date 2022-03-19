/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "TermCompare.h"


TermCompare::TermCompare()
{
  TermCompare::setConstants();
}


void TermCompare::setConstants()
{
  // Bit layout:
  // 13 12   11 10  9  8   7  6  5  4   3  2  1  0
  // oper  |    lower    |    upper   | (for input)

  lookup.resize(1 << 14);

  for (unsigned lower = 0; lower < 16; lower++)
  {
    for (unsigned upper = 0; upper < 16; upper++)
    {
      const unsigned index0 = (lower << 8) | (upper << 4);

      for (unsigned value = 0; value < 16; value++)
      {
        const unsigned index1 = index0 | value;

        lookup[index1 | (COVER_EQUAL << 12)] = 
          (value == lower);
        lookup[index1 | (COVER_INSIDE_RANGE << 12)] = 
          (value >= lower && value <= upper);
        lookup[index1 | (COVER_GREATER_EQUAL << 12)] = 
          (value >= lower);
        lookup[index1 | (COVER_LESS_EQUAL << 12)] = 
          (value <= lower);
      }
    }
  }
}


unsigned short TermCompare::getIndex(
  const unsigned char lower,
  const unsigned char upper,
  const CoverOperator oper) const
{
  return 
    (static_cast<unsigned short>(oper) << 12) | 
    (static_cast<unsigned short>(lower) << 8) | 
    (static_cast<unsigned short>(upper) << 4);
}


bool TermCompare::includes(
  const unsigned short index,
  const unsigned char value) const
{
  return lookup[index | value];
}


unsigned char TermCompare::getData(
  const Opponent opponent,
  const bool usedFlag,
  const unsigned char range,
  const unsigned char complexity) const
{
  // Coding:
  // 7   opponent
  // 6   usedFlag
  // 2-5 range
  // 0-1 complexity
  return
    (static_cast<unsigned char>(opponent) << 7) |
    (static_cast<unsigned char>(usedFlag) << 6) |
    (range << 2) |
    complexity;
}


Opponent TermCompare::opponent(const unsigned char data) const
{
  return ((data & 0x80) ? OPP_EAST : OPP_WEST);
}


bool TermCompare::used(const unsigned char data) const
{
  return (data & 0x40);
}


unsigned char TermCompare::range(const unsigned char data) const
{
  return ((data >> 2) & 0xf);
}


unsigned char TermCompare::complexity(const unsigned char data) const
{
  return (data & 0x3);
}


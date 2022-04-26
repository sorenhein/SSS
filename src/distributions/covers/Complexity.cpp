/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "Complexity.h"


using namespace std;


void Complexity::reset()
{
  sum = 0;
  max = 0;
}


void Complexity::addCover(
  const unsigned char coverComplexity,
  const unsigned char rowComplexity)
{
  sum += coverComplexity;
  if (rowComplexity > max)
    max = rowComplexity;
}


void Complexity::addRow(const unsigned char rowComplexity)
{
  sum += rowComplexity;
  if (rowComplexity > max)
    max = rowComplexity;
}


unsigned char Complexity::headroom(const Complexity& solution) const
{
  if (solution.sum == 0)
    return numeric_limits<unsigned char>::max();
  else if (sum >= solution.sum)
    return 0;
  else
    return solution.sum - sum;
}


bool Complexity::operator < (const Complexity& tc2) const
{
  if (sum < tc2.sum)
    return true;
  else if (sum > tc2.sum)
    return false;
  else
    return (max < tc2.max);
}


string Complexity::str() const
{
  return to_string(+sum) + "/" + to_string(+max);
}


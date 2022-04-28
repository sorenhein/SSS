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


bool Complexity::match(
  const unsigned char coverComplexity,
  const unsigned char rowComplexity,
  const Complexity& solution) const
{
  if (solution.sum == 0)
    return true;
  if (sum + coverComplexity > solution.sum)
    return false;
  else if (sum + coverComplexity < solution.sum)
    return true;
  else
    // The cover would be added to this specific row
    return (rowComplexity + coverComplexity < solution.max);
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


bool Complexity::operator < (const Complexity& comp2) const
{
  if (sum < comp2.sum)
    return true;
  else if (sum > comp2.sum)
    return false;
  else
    return (max < comp2.max);
}


string Complexity::str() const
{
  return to_string(+sum) + "/" + to_string(+max);
}


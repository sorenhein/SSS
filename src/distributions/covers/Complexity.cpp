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
  raw = 0;
}


bool Complexity::match(
  const unsigned char coverComplexity,
  const unsigned char rowComplexity,
  const unsigned rawWeight,
  const Complexity& solution) const
{
  if (solution.sum == 0)
    return true;
  if (sum + coverComplexity > solution.sum)
    return false;
  else if (sum + coverComplexity < solution.sum)
    return true;
  else if (rowComplexity + coverComplexity > solution.max)
    // The cover would be added to this specific row
    return false;
  else if (rowComplexity + coverComplexity < solution.max)
    return true;
  else
    return (raw + rawWeight < solution.raw);
}


void Complexity::addCover(
  const unsigned char coverComplexity,
  const unsigned char rowComplexity,
  const unsigned rawWeight)
{
  sum += coverComplexity;
  if (rowComplexity > max)
    max = rowComplexity;
  raw += rawWeight;
}


void Complexity::addRow(
  const unsigned char rowComplexity,
  const unsigned rawWeight)
{
  sum += rowComplexity;
  if (rowComplexity > max)
    max = rowComplexity;
  raw += rawWeight;
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
  else if (max < comp2.max)
    return true;
  else if (max < comp2.max)
    return false;
  else
    return (raw < comp2.raw);
}


bool Complexity::compareAgainstPartial(const Complexity& partial) const
{
  // Complexity itself is supposed to the a finished solution here,
  // while partial comes from an unfinished stack entry.
  // The partial entry will need at least one more cover, and here
  // we assume (see Term) that any complexity is >= 2.
  // In this context, a true return means that we can definitely
  // discard the partial, and false means we can't.
  
  if (sum < partial.sum + 2)
    return true;
  else if (sum > partial.sum + 2)
    return false;
  else if (max < partial.max)
    return true;
  else
    return false;
}


string Complexity::str() const
{
  return to_string(+sum) + "/" + to_string(+max);
}


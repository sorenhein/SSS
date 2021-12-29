/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include "Compare.h"

using namespace std;


Compare compressCore(const unsigned detail)
{
  // Only deals with secondary comparisons.
  if (detail & WIN_DIFFERENT_SECONDARY)
    return WIN_DIFFERENT;

  if (detail & WIN_FIRST_SECONDARY)
  {
    if (detail & WIN_SECOND_SECONDARY)
      return WIN_DIFFERENT;
    else
      return WIN_FIRST;
  }
  else if (detail & WIN_SECOND_SECONDARY)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


Compare compressCompareDetail(const unsigned detail)
{
  // First deals with primary comparisons, then uses compressCore.
  if (detail & WIN_DIFFERENT_PRIMARY)
    return WIN_DIFFERENT;
  else if (detail & WIN_FIRST_PRIMARY)
  {
    if (detail & WIN_SECOND_PRIMARY)
      return WIN_DIFFERENT;
    else
      return WIN_FIRST;
  }
  else if (detail & WIN_SECOND_PRIMARY)
    return WIN_SECOND;
  else
    return compressCore(detail);
}


void processCore(
  const unsigned detail,
  Compare& compressed,
  CompareDetail& cleaned)
{
  // Only deals with secondary comparisons.
  if (detail & WIN_DIFFERENT_SECONDARY)
  {
    compressed = WIN_DIFFERENT;
    cleaned = WIN_DIFFERENT_SECONDARY;
  }
  else if (detail & WIN_FIRST_SECONDARY)
  {
    if (detail & WIN_SECOND_SECONDARY)
    {
      compressed = WIN_DIFFERENT;
      cleaned = WIN_DIFFERENT_SECONDARY;
    }
    else
    {
      compressed = WIN_FIRST;
      cleaned = WIN_FIRST_SECONDARY;
    }
  }
  else if (detail & WIN_SECOND_SECONDARY)
  {
    compressed = WIN_SECOND;
    cleaned = WIN_SECOND_SECONDARY;
  }
  else
  {
    compressed = WIN_EQUAL;
    cleaned = WIN_EQUAL_OVERALL;
  }
}


void processCompareDetail(
  const unsigned detail,
  Compare& compressed,
  CompareDetail& cleaned)
{
  // This turns detail into both a compressed (Compare) and a 
  // more informative (CompareDetail) description.
  // First deals with primary comparisons, then uses processCore.

  if (detail & WIN_DIFFERENT_PRIMARY)
  {
    compressed = WIN_DIFFERENT;
    cleaned = WIN_DIFFERENT_PRIMARY;
  }
  else if (detail & WIN_FIRST_PRIMARY)
  {
    if (detail & WIN_SECOND_PRIMARY)
    {
      compressed = WIN_DIFFERENT;
      cleaned = WIN_DIFFERENT_PRIMARY;
    }
    else
    {
      compressed = WIN_FIRST;
      cleaned = WIN_FIRST_PRIMARY;
    }
  }
  else if (detail & WIN_SECOND_PRIMARY)
  {
    compressed = WIN_SECOND;
    cleaned = WIN_SECOND_PRIMARY;
  }
  else
    processCore(detail, compressed, cleaned);
}


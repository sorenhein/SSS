/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

// Useful for two-level comparisons, concretely in Result.

#ifndef SSS_COMPSEC_H
#define SSS_COMPSEC_H

#include "Compare.h"


Compare compressCore(const unsigned detail);

Compare compressCompareDetail(const unsigned detail);

void processCore(
  const unsigned detail,
  Compare& compressed,
  CompareDetail& cleaned);

void processCompareDetail(
  const unsigned detail,
  Compare& compressed,
  CompareDetail& cleaned);

#endif

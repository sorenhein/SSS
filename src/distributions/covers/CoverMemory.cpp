/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "Covers.h"

#include "CoverMemory.h"
#include "Manual.h"


void CoverMemory::prepareRows(
  const unsigned char maxLength,
  const unsigned char maxTops,
  const unsigned char numTops,
  list<list<ManualData>>& manualData)
{
  Profile sumProfile;
  sumProfile.setSingle(numTops, maxLength, maxTops);

  Manual manual;
  manual.make(maxLength, maxTops, numTops, manualData);
}


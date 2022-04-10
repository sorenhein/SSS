/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "Covers.h"

#include "CoverMemory.h"
#include "Manual.h"
#include "ExplStats.h"

#include "Profile.h"
#include "ProfilePair.h"
#include "Product.h"
#include "ProductMemory.h"


void CoverMemory::resize(const unsigned char maxCards)
{
  counts.resize(maxCards+1);
  for (unsigned char c = 0; c <= maxCards; c++)
    counts[c].resize(14);
}


void CoverMemory::resizeStats(ExplStats& explStats) const
{
  explStats.resize(counts);
}


void CoverMemory::prepareRows(
  Covers& covers,
  ProductMemory& productMemory,
  const unsigned char maxLength,
  const unsigned char maxTops,
  const unsigned char numTops,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  assert(maxLength < counts.size());
  assert(maxTops < counts[maxLength].size());

  Profile sumProfile;
  sumProfile.setSingle(numTops, maxLength, maxTops);

  Manual manual;
  list<list<ManualData>> manualData;

  manual.make(maxLength, maxTops, numTops, manualData);

  counts[maxLength][maxTops] = manualData.size();

  unsigned index = 0;
  for (auto& manualList: manualData)
  {
    covers.prepareRowNew(
      productMemory,
      manualList,
      sumProfile,
      index++,
      distProfiles,
      cases);
  }
}


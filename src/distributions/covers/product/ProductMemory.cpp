/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "ProductMemory.h"

#include "Profile.h"
#include "ProfilePair.h"


ProductMemory::ProductMemory()
{
  ProductMemory::reset();
}


void ProductMemory::reset()
{
  memory.clear();
  enterStats.clear();
}


void ProductMemory::resize(const unsigned char memSize)
{
  memory.resize(memSize);
  enterStats.resize(memSize);
}


ProductUnit * ProductMemory::enterOrLookup(
  const Profile& sumProfile,
  const ProfilePair& profilePair)
{
  const unsigned numTops = sumProfile.size();
  assert(numTops < memory.size());

  unsigned long long code = profilePair.getCode(sumProfile);
  auto it = memory[numTops].find(code);

  if (it == memory[numTops].end())
  {
    ProductUnit& productUnit = memory[numTops][code] = ProductUnit();

    productUnit.product.resize(numTops);
    profilePair.setProduct(productUnit.product, sumProfile);
    
    productUnit.numCovers = 1;
    productUnit.numTableaux = 0;
    productUnit.numUses = 0;

    enterStats[numTops].numUnique++;
    enterStats[numTops].numTotal++;

    return &productUnit;
  }
  else
  {
    ProductUnit& productUnit = it->second;
    productUnit.numCovers++;

    enterStats[numTops].numTotal++;

    return &productUnit;
  }
}


ProductUnit const * ProductMemory::lookupByTop(
  const Profile& sumProfile,
  const ProfilePair& profilePair) const
{
  const unsigned numTops = sumProfile.size();
  assert(numTops < memory.size());

  // profilePair only has the highest top set (perhaps).
  // In order to look up the pair, we have to fill out the other tops
  // as don't-care.
  ProfilePair pairCopy = profilePair;
  for (unsigned char i = 0; i+1 < static_cast<unsigned char>(numTops); i++)
    pairCopy.addTop(i, 0, sumProfile[i]);

  unsigned long long code = pairCopy.getCode(sumProfile);
  auto it = memory[numTops].find(code);
if (it == memory[numTops].end())
{
cout << "lookup pair:\n" << profilePair.strLines();
cout << "sumProfile " << sumProfile.strLine();
  assert(it != memory[numTops].end());
}

  // Don't count these.
  return &(it->second);
}


string ProductMemory::strEnterStats() const
{
  stringstream ss;

  ss << "ProductMemory entry statistics\n\n";

  ss <<
    setw(8) << "Numtops" <<
    enterStats[0].strHeader() <<
    "\n";

  EnterStat sum;

  for (unsigned i = 0; i < enterStats.size(); i++)
  {
    if (enterStats[i].numTotal == 0)
      continue;

    ss << setw(8) << i << enterStats[i].str() << "\n";
    
    sum += enterStats[i];
  }

  ss << string(44, '-') << "\n";
  ss << setw(8) << " " << sum.str() << "\n";

  return ss.str() + "\n";
}


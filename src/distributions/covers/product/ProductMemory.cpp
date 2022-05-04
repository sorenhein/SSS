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
  factoredMemory.clear();
  productMemory.clear();
  enterStats.clear();
}


void ProductMemory::resize(const unsigned char memSize)
{
  factoredMemory.resize(memSize);
  productMemory.resize(memSize);
  enterStats.resize(memSize);
}


FactoredProduct * ProductMemory::enterOrLookup(
  const Profile& sumProfile,
  const ProfilePair& profilePair)
{
  const unsigned numTops = sumProfile.size();
  assert(numTops < factoredMemory.size());

  unsigned long long code = profilePair.getCode(sumProfile);
  auto fit = factoredMemory[numTops].find(code);

  if (fit == factoredMemory[numTops].end())
  {
    enterStats[numTops].numUnique++;
    enterStats[numTops].numTotal++;

    // Enter a new factored product.
    FactoredProduct& factoredProduct = 
      factoredMemory[numTops][code] = FactoredProduct();

    const unsigned char canonicalShift =
      profilePair.getCanonicalShift(sumProfile);

    if (canonicalShift == 0)
    {
      // We will get a canonical product directly from profilePair.
      auto pit = productMemory[numTops].find(code);
      if (pit == productMemory[numTops].end())
      {
        // We have not seen this product before.
        Product& product = productMemory[numTops][code] = Product();

        product.resize(numTops);
        profilePair.setProduct(product, sumProfile, code);

        factoredProduct.set(&product, canonicalShift);
      }
      else
      {
assert(false);
        Product& product = pit->second;
        factoredProduct.set(&product, canonicalShift);
      }
    }
    else
    {
      // Look up the corresponding canonical product.
      const unsigned long long canonicalCode =
        profilePair.getCanonicalCode(code, canonicalShift);

      const unsigned canonTops = numTops - canonicalShift;
      auto canonIt = factoredMemory[canonTops].find(canonicalCode);
      assert(canonIt != factoredMemory[canonTops].end());

      factoredProduct.set(canonIt->second, canonicalShift);
    }

    return &factoredProduct;
  }
  else
  {
    // Look up an existing element.
    enterStats[numTops].numTotal++;
    return &(fit->second);
  }
}


FactoredProduct const * ProductMemory::lookupByTop(
  const Profile& sumProfile,
  const ProfilePair& profilePair) const
{
  const unsigned numTops = sumProfile.size();
  assert(numTops < factoredMemory.size());

  // profilePair only has the highest top set (perhaps).
  // In order to look up the pair, we have to fill out the other tops
  // as don't-care.
  ProfilePair pairCopy = profilePair;
  for (unsigned char i = 0; i+1 < static_cast<unsigned char>(numTops); i++)
    pairCopy.setTop(i, 0, sumProfile[i]);

  unsigned long long code = pairCopy.getCode(sumProfile);
  auto fit = factoredMemory[numTops].find(code);
  assert(fit != factoredMemory[numTops].end());

  // Don't count these.
  return &(fit->second);
}


string ProductMemory::strEnterStats() const
{
  stringstream ss;

  ss << "ProductMemory entry statistics\n\n";

// TODO Delete
unsigned numprod = 0;
for (unsigned i = 0; i < productMemory.size(); i++)
{
  ss << setw(2) << i << setw(8) << productMemory[i].size() << "\n";
  numprod += productMemory[i].size();
}
ss << "NUMPROD " << numprod << "\n\n";

  ss <<
    setw(8) << "Numtops" <<
    enterStats[0].strHeader();

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


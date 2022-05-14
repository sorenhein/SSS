/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "ProductMemory.h"

#include "Profile.h"
#include "ProfilePair.h"

mutex mtxProductMemory;


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


void ProductMemory::resize(const size_t memSize)
{
  factoredMemory.resize(memSize);
  enterStats.resize(memSize);
}


void ProductMemory::enterCanonical(
  const Profile& sumProfile,
  const ProfilePair& profilePair,
  const size_t canonicalTops,
  const unsigned char canonicalShift,
  const unsigned long long canonicalCode,
  FactoredProduct& factoredProduct)
{
  enterStats[canonicalTops].numCanonical++;

  productMemory.emplace_back(Product());
  Product& product = productMemory.back();

  product.resize(canonicalTops);
  profilePair.setProduct(product, sumProfile, canonicalCode);

  factoredProduct.set(&product, canonicalShift);
}


FactoredProduct * ProductMemory::enterOrLookup(
  const Profile& sumProfile,
  const ProfilePair& profilePair)
{
  lock_guard<mutex> lg(mtxProductMemory);

  const size_t numTops = sumProfile.size();
  assert(numTops < factoredMemory.size());
  enterStats[numTops].numTotal++;

  unsigned long long code = profilePair.getCode(sumProfile);
  auto fit = factoredMemory[numTops].find(code);

  if (fit == factoredMemory[numTops].end())
  {
    enterStats[numTops].numUnique++;

    // Enter a new factored product.
    FactoredProduct& factoredProduct = 
      factoredMemory[numTops][code] = FactoredProduct();

    const unsigned char canonicalShift =
      profilePair.getCanonicalShift(sumProfile);

    if (canonicalShift == 0)
    {
      // We will get a canonical product directly from profilePair.
      // It will always be different from others we have seen.
      ProductMemory::enterCanonical(
        sumProfile, profilePair, numTops, 0, code, factoredProduct);
    }
    else
    {
      // Look up the corresponding canonical product.
      const unsigned long long canonicalCode =
        profilePair.getCanonicalCode(code, canonicalShift);

      const size_t canonicalTops = numTops - canonicalShift;
      auto canonIt = factoredMemory[canonicalTops].find(canonicalCode);

      if (canonIt == factoredMemory[canonicalTops].end())
      {
        // This only happens when we solve single distributions.
        // In this case we have to log the canonical entry first.
        ProductMemory::enterCanonical(
          sumProfile, profilePair, canonicalTops,
          canonicalShift, canonicalCode, factoredProduct);
      }
      else
      {
        // Use the canonical product that we found.
        factoredProduct.set(canonIt->second, canonicalShift);
      }
    }

    return &factoredProduct;
  }
  else
  {
    // Look up an existing element.
    return &(fit->second);
  }
}


FactoredProduct const * ProductMemory::lookupByTop(
  const Profile& sumProfile,
  const ProfilePair& profilePair) const
{
  const size_t numTops = sumProfile.size();
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

  ss << string(56, '-') << "\n";
  ss << setw(8) << " " << sum.str() << "\n";

  return ss.str() + "\n";
}


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
  enterStats.resize(memSize);
}


FactoredProduct * ProductMemory::enterOrLookup(
  const Profile& sumProfile,
  const ProfilePair& profilePair)
{
  const unsigned numTops = sumProfile.size();
  assert(numTops < factoredMemory.size());

  unsigned long long code = profilePair.getCode(sumProfile);
  auto it = factoredMemory[numTops].find(code);

  if (it == factoredMemory[numTops].end())
  {
    // Enter a new product.
    productMemory.emplace_back(Product());
    Product& product = productMemory.back();
    product.resize(numTops);
    profilePair.setProduct(product, sumProfile, code);

    // Enter a new factored product.
    FactoredProduct& factoredProduct = 
      factoredMemory[numTops][code] = FactoredProduct();

    enterStats[numTops].numUnique++;
    enterStats[numTops].numTotal++;

const unsigned char cs = profilePair.getCanonicalShift(sumProfile);

    // TODO Make this cleaner -- perhaps an own method or
    // a recursive call.
    if (product.canonical())
    {
      factoredProduct.canonicalPtr = &product;
      factoredProduct.canonicalShift = 0;
    }
    else
    {
      factoredProduct.canonicalShift = product.getCanonicalShift();
      const unsigned long long canonicalCode =
        profilePair.getCanonicalCode(code, factoredProduct.canonicalShift);

      const unsigned canonTops = numTops - factoredProduct.canonicalShift;
      auto canonIt = factoredMemory[canonTops].find(canonicalCode);
      assert(canonIt != factoredMemory[canonTops].end());

      factoredProduct.canonicalPtr = canonIt->second.canonicalPtr;
    }
assert(factoredProduct.canonicalShift == cs);

    return &factoredProduct;
  }
  else
  {
    // Look up an existing element.
    FactoredProduct& factoredProduct = it->second;

    enterStats[numTops].numTotal++;

    return &factoredProduct;
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
  auto it = factoredMemory[numTops].find(code);
if (it == factoredMemory[numTops].end())
{
cout << "lookup pair:\n" << profilePair.strLines();
cout << "sumProfile " << sumProfile.strLine();
  assert(it != factoredMemory[numTops].end());
}

  // Don't count these.
  return &(it->second);
}


string ProductMemory::strEnterStats() const
{
  stringstream ss;

  ss << "ProductMemory entry statistics\n\n";

ss << "NUMPROD " << productMemory.size() << "\n\n";
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


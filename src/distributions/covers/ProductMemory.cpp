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
}


void ProductMemory::resize(const unsigned char memSize)
{
  memory.resize(memSize);
}


Product const * ProductMemory::enterOrLookup(
  const Profile& sumProfile,
  const ProfilePair& profilePair)
{
  const unsigned numTops = sumProfile.size();
  assert(numTops < memory.size());

  unsigned long long code = profilePair.getCode(sumProfile);
  auto it = memory[numTops].find(code);

  if (it == memory[numTops].end())
  {
// cout << "ADDPM\n";
// cout << "ADD " << +numTops << "\n" << profilePair.strLines();
    Product& product = memory[numTops][code] = Product();

    product.resize(numTops);
    product.set(sumProfile, profilePair);
    return &product;
  }
  else
  {
// cout << "GOTPM\n";
// cout << "GOT " << +numTops << "\n" << profilePair.strLines();
// cout << "LOOKUP " << (it->second).strLine() << endl;
if ((it->second).size() != numTops)
{
  assert(false);
}
    return &(it->second);
  }
}


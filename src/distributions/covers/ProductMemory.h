/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PRODUCTMEMORY_H
#define SSS_PRODUCTMEMORY_H

#include <vector>
#include <map>
#include <string>

#include "Product.h"

using namespace std;

class Profile;
class ProfilePair;


class ProductMemory
{
  private: 

    vector<map<unsigned long long, Product>> memory;


  public:

    ProductMemory();

    void reset();

    void resize(const unsigned char oppsLength);

    Product const * enterOrLookup(
      const Profile& sumProfile,
      const ProfilePair& profilePair);
};

#endif

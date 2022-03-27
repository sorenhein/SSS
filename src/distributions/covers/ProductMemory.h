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


struct ProductUnit
{
  Product product;

  unsigned numCovers;
  unsigned numTableaux;
  unsigned numUses;
};


class ProductMemory
{
  private: 
    
    struct EnterStat
    {
      unsigned numUnique;
      unsigned numTotal;
    };

    vector<map<unsigned long long, ProductUnit>> memory;

    vector<EnterStat> enterStats;


  public:

    ProductMemory();

    void reset();

    void resize(const unsigned char oppsLength);

    ProductUnit * enterOrLookup(
      const Profile& sumProfile,
      const ProfilePair& profilePair);

    string strEnterStats() const;
};

#endif

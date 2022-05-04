/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PRODUCTMEMORY_H
#define SSS_PRODUCTMEMORY_H

#include <iostream>
#include <iomanip>
#include <sstream>

#include <vector>
#include <list>
#include <map>
#include <string>

#include "Product.h"
#include "FactoredProduct.h"

using namespace std;

class Profile;
class ProfilePair;


class ProductMemory
{
  private: 
    
    struct EnterStat
    {
      unsigned numCanonical;
      unsigned numUnique;
      unsigned numTotal;

      void operator += (const EnterStat& estat2)
      {
        numCanonical += estat2.numCanonical;
        numUnique += estat2.numUnique;
        numTotal += estat2.numTotal;
      };

      string strHeader() const
      {
        stringstream ss;

        ss <<
          setw(12) << "Canonical" <<
          setw(12) << "Unique" <<
          setw(12) << "Total" <<
          setw(12) << "Ratio can." <<
          "\n";

        return ss.str();
      };

      string str() const
      {
        stringstream ss;

        ss <<
          setw(12) << numCanonical <<
          setw(12) << numUnique <<
          setw(12) << numTotal <<
          setw(11) << fixed << setprecision(1) <<
            100. * static_cast<float>(numCanonical) /
            static_cast<float>(numTotal) <<
            "%";

        return ss.str();
      };
    };

    vector<map<unsigned long long, FactoredProduct>> factoredMemory;

    list<Product> productMemory;

    vector<EnterStat> enterStats;


    void enterCanonical(
      const Profile& sumProfile,
      const ProfilePair& profilePair,
      const unsigned canonicalTops,
      const unsigned char canonicalShift,
      const unsigned long long canonicalCode,
      FactoredProduct& factoredProduct);


  public:

    ProductMemory();

    void reset();

    void resize(const unsigned char oppsLength);

    FactoredProduct * enterOrLookup(
      const Profile& sumProfile,
      const ProfilePair& profilePair);

    FactoredProduct const * lookupByTop(
      const Profile& sumProfile,
      const ProfilePair& profilePair) const;

    string strEnterStats() const;
};

#endif

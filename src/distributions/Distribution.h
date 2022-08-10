/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTRIBUTIONX_H
#define SSS_DISTRIBUTIONX_H

#include <string>

#include "DistMap.h"
#include "DistCore.h"

using namespace std;

class RanksNames;


class Distribution
{
  private:

    DistMap distMap;

    DistCore * distCorePtr;

    unsigned index;


  public:

    void setRanks(
      const unsigned char cards,
      const unsigned holding2) // Binary, not trinary format
    {
      distMap.setRanks(cards, holding2);
    };

    void setPtr(DistCore * distCorePtrIn)
    {
      assert(distCorePtrIn != nullptr);
      distCorePtr = distCorePtrIn;
    };

    void setPtr(const Distribution& dist)
    {
      assert(dist.distCorePtr != nullptr);
      setPtr(dist.distCorePtr);
    };

    DistID getID() const
    {
      return distMap.getID();
    }

    void split()
    {
      assert(distCorePtr != nullptr);
      distCorePtr->splitAlternative(distMap);
    };

    void setLookups()
    {
      assert(distCorePtr != nullptr);
      distCorePtr->setLookups();
    };

    RanksNames& getRanksNames()
    {
      assert(distCorePtr != nullptr);
      return distCorePtr->getRanksNames();
    }

    size_t size() const
    {
      assert(distCorePtr != nullptr);
      return distCorePtr->size();
    };

    const SurvivorList& getSurvivors(const Play& play) const
    {
      assert(distCorePtr != nullptr);
      return distCorePtr->getSurvivors(distMap, play);
    };

    const Reduction& getReduction(const unsigned char rankNS) const
    {
      assert(distCorePtr != nullptr);
      return distCorePtr->getReduction(distMap, rankNS);
    };

    void prepareCovers(ProductMemory& productMemory)
    {
      assert(distCorePtr != nullptr);
      distCorePtr->prepareCovers(productMemory);
    };

    Covers& covers()
    {
      assert(distCorePtr != nullptr);
      return distCorePtr->getCovers();
    }

    const Covers& covers() const
    {
      assert(distCorePtr != nullptr);
      return distCorePtr->getCovers();
    }

    string str() const
    {
      assert(distCorePtr != nullptr);
      return distCorePtr->str();
    };
};

#endif

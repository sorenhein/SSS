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


class DistributionX
{
  private:

    DistMap distMap;

    DistCore * distCorePtr;


  public:

    void setRanks(
      const unsigned cards,
      const unsigned holding2) // Binary, not trinary format
    {
      distMap.setRanks(cards, holding2);
    };

    void setPtr(DistCore * distCorePtrIn)
    {
      assert(distCorePtrIn != nullptr);
      distCorePtr = distCorePtrIn;
    };

    void setPtr(const DistributionX& dist)
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

    unsigned size() const
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

    string str() const
    {
      assert(distCorePtr != nullptr);
      return distCorePtr->str();
    };
};

#endif

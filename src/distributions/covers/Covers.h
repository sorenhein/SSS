/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERS_H
#define SSS_COVERS_H

#include <list>
#include <vector>
#include <string>

#include "product/Profile.h"

#include "CoverStore.h"
#include "TableauCache.h"
#include "CoverRow.h"


class ProductMemory;
class CoverTableau;
class Tricks;
class Result;


using namespace std;


class Covers
{
  private:

    Profile sumProfile;

    vector<unsigned char> cases;

    CoverStore store;

    TableauCache tableauCache;

    // These are the manually set rows
    list<CoverRow> rows;

    // This is the separate cache corresponding to the rows.
    TableauCache tableauRowCache;


  public:

    Covers();

    void reset();

    void prepare(
      ProductMemory& productMemory,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& casesIn,
      const Profile& sumProfileIn);

    CoverRow& addRow();

    void sortRows();

    const Cover& lookup(const Cover& cover) const;

    void explain(
      const list<Result>& results,
      const unsigned numStrategyTops,
      CoverTableau& solution,
      bool& newTableauFlag);

    CoverState explainManually(
      const list<Result>& results,
      CoverTableau& solution);

    template<class T, class C>
    void explainTemplate(
      const Tricks& tricks,
      const unsigned char tmin,
      const unsigned numStrategyTops,
      const C& candidates,
      list<T>& stack,
      CoverTableau& solution);

    const Profile& getSumProfile() const;

    void getCoverCounts(
      unsigned& numTableaux,
      unsigned& numUses) const;

    string strCache() const;
};


template<class T, class C>
void Covers::explainTemplate(
  const Tricks& tricks,
  const unsigned char tmin,
  const unsigned numStrategyTops,
  const C& candidates,
  list<T>& stack,
  CoverTableau& solution)
{
  stack.emplace_back(T());

  T& entry = stack.back();
  entry.tableau.init(tricks, tmin, cases);
  entry.iter = store.begin();

  auto stackIter = stack.begin();
  while (stackIter != stack.end())
  {
    auto& stackElem = stackIter->tableau;

    auto candIter = stackIter->iter;
    while (candIter != candidates.end())
    {
      if (candIter->effectiveDepth() > numStrategyTops)
      {
        candIter++;
        continue;
      }

      const unsigned char headroom = stackElem.headroom(solution);

      if (candIter->minComplexityAdder(stackElem.getResidualWeight()) > 
          headroom)
      {
        // As the covers are ordered, later covers have no chance either.
        break;
      }

      if (candIter->getComplexity() > headroom)
      {
        // The current cover may be too complex, but there may be others.
        candIter++;
        continue;
      }

      if (stackIter->tableau.attempt(cases, candIter, stack, solution))
      {
        // We found a solution.  It may have replaced the previous one.
        break;
      }

      candIter++;
    }

    // Erasing first stack element.
    stackIter = stack.erase(stackIter);
  }
}

#endif

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

#include "CoverStore.h"
#include "CoverRow.h"
#include "RowStore.h"

#include "tableau/TableauCache.h"

#include "product/Profile.h"


class ProductMemory;
class CoverTableau;
class Tricks;
class Result;
template<typename T> class CoverStack;


using namespace std;


class Covers
{
  private:

    vector<unsigned char> cases;

    CoverStore store;

    // This is the cache corresponding to the algorithmic covers.
    TableauCache tableauCache;

    // These are the manually set rows
    RowStore rows;

    // This is the separate cache corresponding to the rows.
    TableauCache tableauRowCache;

    Profile sumProfile;


    void fillStore(
      ProductMemory& productMemory,
      const vector<Profile>& distProfiles,
      ProfilePair& running);

    template<class C, class T>
    void explainTemplate(
      const Tricks& tricks,
      const unsigned char tmin,
      const unsigned numStrategyTops,
      const C& candidates,
      CoverStack<T>& stack,
      // list<T>& stack,
      CoverTableau& solution);


  public:

    Covers();

    void reset();

    void prepare(
      ProductMemory& productMemory,
      const vector<Profile>& distProfiles,
      const vector<unsigned char>& casesIn,
      const Profile& sumProfileIn);

    void addDirectly(list<Cover const *>& coverPtrs);
    // CoverRow& addRow();

    // void sortRows();

    const Cover& lookup(const Cover& cover) const;

    void explain(
      const list<Result>& results,
      const unsigned numStrategyTops,
      CoverTableau& solution,
      bool& newTableauFlag);

    void explainManually(
      const list<Result>& results,
      CoverTableau& solution);

    const Profile& getSumProfile() const;

    void getCoverCounts(
      unsigned& numTableaux,
      unsigned& numUses) const;

    string strCache() const;

    string strSignature() const;
};


template<class C, class T>
void Covers::explainTemplate(
  const Tricks& tricks,
  const unsigned char tmin,
  const unsigned numStrategyTops,
  const C& candidates,
  CoverStack<T>& stack,
  // list<T>& stack,
  CoverTableau& solution)
{
  stack.emplace(tricks, tmin, candidates.begin());

  // stack.emplace_back(T());

  // T& entry = stack.back();
  // entry.tableau.init(tricks, tmin);
  // entry.iter = candidates.begin();

// unsigned candNo = 0;

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
// candNo++;
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
// candNo++;
        continue;
      }

      if (stackIter->tableau.attempt(cases, candIter, stack, solution))
      {
        // We found a solution.  It may have replaced the previous one.
        break;
      }

// candNo++;
      candIter++;
    }
// cout << "cno " << candNo << ", stack " << stack.size() <<
  // ", soln " << (solution.complete() ? "yes" : "no") <<
  // ", res " << (solution.complete() ? 0 : stackElem.getResidualWeight()) << 
  // "\n";

    // Erasing first stack element.
    stackIter = stack.erase(stackIter);
  }
}

#endif

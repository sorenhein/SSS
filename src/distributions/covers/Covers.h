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
// #include "StackEntry.h"

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


// TODO
extern Edata edata;

template<class C, class T>
void Covers::explainTemplate(
  const Tricks& tricks,
  const unsigned char tmin,
  const unsigned numStrategyTops,
  const C& candidates,
  CoverStack<T>& stack,
  CoverTableau& solution)
{
  stack.emplace(tricks, tmin, candidates.begin());

edata.stackActual = 0;
edata.firstFix = 0;
edata.stackMax = 0;
edata.numSteps = 0;
edata.numCompares = 0;
edata.numSolutions = 0;
edata.numBranches = 0;
// cout << edata.strHeader();

  while (! stack.empty())
  {
    auto handle = stack.extract(stack.begin());
    StackEntry<T>& stackElem = handle.value();
// Can probably avoid overwriting in CoverTableau, but for now
// we make a copy
StackEntry<T> stackElemCopy = stackElem;

    CoverTableau& tableau = stackElem.tableau;
    auto candIter = stackElem.iter;

unsigned tmp = stack.size();
unsigned tmpSolutions = edata.numSolutions;
bool branchFlag = false;
unsigned branchLimit;
if (tmp < 10000)
  branchLimit = 5;
else if (tmp < 30000)
  branchLimit = 3;
else if (tmp < 50000)
  branchLimit = 2;
else
  branchLimit = 1;

    while (candIter != candidates.end())
    {
      if (candIter->effectiveDepth() > numStrategyTops)
      {
        candIter++;
        continue;
      }

      const unsigned char headroom = tableau.headroom(solution);

      if (candIter->minComplexityAdder(tableau.getResidualWeight()) > 
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

      // if (stackIter->tableau.attempt(cases, candIter, stack, solution))
      if (tableau.attempt(cases, candIter, stack, solution))
      {
        // We found a solution.  It may have replaced the previous one.
if (edata.firstFix == 0)
  edata.firstFix = edata.numSteps;
        break;
      }

      candIter++;

      if (candIter != candidates.end() && stack.size() - tmp > branchLimit)
      {
        branchFlag = true;
        break;
      }
    }

edata.numBranches += stack.size() - tmp;

if (stack.size() > edata.stackMax)
  edata.stackMax = stack.size();

if (edata.numSolutions > tmpSolutions)
{
// unsigned cs = stack.size();
  stack.prune(solution);
// cout << "Erased " << cs - stack.size() << " << elements\n";


}

edata.stackActual = stack.size();

edata.numSteps++;
if (edata.numSteps % 100 == 0)
{
  T t;
  // cout << edata.str(t.ID());
}

    if (branchFlag)
    {
      // cout << "Branch\n";
      stackElemCopy.iter = candIter;
      const unsigned w = stackElemCopy.tableau.getResidualWeight();
      if (w == 0)
      {
        assert(false);
      }
      const unsigned char minCompAdder = 
        candIter->minComplexityAdder(w);
      stackElemCopy.tableau.project(minCompAdder);
      stack.insert(stackElemCopy);
    }


  }

/* */
T t;
cout << edata.strHeader();
cout << edata.str(t.ID());
/* */

}

#endif

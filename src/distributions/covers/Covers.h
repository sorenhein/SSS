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
extern unsigned countTMP;

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

// unsigned candNo = 0;

unsigned firstFix = 0;
unsigned stackMax = 0;
unsigned numTries = 0;
countTMP = 0;

  // auto stackIter = stack.begin();
  // while (stackIter != stack.end())
  while (! stack.empty())
  {
    // auto& stackElem = stackIter->tableau;
    // TODO This a an actual copy, but I don't see how to benefit
    // from extract
    StackEntry<T> stackElem = * stack.begin();
    CoverTableau& tableau = stackElem.tableau;
    stack.erase(stack.begin());
    // auto stackElem = stack.extract(stack.begin());

    // auto candIter = stackIter->iter;
    auto candIter = stackElem.iter;
    while (candIter != candidates.end())
    {
      if (candIter->effectiveDepth() > numStrategyTops)
      {
        candIter++;
// candNo++;
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
// candNo++;
        continue;
      }

      // if (stackIter->tableau.attempt(cases, candIter, stack, solution))
      if (tableau.attempt(cases, candIter, stack, solution))
      {
        // We found a solution.  It may have replaced the previous one.
if (firstFix == 0)
  firstFix = numTries;
        break;
      }

// candNo++;
      candIter++;
    }
// cout << "cno " << candNo << ", stack " << stack.size() <<
  // ", soln " << (solution.complete() ? "yes" : "no") <<
  // ", res " << (solution.complete() ? 0 : stackElem.getResidualWeight()) << 
  // "\n";

if (stack.size() > stackMax)
  stackMax = stack.size();

    // Erasing first stack element.
    // stackIter = stack.erase(stackIter);
/*
cout << "Stack now: soln " <<
  (solution.complete() ? "yes " + solution.strBracket() : "no") << "\n";
for (auto& s: stack)
  cout << s.tableau.strBracket() << ", " <<
    s.tableau.lowerTMP() << "\n";
cout << "\n";
*/
{
}

// if (numTries > 20)
  // assert(false);

numTries++;
  }

T tmp;
string s = tmp.ID();
  
cout << s << " ttff " << firstFix << "\n";
cout << s << " smax " << stackMax << "\n";
cout << s << " snum " << numTries << "\n";
cout << s << " coun " << countTMP << "\n";

}

#endif

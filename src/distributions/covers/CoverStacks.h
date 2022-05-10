/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSTACKS_H
#define SSS_COVERSTACKS_H

#include <set>
#include <list>

#include "tableau/CoverTableau.h"

class Cover;
class CoverRow;


using namespace std;


struct CoverStackEntry
{
  CoverTableau tableau;

  set<Cover>::const_iterator iter;

  bool operator < (const CoverStackEntry& se2) const
  {
    return (tableau < se2.tableau);
  };
};

struct RowStackEntry
{
  CoverTableau tableau;

  list<CoverRow>::const_iterator iter;

  bool operator < (const RowStackEntry& rse2) const
  {
    return (tableau < rse2.tableau);
  };
};

template<class C>
struct StackEntry
{
  CoverTableau tableau;

  typename list<C>::const_iterator iter;

  bool operator < (const StackEntry& se2) const
  {
    return (tableau < se2.tableau);
  };
};

template<class C>
class CoverStack
{
  private:

    list<StackEntry<C>> stack;

  public:
};

#endif

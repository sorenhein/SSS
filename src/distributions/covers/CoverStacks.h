/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERSTACKS_H
#define SSS_COVERSTACKS_H

#include <set>
#include <list>

class Cover;
class CoverRow;


using namespace std;


struct StackEntry
{
  CoverTableau tableau;

  set<Cover>::const_iterator iter;
};

struct RowStackEntry
{
  CoverTableau tableau;

  list<CoverRow>::const_iterator iter;
};

#endif

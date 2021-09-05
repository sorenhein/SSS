/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXTENSIONS_H
#define SSS_EXTENSIONS_H

#include <list>

#include "Extension.h"
#include "Splits.h"

using namespace std;

class Slist;
class Strategy;
class Ranges;


class Extensions
{
  private:

    // An Extensions is parallel to a Strategies.

    enum ExtensionSplit
    {
     EXTENSION_SPLIT1 = 0,
     EXTENSION_SPLIT2 = 1
    };


    list<Extension> extensions;

    Splits splits1;
    Splits splits2;


    void split(
      Slist& slist,
      const Strategy& counterpart,
      const Ranges& ranges,
      const ExtensionSplit split);
    
    bool productDominatedHeavier(
      list<Extension>::iterator& iter,
      const Extension& product) const;

    bool processSameWeights(
      list<Extension>::iterator& iter,
      list<Extension>::iterator& piter);

    void eraseDominatedLighter(
      list<Extension>::iterator& iter,
      list<Extension>::iterator& piter);

    void add();

  public:

    Extensions();

    void reset();

    void split(
      Slist& slist1,
      Slist& slist2,
      const Ranges& ranges);
    
    void multiply(const Ranges& ranges);

    void flatten(Slist& slist);
};

#endif

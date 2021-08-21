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

class Strategies;
class Strategy;
class Ranges;


enum ExtensionSplit
{
 EXTENSION_SPLIT1 = 0,
 EXTENSION_SPLIT2 = 1
};


class Extensions
{
  private:

    // An Extensions is parallel to a Strategies.

    list<Extension> extensions;

    Splits splits1;
    Splits splits2;


    /* */
    void makeEntry(
      const Strategy& strat1,
      const Strategy& strat2,
      const Ranges& ranges,
      const unsigned index1,
      const unsigned index2);

    bool lessEqualPrimary(
      const Extension& ext1,
      const Extension& ext2) const;

    Compare compareSecondary(
      const Extension& ext1,
      const Extension& ext2) const;
      /* */

    void add();
    void addNew();


  public:

    Extensions();

    ~Extensions();

    void reset();

    void split(
      Strategies& strategies,
      const Strategy& counterpart,
      const Ranges& ranges,
      const ExtensionSplit split);
    
    void multiply(const Ranges& ranges);

    void flatten(list<Strategy>& strategies);
};

#endif

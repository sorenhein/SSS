#ifndef SSS_EXTENSIONS_H
#define SSS_EXTENSIONS_H

#include <list>
#include <string>

#include "Extension.h"
#include "Splits.h"

#include "../Strategies.h"

#include "../result/Ranges.h"

using namespace std;


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


    void makeEntry(
      const Strategy& strat1,
      const Strategy& strat2,
      const Ranges& ranges,
      const unsigned index1,
      const unsigned index2);

    bool greaterEqualByTricks(
      const Extension& ext1,
      const Extension& ext2,
      CompareDetail& compOverlap) const;

    Compare compareDetail(
      const Extension& ext1,
      const Extension& ext2,
      const CompareDetail& compOverlap) const;

    void add();


  public:

    Extensions();

    ~Extensions();

    void reset();

    void split(
      Strategies& strategies,
      const Strategy& counterpart,
      const ExtensionSplit split);
    
    void multiply(const Ranges& ranges);

    void flatten(list<Strategy>& strategies);
};

#endif

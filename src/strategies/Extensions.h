#ifndef SSS_EXTENSIONS_H
#define SSS_EXTENSIONS_H

#include <list>
#include <string>

#include "Strategies.h"
#include "Extension.h"
#include "Splits.h"

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

  public:

    Extensions();

    ~Extensions();

    void reset();

    void split(
      Strategies& strategies,
      const Strategy& counterpart,
      const ExtensionSplit split);
    
    void multiply();

    void flatten(Strategies& strategies);
};

#endif

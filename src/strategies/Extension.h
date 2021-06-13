#ifndef SSS_EXTENSION_H
#define SSS_EXTENSION_H

#include <list>
#include <string>

#include "Strategy.h"

using namespace std;


class Extension
{
  private:

    // An extension is created from two Strategy's as they are
    // multiplied.  The overlap in terms of distribution is stored
    // in a Strategy.  The unique distributions of the factors are
    // stored in a different place (Extensions), but the extension
    // knows its index within that place.  The weight of the
    // Extension is the weight of the Strategy plus the weights of
    // the unique components.

    Strategy overlap;

    unsigned index1;
    unsigned index2;

    unsigned weightInt;


  public:

    Extension();

    ~Extension();

    void reset();

    unsigned weight() const;

};

#endif

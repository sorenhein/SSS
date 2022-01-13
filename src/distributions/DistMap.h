/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTMAP_H
#define SSS_DISTMAP_H

#include <vector>
#include <string>

#include "DistHelp.h"

// This class contains enough information to map a general binary
// distribution number to and from a canonical one.  The canonical
// aspects are processed in DistCore.


using namespace std;


class DistMap
{
  private:

    friend class DistCore;

    unsigned cards;

    vector<unsigned> full2reduced;
    vector<unsigned> reduced2full;

    unsigned rankSize; // Reduced ranks

    SideInfo opponents;


    void shrink(
      const unsigned maxFullRank,
      const unsigned maxReducedRank);


  public:

    DistMap();

    void reset();

    void setRanks(
      const unsigned cards,
      const unsigned holding2); // Binary, not trinary format

    DistID getID() const;
};

#endif

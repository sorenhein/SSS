#ifndef SSS_STRUCT_H
#define SSS_STRUCT_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "const.h"

using namespace std;


// CombEntry is used to map a given holding to a canonical combination,
// where only the ranks within a suit matter.

struct CombEntry
{
  bool canonicalFlag;
  unsigned canonicalHolding3; // Trinary
  unsigned canonicalHolding2; // Binary
  unsigned canonicalIndex;
  bool rotateFlag;
  // Once we have a Combination, probably
  // Combination * combinationPtr;
};

// Survivor is used in the context of rank-reduced distributions.
// It can happen that a NS card leads two EW ranks to collapse after
// the trick.  The parent trick has a number of possible distributions,
// say 4, and the child trick only has 3 tricks.  These have to be
// mapped to one another.  There is a Survivor for each parent
// distribution.  The fullNo is the parent number and the reducedNo
// is the child number.

struct Survivor
{
  unsigned fullNo;
  unsigned reducedNo;
};

#endif

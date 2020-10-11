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

#endif

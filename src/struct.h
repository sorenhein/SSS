#ifndef SSS_STRUCT_H
#define SSS_STRUCT_H

using namespace std;


// CombEntry is used to map a given holding to a canonical combination,
// where only the ranks within a suit matter.

struct CombEntry
{
  bool canonicalFlag;
  unsigned canonicalHolding;
  unsigned canonicalIndex;
  bool rotateFlag;
  vector<char> canonical2comb;
  // Once we have a Combination, probably
  // Combination * combinationPtr;
};

#endif

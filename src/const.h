#ifndef SSS_CONST_H
#define SSS_CONST_H

#include <string>
#include <map>

using namespace std;


// A real suit has up to 13 cards, but let's see if we can solve more.
#define MAX_CARDS 15

const string CARD_NAMES = "23456789TJQKABC";

enum CardPosition
{
  CONVERT_NORTH = 0,
  CONVERT_SOUTH = 1,
  CONVERT_OPPS = 2
};


// CombEntry is used to map a given holding to a canonical combination,
// where only the ranks within a suit matter.

struct CombEntry
{
  bool canonicalFlag;
  int canonicalHolding;
  int canonicalIndex;
  bool rotateFlag;
  map<string, string> canonical2comb;
  // Once we have a Combination, probably
  // Combination * combinationPtr;
};


#define UNUSED(x) ((void)(true ? 0 : ((x), void(), 0)))

#endif

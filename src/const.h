#ifndef SSS_CONST_H
#define SSS_CONST_H

#include <vector>
#include <limits>

using namespace std;


// A real suit has up to 13 cards, but let's see if we can solve more.
#define MAX_CARDS 15

#define CARD_NAMES "23456789TJQKABC"

enum CardPosition
{
  CONVERT_NORTH = 0,
  CONVERT_SOUTH = 1,
  CONVERT_OPPS = 2
};

enum PairPosition
{
  PAIR_NS = 0,
  PAIR_EW = 1
};

enum SidePosition
{
  SIDE_NORTH = 0,
  SIDE_SOUTH = 1
};

enum CombinationType
{
  COMB_TRIVIAL = 0,
  COMB_CONST = 1,
  COMB_SIMPLE = 2,
  COMB_OTHER = 3
};

const unsigned UNSIGNED_NOT_SET = numeric_limits<unsigned>::max();

#define UNUSED(x) ((void)(true ? 0 : ((x), void(), 0)))

#endif

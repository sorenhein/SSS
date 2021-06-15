#ifndef SSS_CONST_H
#define SSS_CONST_H

#include <vector>
#include <limits>

using namespace std;


// A real suit has up to 13 cards, but let's see if we can solve more.
#define MAX_CARDS 15

#define CARD_NAMES "23456789TJQKABC"

#define GENERIC_NAMES "HhGgIiJj"

enum Side
{
  POSITION_NORTH = 0,
  POSITION_SOUTH = 1,
  POSITION_OPPS = 2
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

enum Level
{
  LEVEL_LEAD = 0,
  LEVEL_LHO = 1,
  LEVEL_PARD = 2,
  LEVEL_RHO = 3,
  LEVEL_SIZE = 4
};

enum WinningSide
{
  WIN_NORTH = 0,
  WIN_SOUTH = 1,
  WIN_EITHER = 2,
  WIN_NONE = 3
};

enum WinnerCompare
{
  WIN_FIRST = 0,
  WIN_SECOND = 1,
  WIN_EQUAL = 2,
  WIN_DIFFERENT = 3,
  WIN_UNSET = 4
};

enum CombinationType
{
  COMB_TRIVIAL = 0,
  COMB_CONST = 1,
  COMB_SIMPLE = 2,
  COMB_OTHER = 3
};

enum Compare
{
  COMPARE_LESS_THAN = 0,
  COMPARE_EQUAL = 1,
  COMPARE_GREATER_THAN = 2,
  COMPARE_INCOMMENSURATE = 3
};


const unsigned UNSIGNED_NOT_SET = numeric_limits<unsigned>::max();

#define UNUSED(x) ((void)(true ? 0 : ((x), void(), 0)))

#endif

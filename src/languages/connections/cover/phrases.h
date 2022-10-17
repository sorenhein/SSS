/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PHRASES_H
#define SSS_PHRASES_H

#include "../../VerbalConnection.h"
#include "../../PhraseExpansion.h"

enum PhrasesEnum: unsigned
{
  PLAYER_WEST = 0,
  PLAYER_EAST = 1,
  PLAYER_EITHER = 2,
  PLAYER_EACH = 3,
  PLAYER_NEITHER = 4,
  PLAYER_SUIT = 5,

  LENGTH_VERB_VOID = 10,
  LENGTH_VERB_NOT_VOID = 11,
  LENGTH_VERB_XTON = 12,
  LENGTH_VERB_EVENLY = 13,
  LENGTH_VERB_ODD_EVENLY = 14,
  LENGTH_VERB_XTON_ATMOST = 15,
  LENGTH_VERB_CARDS = 16,
  LENGTH_VERB_ATMOST = 17,
  LENGTH_VERB_12 = 18,
  LENGTH_VERB_RANGE = 19,
  LENGTH_VERB_BETWEEN = 20,
  LENGTH_VERB_SPLIT = 21,

  LENGTH_ORDINAL_EXACT = 30,
  LENGTH_ORDINAL_ATMOST = 31,
  LENGTH_ORDINAL_ADJACENT = 32,
  LENGTH_ORDINAL_RANGE = 33,

  COUNT_EXACT = 40,
  COUNT_ATMOST = 41,
  COUNT_ATLEAST = 42,
  COUNT_RANGE = 43,
  COUNT_OR = 44,

  TOPS_RANGE = 53,
  TOPS_ACTUAL = 54,
  TOPS_SOME_ACTUAL = 55,
  TOPS_FULL_ACTUAL = 56,
  TOPS_RANKS = 57,
  TOPS_LOWER = 58,

  BOTH_ONE_PLAYER = 60,
  BOTH_EITHER_PLAYER = 61,
  BOTH_ONE_PLAYER_INDEF_LENGTH = 62,
  BOTH_ONE_PLAYER_SET_LENGTH = 63,
  ONE_PLAYER_SET = 64,
  ONE_PLAYER_UNSET = 65,

  BELOW_NORMAL = 70,
  BELOW_COMPLETELY = 71,

  BOTTOMS_NORMAL = 80,

  LIST_HOLDING_EXACT = 90,

  EITHER_WAY = 100,
  ONE_WAY = 101,

  HONORS_ONE = 110,
  HONORS_MULTIPLE = 111,
  HONORS_12 = 112,
  HONORS_RANGE = 113,

  DIGITS_RANGE = 120,

  OF_DEFINITE_RANK = 130,

  VERBAL_PHRASE_SIZE = 140
};

enum PhrasesGroup: unsigned
{
  GROUP_PHRASES_PLAYER = 0,
  GROUP_PHRASES_LENGTH_VERB = 1,
  GROUP_PHRASES_LENGTH_ORDINAL = 2,
  GROUP_PHRASES_COUNT = 3,
  GROUP_PHRASES_DIGITS = 4,
  GROUP_PHRASES_TOPS = 5,
  GROUP_PHRASES_BOTH_SIDES = 6,
  GROUP_PHRASES_BELOW = 7,
  GROUP_PHRASES_BOTTOMS = 8,
  GROUP_PHRASES_LIST = 9,
  GROUP_PHRASES_EITHER_WAY = 10,
  GROUP_PHRASES_HONORS = 11

};

#endif

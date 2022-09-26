/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALBLANK_H
#define SSS_VERBALBLANK_H


using namespace std;

#define BLANK_MAX_VERSIONS 15

enum VerbalBlank: unsigned
{
  BLANK_LENGTH_VERB = 0,
  BLANK_LENGTH_ADJ = 1,
  BLANK_TOPS = 2,
  BLANK_TOPS_PHRASE = 3,
  BLANK_BOTTOMS = 4,
  BLANK_PLAYER_CAP = 5,
  BLANK_EXCLUDING = 6,
  BLANK_BELOW = 7,
  BLANK_LIST_PHRASE = 8,
  BLANK_SIZE = 9
};


enum BlankPlayerCap: unsigned
{
  BLANK_PLAYER_CAP_WEST = 0,
  BLANK_PLAYER_CAP_EAST = 1,
  BLANK_PLAYER_CAP_EITHER = 2,
  BLANK_PLAYER_CAP_EACH = 3,
  BLANK_PLAYER_CAP_NEITHER = 4,
  BLANK_PLAYER_CAP_SUIT = 5
};

enum BlankLengthVerb: unsigned
{
  BLANK_LENGTH_VERB_VOID = 0,
  BLANK_LENGTH_VERB_SINGLE = 1,
  BLANK_LENGTH_VERB_DOUBLE = 2,
  BLANK_LENGTH_VERB_TRIPLE = 3,
  BLANK_LENGTH_VERB_EVENLY = 4,
  BLANK_LENGTH_VERB_ODD_EVENLY = 5,
  BLANK_LENGTH_VERB_SINGLE_ATMOST = 6,
  BLANK_LENGTH_VERB_DOUBLE_ATMOST = 7,
  BLANK_LENGTH_VERB_TRIPLE_ATMOST = 8,
  BLANK_LENGTH_VERB_CARDS_PARAM = 9,
  BLANK_LENGTH_VERB_CARDS_ATMOST_PARAM = 10,
  BLANK_LENGTH_VERB_RANGE_PARAMS = 11,
  BLANK_LENGTH_VERB_SPLIT_PARAMS = 12
};

enum BlankLengthAdj: unsigned
{
  BLANK_LENGTH_ADJ_SINGLE = 0,
  BLANK_LENGTH_ADJ_DOUBLE = 1,
  BLANK_LENGTH_ADJ_TRIPLE = 2,
  BLANK_LENGTH_ADJ_LONG = 3,
  BLANK_LENGTH_ADJ_SINGLE_ATMOST = 4,
  BLANK_LENGTH_ADJ_DOUBLE_ATMOST = 5,
  BLANK_LENGTH_ADJ_TRIPLE_ATMOST = 6,
  BLANK_LENGTH_ADJ_LONG_ATMOST = 7,
  BLANK_LENGTH_ADJ_23 = 8
};

enum BlankTops: unsigned
{
  BLANK_TOPS_ONE_ATMOST = 0,
  BLANK_TOPS_ONE_ATLEAST = 1,
  BLANK_TOPS_ONE_RANGE_PARAMS = 2,
  BLANK_TOPS_ACTUAL = 3
};

enum BlankBottoms: unsigned
{
  BLANK_BOTTOMS_NORMAL = 0
};

enum BlankExcluding: unsigned
{
  BLANK_EXCLUDING_NONE = 0,
  BLANK_EXCLUDING_NEITHER = 1,
  BLANK_EXCLUDING_NOT = 2
};

enum BlankBelow: unsigned
{
  BLANK_BELOW_NORMAL = 0,
  BLANK_BELOW_COMPLETELY = 1
};

enum BlankTopsPhrase: unsigned
{
  BLANK_TOPS_PHRASE_HOLDING = 0
};

enum BlankListPhrase: unsigned
{
  BLANK_LIST_PHRASE_HOLDING = 0
};

#endif

/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALBLANK_H
#define SSS_VERBALBLANK_H


using namespace std;

#define BLANK_MAX_VERSIONS 12

enum VerbalBlank: unsigned
{
  BLANK_LENGTH_BAREWORD = 0,
  BLANK_LENGTH_PHRASE = 1,

  BLANK_PLAYER_CAP = 2,

  BLANK_SIZE = 3
};


enum BlankPlayerCap
{
  BLANK_PLAYER_CAP_WEST = 0,
  BLANK_PLAYER_CAP_EAST = 1,
  BLANK_PLAYER_CAP_EITHER = 2,
  BLANK_PLAYER_CAP_EACH = 3,
  BLANK_PLAYER_CAP_NEITHER = 4,
  BLANK_PLAYER_CAP_SUIT = 5
};

enum BlankLengthPhrase
{
  BLANK_LENGTH_PHRASE_VOID = 0,
  BLANK_LENGTH_PHRASE_SINGLE = 1,
  BLANK_LENGTH_PHRASE_DOUBLE = 2,
  BLANK_LENGTH_PHRASE_TRIPLE = 3,
  BLANK_LENGTH_PHRASE_EVENLY = 4,
  BLANK_LENGTH_PHRASE_SINGLE_ATMOST = 5,
  BLANK_LENGTH_PHRASE_DOUBLE_ATMOST = 6,
  BLANK_LENGTH_PHRASE_TRIPLE_ATMOST = 7,
  BLANK_LENGTH_PHRASE_CARDS_PARAM = 8,
  BLANK_LENGTH_PHRASE_RANGE_PARAMS = 9,
  BLANK_LENGTH_PHRASE_SPLIT_PARAMS = 10
};

#endif

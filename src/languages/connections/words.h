/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_WORDS_H
#define SSS_WORDS_H

enum WordsEnum: unsigned
{
  WORDS_CARD = 0,
  WORDS_CARDS = 1,
  WORDS_HONOR = 2,
  WORDS_HONORS = 3,
  WORDS_SMALL = 4,
  WORDS_CONJUNCTION = 5
};

enum WordsGroups: unsigned
{
  GROUP_CARDS = 0,
  GROUP_HONORS = 1,
  GROUP_SMALL = 2,
  GROUP_CONJUNCTIONS = 3
};

#endif

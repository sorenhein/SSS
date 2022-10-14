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
  WORDS_MID_HONOR = 4,
  WORDS_MID_HONORS = 5,
  WORDS_SMALL = 6,
  WORDS_CONJUNCTION = 7,
  WORDS_PARTICLE_DEF_PLURAL = 8,
  WORDS_DEF_PLURAL_OF = 9,
  WORDS_THEY_HOLD = 10
};

enum WordsGroups: unsigned
{
  GROUP_CARDS = 0,
  GROUP_HONORS = 1,
  GROUP_SMALL = 2,
  GROUP_CONJUNCTIONS = 3,
  GROUP_PARTICLES = 4,
  GROUP_PREPOSITIONS = 5,
  GROUP_THEY_HOLD = 6
};

#endif

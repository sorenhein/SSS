/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PHRASEEXPANSION_H
#define SSS_PHRASEEXPANSION_H


enum PhraseExpansion: unsigned
{
  PHRASE_NONE = 0,
  PHRASE_DIGITS = 1,
  PHRASE_NUMERICAL = 2,
  PHRASE_ORDINAL = 3,

  PHRASE_LOWEST_CARD = 4,

  PHRASE_INDEFINITE_RANK = 5,
  PHRASE_DEFINITE_RANK = 6,
  PHRASE_OF_DEFINITE_RANK = 7,

  PHRASE_COMPLETION_SET = 8,
  PHRASE_COMPLETION_UNSET = 9,
  PHRASE_XES = 10,

  PHRASE_SIZE = 11
};

#endif

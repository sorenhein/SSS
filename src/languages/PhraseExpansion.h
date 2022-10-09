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
  PHRASE_NUMERICAL = 1,
  PHRASE_ORDINAL = 2,
  PHRASE_RANKS = 3,
  PHRASE_TEXT_LOWER = 4,
  PHRASE_TEXT_BELOW = 5,
  PHRASE_RANGE_OF = 6,
  PHRASE_SOME_OF = 7,
  PHRASE_COMPLETION_SET = 8,
  PHRASE_COMPLETION_BOTH = 9,
  PHRASE_COMPLETION_XES = 10,
  PHRASE_SIZE = 11
};

#endif

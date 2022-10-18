/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SENTENCES_H
#define SSS_SENTENCES_H

#include "../../VerbalConnection.h"
#include "../../PhraseExpansion.h"

enum SentencesEnum: unsigned
{
  SENTENCE_LENGTH = 0,
  SENTENCE_TOPS = 1,
  SENTENCE_COUNT_TOPS = 2,
  SENTENCE_EXACTLY_COUNT_TOPS = 3,
  SENTENCE_TOPS_ORDINAL = 4,
  SENTENCE_COUNT_TOPS_ORDINAL = 5,
  SENTENCE_EXACTLY_COUNT_TOPS_ORDINAL = 6,
  SENTENCE_TOPS_AND_LOWER = 7,
  SENTENCE_EXACTLY_TOPS_AND_LOWER = 8,
  SENTENCE_TOPS_AND_COUNT_BELOW_CARD = 9,
  SENTENCE_EXACTLY_TOPS_MAYBE_UNSET = 10,

  SENTENCE_TOPS_BOTH_SYMM = 11,
  SENTENCE_TOPS_BOTH_NOT_SYMM = 12,
  SENTENCE_TOPS_AND_XES = 13,
  SENTENCE_ONLY_BELOW = 14,
  SENTENCE_SINGULAR_EXACT = 15,
  SENTENCE_HONORS_ORDINAL = 16,
  SENTENCE_LIST = 17,
  SENTENCE_TOPS_AND_BELOW_NEW = 19,
  SENTENCE_TOPS_BOTH_LENGTH = 20,
  SENTENCE_SIZE = 21
};

enum SentencesGroup: unsigned
{
  GROUP_SENTENCES = 0
};

#endif

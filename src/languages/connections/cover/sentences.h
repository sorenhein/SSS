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
  SENTENCE_LENGTH_ONLY = 0,
  SENTENCE_ONETOP_ONLY = 1,
  SENTENCE_COUNT_ONETOP = 2,
  SENTENCE_TOPS_LENGTH = 3,
  SENTENCE_TOPS_LENGTH_OLD = 4,
  SENTENCE_TOPS_BOTH_LENGTH = 5,
  SENTENCE_TOPS_BOTH_SYMM = 6,
  SENTENCE_TOPS_BOTH_NOT_SYMM = 7,
  SENTENCE_TOPS_AND_XES = 8,
  SENTENCE_TOPS_AND_LOWER = 9,
  SENTENCE_TOPS_AND_BELOW = 10,
  SENTENCE_ONLY_BELOW = 11,
  SENTENCE_SINGULAR_EXACT = 12,
  SENTENCE_HONORS_ORDINAL = 13,
  SENTENCE_LIST = 14,
  SENTENCE_SET_UNSET = 15,
  SENTENCE_SIZE = 16
};

enum SentencesGroup: unsigned
{
  GROUP_SENTENCES = 0
};

#endif

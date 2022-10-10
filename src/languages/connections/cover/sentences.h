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
  SENTENCE_TOPS_LENGTH = 2,
  SENTENCE_TOPS_LENGTH_WITHOUT = 3,
  SENTENCE_TOPS_EXCLUDING = 4,
  SENTENCE_TOPS_AND_XES = 5,
  SENTENCE_TOPS_AND_LOWER = 6,
  SENTENCE_ONLY_BELOW = 7,
  SENTENCE_LIST = 8,
  SENTENCE_SIZE = 9
};

enum SentencesGroup: unsigned
{
  GROUP_SENTENCES = 0
};

#endif

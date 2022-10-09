/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_WORDS_H
#define SSS_WORDS_H

#include "../VerbalConnection.h"
#include "../PhraseExpansion.h"

enum WordsEnum: unsigned
{
  WORDS_CARD = 0,
  WORDS_CARDS = 1,
  WORDS_HONOR = 2,
  WORDS_HONORS = 3,
  WORDS_CONJUNCTION = 4
};

enum WordsGroups: unsigned
{
  GROUP_CARDS = 0,
  GROUP_HONORS = 1,
  GROUP_CONJUNCTIONS = 2
};


list<VerbalConnection> wordsConnection =
{
  { WORDS_CARD, "CARD", GROUP_CARDS, PHRASE_NONE },
  { WORDS_CARDS, "CARDS", GROUP_CARDS, PHRASE_NONE },

  { WORDS_HONOR, "HONOR", GROUP_HONORS, PHRASE_NONE },
  { WORDS_HONORS, "HONORS", GROUP_HONORS, PHRASE_NONE },

  { WORDS_CONJUNCTION, "CONJUNCTION_OR", GROUP_CONJUNCTIONS, PHRASE_NONE }
};

#endif

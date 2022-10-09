/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CARDS_SHORT_H
#define SSS_CARDS_SHORT_H

#include "../../VerbalConnection.h"
#include "../../PhraseExpansion.h"

enum ShortEnum: unsigned
{
  SHORT_2 = 0,
  SHORT_3 = 1,
  SHORT_4 = 2,
  SHORT_5 = 3,
  SHORT_6 = 4,
  SHORT_7 = 5,
  SHORT_8 = 6,
  SHORT_9 = 7,
  SHORT_TEN = 8,
  SHORT_JACK = 9,
  SHORT_QUEEN = 10,
  SHORT_KING = 11,
  SHORT_ACE = 12
};

enum ShortGroups: unsigned
{
  GROUP_SHORT = 0
};


list<VerbalConnection> cardsShortConnection =
{
  { SHORT_2, "CARD_SHORT_2", GROUP_SHORT, PHRASE_NONE },
  { SHORT_3, "CARD_SHORT_3", GROUP_SHORT, PHRASE_NONE },
  { SHORT_4, "CARD_SHORT_4", GROUP_SHORT, PHRASE_NONE },
  { SHORT_5, "CARD_SHORT_5", GROUP_SHORT, PHRASE_NONE },
  { SHORT_6, "CARD_SHORT_6", GROUP_SHORT, PHRASE_NONE },
  { SHORT_7, "CARD_SHORT_7", GROUP_SHORT, PHRASE_NONE },
  { SHORT_8, "CARD_SHORT_8", GROUP_SHORT, PHRASE_NONE },
  { SHORT_9, "CARD_SHORT_9", GROUP_SHORT, PHRASE_NONE },
  { SHORT_TEN, "CARD_SHORT_TEN", GROUP_SHORT, PHRASE_NONE },
  { SHORT_JACK, "CARD_SHORT_JACK", GROUP_SHORT, PHRASE_NONE },
  { SHORT_QUEEN, "CARD_SHORT_QUEEN", GROUP_SHORT, PHRASE_NONE },
  { SHORT_KING, "CARD_SHORT_KING", GROUP_SHORT, PHRASE_NONE },
  { SHORT_ACE, "CARD_SHORT_ACE", GROUP_SHORT, PHRASE_NONE }
};

#endif

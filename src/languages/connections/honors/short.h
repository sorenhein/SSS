/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_HONORS_SHORT_H
#define SSS_HONORS_SHORT_H

#include "../../VerbalConnection.h"
#include "../../PhraseExpansion.h"

enum HonorShortEnum: unsigned
{
  HONOR_SHORT_1 = 0,
  HONOR_SHORT_2 = 1,
  HONOR_SHORT_3 = 2,
  HONOR_SHORT_4 = 3,
  HONOR_SHORT_5 = 4,
  HONOR_SHORT_6 = 5
};

enum HonorShortGroups: unsigned
{
  GROUP_HONOR_SHORT = 0
};


list<VerbalConnection> honorsShortConnection =
{
  { HONOR_SHORT_1, "CARD_SHORT_1", GROUP_HONOR_SHORT, PHRASE_NONE },
  { HONOR_SHORT_2, "CARD_SHORT_2", GROUP_HONOR_SHORT, PHRASE_NONE },
  { HONOR_SHORT_3, "CARD_SHORT_3", GROUP_HONOR_SHORT, PHRASE_NONE },
  { HONOR_SHORT_4, "CARD_SHORT_4", GROUP_HONOR_SHORT, PHRASE_NONE },
  { HONOR_SHORT_5, "CARD_SHORT_5", GROUP_HONOR_SHORT, PHRASE_NONE },
  { HONOR_SHORT_6, "CARD_SHORT_6", GROUP_HONOR_SHORT, PHRASE_NONE }
};

#endif

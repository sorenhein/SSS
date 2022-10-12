/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PREPOSITION_H
#define SSS_PREPOSITION_H

#include "../../VerbalConnection.h"
#include "../../PhraseExpansion.h"

enum PrepositionEnum: unsigned
{
  PREP_OF_2 = 0,
  PREP_OF_3 = 1,
  PREP_OF_4 = 2,
  PREP_OF_5 = 3,
  PREP_OF_6 = 4,
  PREP_OF_7 = 5,
  PREP_OF_8 = 6,
  PREP_OF_9 = 7,
  PREP_OF_TEN = 8,
  PREP_OF_JACK = 9,
  PREP_OF_QUEEN = 10,
  PREP_OF_KING = 11,
  PREP_OF_ACE = 12
};

enum PrepositionGroups: unsigned
{
  GROUP_PREP_LOCAL = 0
};


list<VerbalConnection> prepositionConnection =
{
  { PREP_OF_2, "PREP_OF_2", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_3, "PREP_OF_3", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_4, "PREP_OF_4", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_5, "PREP_OF_5", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_6, "PREP_OF_6", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_7, "PREP_OF_7", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_8, "PREP_OF_8", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_9, "PREP_OF_9", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_TEN, "PREP_OF_TEN", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_JACK, "PREP_OF_JACK", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_QUEEN, "PREP_OF_QUEEN", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_KING, "PREP_OF_KING", GROUP_PREP_LOCAL, PHRASE_NONE },
  { PREP_OF_ACE, "PREP_OF_ACE", GROUP_PREP_LOCAL, PHRASE_NONE }
};

#endif

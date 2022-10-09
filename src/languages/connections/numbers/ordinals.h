/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_ORDINALS_H
#define SSS_ORDINALS_H

#include "../../VerbalConnection.h"
#include "../../PhraseExpansion.h"

enum OrdinalsEnum: unsigned
{
  ORDINAL_0 = 0,
  ORDINAL_1 = 1,
  ORDINAL_2 = 2,
  ORDINAL_3 = 3,
  ORDINAL_4 = 4,
  ORDINAL_5 = 5,
  ORDINAL_6 = 6,
  ORDINAL_7 = 7,
  ORDINAL_8 = 8,
  ORDINAL_9 = 9,
  ORDINAL_10 = 10,
  ORDINAL_11 = 11,
  ORDINAL_12 = 12,
  ORDINAL_13 = 13
};

enum OrdinalGroups: unsigned
{
  GROUP_ORDINAL = 0
};


list<VerbalConnection> ordinalsConnection =
{
  { ORDINAL_0, "ORDINAL_0", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_1, "ORDINAL_1", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_2, "ORDINAL_2", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_3, "ORDINAL_3", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_4, "ORDINAL_4", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_5, "ORDINAL_5", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_6, "ORDINAL_6", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_7, "ORDINAL_7", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_8, "ORDINAL_8", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_9, "ORDINAL_9", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_10, "ORDINAL_10", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_11, "ORDINAL_11", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_12, "ORDINAL_12", GROUP_ORDINAL, PHRASE_NONE },
  { ORDINAL_13, "ORDINAL_13", GROUP_ORDINAL, PHRASE_NONE }
};

#endif

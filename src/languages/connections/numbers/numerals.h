/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_NUMERALS_H
#define SSS_NUMERALS_H

#include "../../VerbalConnection.h"
#include "../../PhraseExpansion.h"

enum NumeralsEnum: unsigned
{
  NUMERAL_0 = 0,
  NUMERAL_1 = 1,
  NUMERAL_2 = 2,
  NUMERAL_3 = 3,
  NUMERAL_4 = 4,
  NUMERAL_5 = 5,
  NUMERAL_6 = 6,
  NUMERAL_7 = 7,
  NUMERAL_8 = 8,
  NUMERAL_9 = 9,
  NUMERAL_10 = 10,
  NUMERAL_11 = 11,
  NUMERAL_12 = 12,
  NUMERAL_13 = 13
};

enum NumeralGroups: unsigned
{
  GROUP_NUMERAL = 0
};


list<VerbalConnection> numeralsConnection =
{
  { NUMERAL_0, "NUMERAL_0", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_1, "NUMERAL_1", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_2, "NUMERAL_2", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_3, "NUMERAL_3", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_4, "NUMERAL_4", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_5, "NUMERAL_5", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_6, "NUMERAL_6", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_7, "NUMERAL_7", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_8, "NUMERAL_8", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_9, "NUMERAL_9", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_10, "NUMERAL_10", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_11, "NUMERAL_11", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_12, "NUMERAL_12", GROUP_NUMERAL, PHRASE_NONE },
  { NUMERAL_13, "NUMERAL_13", GROUP_NUMERAL, PHRASE_NONE }
};

#endif

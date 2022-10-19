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

#endif

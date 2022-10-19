/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CARDS_DEFINITE_H
#define SSS_CARDS_DEFINITE_H

#include "../../VerbalConnection.h"
#include "../../PhraseExpansion.h"

enum DefiniteEnum: unsigned
{
  DEFINITE_2 = 0,
  DEFINITE_3 = 1,
  DEFINITE_4 = 2,
  DEFINITE_5 = 3,
  DEFINITE_6 = 4,
  DEFINITE_7 = 5,
  DEFINITE_8 = 6,
  DEFINITE_9 = 7,
  DEFINITE_TEN = 8,
  DEFINITE_JACK = 9,
  DEFINITE_QUEEN = 10,
  DEFINITE_KING = 11,
  DEFINITE_ACE = 12
};

enum DefiniteGroups: unsigned
{
  GROUP_DEFINITE = 0
};

#endif

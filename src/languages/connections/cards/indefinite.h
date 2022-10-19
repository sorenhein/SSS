/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CARDS_INDEFINITE_H
#define SSS_CARDS_INDEFINITE_H

#include "../../VerbalConnection.h"
#include "../../PhraseExpansion.h"

enum IndefiniteEnum: unsigned
{
  INDEFINITE_2 = 0,
  INDEFINITE_3 = 1,
  INDEFINITE_4 = 2,
  INDEFINITE_5 = 3,
  INDEFINITE_6 = 4,
  INDEFINITE_7 = 5,
  INDEFINITE_8 = 6,
  INDEFINITE_9 = 7,
  INDEFINITE_TEN = 8,
  INDEFINITE_JACK = 9,
  INDEFINITE_QUEEN = 10,
  INDEFINITE_KING = 11,
  INDEFINITE_ACE = 12
};

enum IndefiniteGroups: unsigned
{
  GROUP_INDEFINITE = 0
};

#endif

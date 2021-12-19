/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CRANKS_H
#define SSS_CRANKS_H

struct Play;


// Functions to set constants for Ranks.

void setRankConstants23();

void setRankConstants4();

unsigned uncanonicalTrinary(
  const unsigned holding4,
  const Play& play);

void uncanonicalBoth(
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2);

#endif

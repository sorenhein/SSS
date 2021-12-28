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

void orientedBoth(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2);

void orientedTrinary(
  const unsigned cards,
  const unsigned holding4,
  const Play& play,
  unsigned& holding3,
  bool& rotateFlag);

unsigned uncanonicalBinary(const unsigned holding4);

unsigned minimalizeRanked(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned oppsCount,
  const unsigned northCount,
  const unsigned southCount,
  const unsigned& holding4);

#endif

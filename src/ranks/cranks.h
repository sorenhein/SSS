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

unsigned holding3_to_holding4(const unsigned holding3);

void orientedBoth(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2);

unsigned orientedTrinary(
  const unsigned cards,
  const unsigned holding4,
  const Play& play);

unsigned uncanonicalTrinary(
  const unsigned holding4,
  const Play& play);

unsigned uncanonicalBinary(const unsigned holding4);

void uncanonicalBoth(
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2);

unsigned minimalizeRanked(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned oppsCount,
  const unsigned northCount,
  const unsigned southCount,
  const unsigned& holding4);

#endif

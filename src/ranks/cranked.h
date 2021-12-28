/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CRANKED_H
#define SSS_CRANKED_H

struct Play;


// Functions for ranked calculations in Ranks.

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

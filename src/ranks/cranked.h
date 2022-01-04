/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CRANKED_H
#define SSS_CRANKED_H

struct Play;


// Functions for ranked calculations in Ranks.

void setRankedConstants();

void rankedTrinary(
  const unsigned cards,
  const unsigned holding4,
  const Play& play,
  unsigned& holding3,
  bool& rotateFlag);

void rankedBoth(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned holding4,
  unsigned& holding3,
  unsigned& holding2);

unsigned rankedMinimalize(
  const bool rotateFlag,
  const unsigned cards,
  const unsigned oppsCount,
  const unsigned northCount,
  const unsigned southCount,
  const unsigned& holding4);

#endif

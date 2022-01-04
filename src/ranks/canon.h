/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CANON_H
#define SSS_CANON_H

class Declarer;
class Opponents;


// Functions for canonical calculations in Ranks.

void setCanonicalConstants();

unsigned canonicalTrinary(
  const Declarer& dominant,
  const Declarer& recessive,
  const Opponents& opponents,
  const unsigned char maxGlobalRank);

void canonicalBoth(
  const Declarer& dominant,
  const Declarer& recessive,
  const Opponents& opponents,
  const unsigned char maxGlobalRank,
  unsigned& holding3,
  unsigned& holding2);

#endif

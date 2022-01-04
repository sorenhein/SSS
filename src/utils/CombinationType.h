/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMBINATIONTYPE_H
#define SSS_COMBINATIONTYPE_H

#include <vector>
#include <string>

using namespace std;


// If this ever exceeds 3 bits, we have a problem in CombFiles
enum CombinationType
{
  COMB_NON_MINIMAL = 0,
  COMB_CONSTANT = 1,
  COMB_SINGLE_STRAT = 2,
  COMB_MULT_VOID_ASYMM = 3,
  COMB_MULT_VOID_SYMM = 4,
  COMB_MULT_NON_VOID = 5,
  COMB_SIZE = 6
};

const vector<string> CombinationNames =
{
  "Non-min",
  "Const",
  "Single",
  "AsymmV",
  "SymmV",
  "Non-vd"
};

#endif

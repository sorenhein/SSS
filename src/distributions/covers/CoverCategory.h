/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERCATEGORY_H
#define SSS_COVERCATEGORY_H


using namespace std;


enum CoverSymmetry: unsigned
{
  EXPLAIN_SYMMETRIC = 0,
  EXPLAIN_ANTI_SYMMETRIC = 1,
  EXPLAIN_GENERAL = 2,
  EXPLAIN_TRIVIAL = 3,
  EXPLAIN_SYMMETRY_UNSET = 4
};

enum CoverComposition: unsigned
{
  EXPLAIN_LENGTH_ONLY = 0,
  EXPLAIN_TOPS_ONLY = 1,
  EXPLAIN_MIXED_TERMS = 2,
  EXPLAIN_COMPOSITION_UNSET = 3
};

#endif

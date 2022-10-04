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

enum CoverVerbal: unsigned
{
  VERBAL_GENERAL = 0,
  VERBAL_HEURISTIC = 1,
  VERBAL_LENGTH_ONLY = 2,
  VERBAL_ONE_TOP_ONLY = 3,
  VERBAL_LENGTH_AND_ONE_TOP = 4,
  VERBAL_HIGH_TOPS_EQUAL = 5,
  VERBAL_ANY_TOPS_EQUAL = 6,
  VERBAL_SINGULAR = 7
};

struct CoverLength
{
  bool flag;
  unsigned char length;
};

#endif

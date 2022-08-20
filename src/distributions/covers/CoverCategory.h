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
  VERBAL_GENERAL = 0x001,
  VERBAL_HEURISTIC = 0x002,
  VERBAL_LENGTH_ONLY = 0x004,
  VERBAL_TOPS_ONLY = 0x008,
  VERBAL_LENGTH_AND_ONE_TOP = 0x010,
  VERBAL_HIGH_TOPS_EQUAL = 0x020,
  VERBAL_ANY_TOPS_EQUAL = 0x040,
  VERBAL_SINGULAR = 0x080
};

struct CoverLength
{
  bool flag;
  unsigned char length;
};

#endif
